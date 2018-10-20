#include <Engine3D/engine3D_time.h>
#include <Engine3D/engine3D_util.h>
#include <Engine3D/engine3D_mesh.h>
#include <Engine3D/engine3D_basicShader.h>
#include <Engine3D/engine3D_texture.h>
#include <Engine3D/engine3D_material.h>

#include <wfstn3D_monster.h>
#include <wfstn3D_level.h>

#include <stdbool.h>
#include <stdio.h>

#include <math.h>

#define SCALE (0.7f)

#define START (0.0f)
#define SIZEY SCALE
#define SIZEX (SIZEY / (2.0f * 1.93103f))

#define TEX_MIN_X (0.0f)
#define TEX_MAX_X (-1.0f)
#define TEX_MIN_Y (0.0f)
#define TEX_MAX_Y (-1.0f)

#define MOVE_SPEED (1.0f)
#define MOVE_STOP_DISTANCE (1.5f)
#define MONSTER_WIDTH (0.1f)
#define MONSTER_LENGTH (0.1f)

#define SHOOT_DISTANCE (1000.0f)

static engine3D_mesh_t mesh;
static bool meshIsLoaded = false;

void wfstn3D_monster_init(wfstn3D_monster_t *const monster, const engine3D_transform_t *const transform, wfstn3D_level_t *const level) {
	memcpy(&monster->transform, transform, sizeof(engine3D_transform_t));

	monster->level = level;

	monster->state = WFSTN3D_MONSTER_STATE_ATTACKING;

	monster->material = engine3D_util_safeMalloc(sizeof(engine3D_material_t));
	monster->material->color = engine3D_util_safeMalloc(sizeof(engine3D_vector3f_t));
	monster->material->texture = engine3D_util_safeMalloc(sizeof(engine3D_texture_t));
	monster->material->color->x = 1;
	monster->material->color->y = 1;
	monster->material->color->z = 1;
	engine3D_resourceLoader_loadTexture("SSWVA1.png", monster->material->texture);
	monster->material->specularIntensity = 0;
	monster->material->specularPower = 0;

	if (!meshIsLoaded) {
		engine3D_vertex_t vertices[4] = { { { -SIZEX,START,START },{ TEX_MAX_X,TEX_MAX_Y },{ 0,0,0 } },
		                                  { { -SIZEX,SIZEY,START },{ TEX_MAX_X,TEX_MIN_Y },{ 0,0,0 } },
		                                  { {  SIZEX,SIZEY,START },{ TEX_MIN_X,TEX_MIN_Y },{ 0,0,0 } },
		                                  { {  SIZEX,START,START },{ TEX_MIN_X,TEX_MAX_Y },{ 0,0,0 } } };

		unsigned int indices[6] = { 0, 1, 2,
			                         0, 2, 3 };

		engine3D_mesh_init(&mesh);
		engine3D_mesh_addVertices(&mesh, vertices, 4, indices, 6, true);

		meshIsLoaded = true;
	}
}

void wfstn3D_monster_input(wfstn3D_monster_t *const monster) {
}

void idleUpdate(wfstn3D_monster_t *const monster, const engine3D_vector3f_t *const orientation, float distance) {

}

void chasingUpdate(wfstn3D_monster_t *const monster, const engine3D_vector3f_t *const orientation, float distance) {
	engine3D_vector3f_t tmp, newPos, movementVector, collisionVector;
	if (distance > MOVE_STOP_DISTANCE) {
		float movAmnt = -MOVE_SPEED * (float)engine3D_time_getDelta();

		engine3D_vector3f_add(&monster->transform.translation, engine3D_vector3f_mulf(orientation, movAmnt, &tmp), &newPos);
		wfstn3D_level_checkCollision(&monster->transform.translation, &newPos, MONSTER_WIDTH, MONSTER_LENGTH, &collisionVector, monster->level);
		engine3D_vector3f_mul(&collisionVector, orientation, &movementVector);

		if (engine3D_vector3f_length(&movementVector) > 0) {
			engine3D_vector3f_add(&monster->transform.translation, engine3D_vector3f_mulf(&movementVector, movAmnt, &tmp), &monster->transform.translation);
		}

		if (engine3D_vector3f_length(engine3D_vector3f_sub(&movementVector, orientation, &tmp)) != 0) {
			wfstn3D_level_openDoorsAt(&monster->transform.translation, monster->level);
		}
	}
}

void attackingUpdate(wfstn3D_monster_t *const monster, const engine3D_vector3f_t *const orientation, float distance) {
	engine3D_vector2f_t lineStart, castDirection, lineEnd, collisionVector = { 0,0 }, tmp;

	lineStart.x = monster->transform.translation.x;
	lineStart.y = monster->transform.translation.z;
	castDirection.x = orientation->x;
	castDirection.y = orientation->z;
	engine3D_vector2f_add(&lineStart, engine3D_vector2f_mulf(&castDirection, SHOOT_DISTANCE, &tmp), &lineEnd);

	wfstn3D_level_checkIntersections(monster->level, &lineStart, &lineEnd, &collisionVector);

	if (collisionVector.x != 0 || collisionVector.y != 0)
		fprintf(stderr, "Something got hit\n");
	else
		fprintf(stderr, "NANI?!\n");

	monster->state = WFSTN3D_MONSTER_STATE_CHASING;
}

void dyingUpdate(wfstn3D_monster_t *const monster, const engine3D_vector3f_t *const orientation, float distance) {

}

void deadUpdate(wfstn3D_monster_t *const monster, const engine3D_vector3f_t *const orientation, float distance) {

}

void alignWithGround(wfstn3D_monster_t *const monster) {
	monster->transform.translation.y = 0;
}

void faceCamera(wfstn3D_monster_t *const monster, const engine3D_vector3f_t *const direction) {
	float angle = atanf(direction->z / direction->x) / 3.14159265359f * 180.0f + 90.0f;
	if (direction->x > 0)
		angle += 180.0f;
	monster->transform.rotation.y = angle;
}

void wfstn3D_monster_update(wfstn3D_monster_t *const monster) {
	float distance;
	engine3D_vector3f_t directionToCamera, orientation;
	engine3D_vector3f_sub(&monster->transform.translation, &engine3D_transform_camera->pos, &directionToCamera);
	distance = engine3D_vector3f_length(&directionToCamera);
	engine3D_vector3f_divf(&directionToCamera, distance, &orientation);

	alignWithGround(monster);
	faceCamera(monster, &orientation);

	switch (monster->state) {
	case WFSTN3D_MONSTER_STATE_IDLE:
		idleUpdate(monster, &orientation, distance);
		break;
	case WFSTN3D_MONSTER_STATE_CHASING:
		chasingUpdate(monster, &orientation, distance);
		break;
	case WFSTN3D_MONSTER_STATE_ATTACKING:
		attackingUpdate(monster, &orientation, distance);
		break;
	case WFSTN3D_MONSTER_STATE_DYING:
		dyingUpdate(monster, &orientation, distance);
		break;
	case WFSTN3D_MONSTER_STATE_DEAD:
		deadUpdate(monster, &orientation, distance);
		break;
	default:
		monster->state = WFSTN3D_MONSTER_STATE_IDLE;
		break;
	}
}

void wfstn3D_monster_render(wfstn3D_monster_t *const monster) {
	engine3D_basicShader_t *shader = monster->level->shader;
	engine3D_matrix4f_t transformationMatrix, projectedTransformationMatrix;

	engine3D_transform_getTransformation(&monster->transform, &transformationMatrix);
	engine3D_transform_getProjectedTransformation(&monster->transform, &projectedTransformationMatrix);
	engine3D_basicShader_updateUniforms(shader, &transformationMatrix, &projectedTransformationMatrix, monster->material);

	engine3D_mesh_draw(&mesh);
}

void wfstn3D_monster_cleanup(wfstn3D_monster_t *const monster) {
	free(monster->material->color);
	free(monster->material->texture);
	free(monster->material);
}
