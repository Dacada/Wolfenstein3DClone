#include <Engine3D/engine3D_time.h>
#include <Engine3D/engine3D_util.h>
#include <Engine3D/engine3D_mesh.h>
#include <Engine3D/engine3D_basicShader.h>
#include <Engine3D/engine3D_texture.h>
#include <Engine3D/engine3D_material.h>
#include <Engine3D/engine3D_resourceLoader.h>

#include <wfstn3D_monster.h>
#include <wfstn3D_player.h>
#include <wfstn3D_level.h>

#include <stdbool.h>
#include <stdio.h>

#include <math.h>
#include <string.h>

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
#define MONSTER_WIDTH (0.3f)
#define MONSTER_LENGTH (0.3f)

#define SHOOT_DISTANCE (1000.0f)
#define SHOOT_ANGLE (10.0f)

#define ATTACK_CHANCE (0.5f)
#define SHOOT_DAMAGE_MIN (5)
#define SHOOT_DAMAGE_MAX (30)

#define MAX_HEALTH (100)

static engine3D_mesh_t mesh;
static bool meshIsLoaded = false;

static engine3D_texture_t *animations = NULL;
static size_t animationsLen;

void wfstn3D_monster_init(wfstn3D_monster_t *const monster, const engine3D_transform_t *const transform, wfstn3D_level_t *const level) {
	memcpy(&monster->transform, transform, sizeof(engine3D_transform_t));

	monster->level = level;

	monster->state = WFSTN3D_MONSTER_STATE_IDLE;

	monster->canLook = false;
	monster->canAttack = false;

	monster->health = MAX_HEALTH;

	monster->deathTime = 0;

	if (animations == NULL) {
		animationsLen = 13;
		animations = engine3D_util_safeMalloc(sizeof(engine3D_texture_t)*animationsLen);

		engine3D_resourceLoader_loadTexture("SSWVA1.png", animations + 0);
		engine3D_resourceLoader_loadTexture("SSWVB1.png", animations + 1);
		engine3D_resourceLoader_loadTexture("SSWVC1.png", animations + 2);
		engine3D_resourceLoader_loadTexture("SSWVD1.png", animations + 3);

		engine3D_resourceLoader_loadTexture("SSWVE0.png", animations + 4);
		engine3D_resourceLoader_loadTexture("SSWVF0.png", animations + 5);
		engine3D_resourceLoader_loadTexture("SSWVG0.png", animations + 6);

		engine3D_resourceLoader_loadTexture("SSWVH0.png", animations + 7);

		engine3D_resourceLoader_loadTexture("SSWVI0.png", animations + 8);
		engine3D_resourceLoader_loadTexture("SSWVJ0.png", animations + 9);
		engine3D_resourceLoader_loadTexture("SSWVK0.png", animations + 10);
		engine3D_resourceLoader_loadTexture("SSWVL0.png", animations + 11);

		engine3D_resourceLoader_loadTexture("SSWVM0.png", animations + 12);
	}

	monster->material = engine3D_util_safeMalloc(sizeof(engine3D_material_t));
	monster->material->color = engine3D_util_safeMalloc(sizeof(engine3D_vector3f_t));
	//monster->material->texture = engine3D_util_safeMalloc(sizeof(engine3D_texture_t));
	monster->material->color->x = 1;
	monster->material->color->y = 1;
	monster->material->color->z = 1;
	//engine3D_resourceLoader_loadTexture("SSWVA1.png", monster->material->texture);
	monster->material->specularIntensity = 0;
	monster->material->specularPower = 0;
	monster->material->texture = animations + 0;

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
	(void)monster;
}

void idleUpdate(wfstn3D_monster_t *const monster, const engine3D_vector3f_t *const orientation, float distance) {
	double time = engine3D_timer_getTime() / engine3D_timer_second;
	double timeDecimals = time - (double)((int)time);

	if (timeDecimals < 0.5) {
		monster->canLook = true;
		monster->material->texture = animations + 0;
	}
	else {
		monster->material->texture = animations + 1;

		if (monster->canLook) {
			engine3D_vector2f_t lineStart, castDirection, lineEnd, collisionVector, playerIntersectVector, tmp;

			lineStart.x = monster->transform.translation.x;
			lineStart.y = monster->transform.translation.z;
			castDirection.x = orientation->x;
			castDirection.y = orientation->z;

			engine3D_vector2f_add(&lineStart, engine3D_vector2f_mulf(&castDirection, SHOOT_DISTANCE, &tmp), &lineEnd);

			bool i1 = wfstn3D_level_checkIntersections(monster->level, &lineStart, &lineEnd, &collisionVector, false);
			engine3D_vector2f_t size = { WFSTN3D_PLAYER_SIZE ,WFSTN3D_PLAYER_SIZE }, pos = { engine3D_transform_camera->pos.x,engine3D_transform_camera->pos.z };
			bool i2 = wfstn3D_level_lineIntersectRect(monster->level, &lineStart, &lineEnd, &pos, &size, &playerIntersectVector);

			if (i2 && (!i1 || engine3D_vector2f_length(engine3D_vector2f_sub(&playerIntersectVector, &lineStart, &tmp)) < engine3D_vector2f_length(engine3D_vector2f_sub(&collisionVector, &lineStart, &tmp)))) {
				monster->state = WFSTN3D_MONSTER_STATE_CHASING;
			}

			monster->canLook = false;
		}
	}
}

void chasingUpdate(wfstn3D_monster_t *const monster, const engine3D_vector3f_t *const orientation, float distance) {
	engine3D_vector3f_t tmp, newPos, movementVector, collisionVector;

	double time = engine3D_timer_getTime() / engine3D_timer_second;
	double timeDecimals = time - (double)((int)time);

	if (timeDecimals < 0.25) {
		monster->material->texture = animations + 0;
	}
	else if (timeDecimals < 0.5) {
		monster->material->texture = animations + 1;
	}
	else if (timeDecimals < 0.75) {
		monster->material->texture = animations + 2;
	}
	else {
		monster->material->texture = animations + 3;
	}

	if (((float)rand() / (float)RAND_MAX) < ATTACK_CHANCE * (float)engine3D_time_getDelta())
		monster->state = WFSTN3D_MONSTER_STATE_ATTACKING;

	if (distance > MOVE_STOP_DISTANCE) {
		float movAmnt = MOVE_SPEED * (float)engine3D_time_getDelta();

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
	else {
		monster->state = WFSTN3D_MONSTER_STATE_ATTACKING;
	}
}

void attackingUpdate(wfstn3D_monster_t *const monster, const engine3D_vector3f_t *const orientation, float distance) {
	(void)distance;

	double time = engine3D_timer_getTime() / engine3D_timer_second;
	double timeDecimals = time - (double)((int)time);

	if (timeDecimals < 0.25) {
		monster->material->texture = animations + 4;
	}
	else if (timeDecimals < 0.5) {
		monster->material->texture = animations + 5;
	}
	else if (timeDecimals < 0.75) {
		monster->material->texture = animations + 6;

		if (monster->canAttack) {
			engine3D_vector2f_t lineStart, castDirection, rotatedCastDirection, lineEnd, collisionVector, playerIntersectVector, tmp;

			lineStart.x = monster->transform.translation.x;
			lineStart.y = monster->transform.translation.z;
			castDirection.x = orientation->x;
			castDirection.y = orientation->z;
			engine3D_vector2f_rotateDeg(&castDirection, (((float)rand()) / ((float)RAND_MAX) - 0.5f)*SHOOT_ANGLE, &rotatedCastDirection);

			engine3D_vector2f_add(&lineStart, engine3D_vector2f_mulf(&rotatedCastDirection, SHOOT_DISTANCE, &tmp), &lineEnd);

			bool i1 = wfstn3D_level_checkIntersections(monster->level, &lineStart, &lineEnd, &collisionVector, false);
			engine3D_vector2f_t size = { WFSTN3D_PLAYER_SIZE ,WFSTN3D_PLAYER_SIZE }, pos = { engine3D_transform_camera->pos.x,engine3D_transform_camera->pos.z };
			bool i2 = wfstn3D_level_lineIntersectRect(monster->level, &lineStart, &lineEnd, &pos, &size, &playerIntersectVector);

			if (i2 && (!i1 || engine3D_vector2f_length(engine3D_vector2f_sub(&playerIntersectVector, &lineStart, &tmp)) < engine3D_vector2f_length(engine3D_vector2f_sub(&collisionVector, &lineStart, &tmp)))) {
				wfstn3D_player_damage(monster->level->player, rand() % (SHOOT_DAMAGE_MAX - SHOOT_DAMAGE_MIN) + SHOOT_DAMAGE_MIN);
			}

			monster->state = WFSTN3D_MONSTER_STATE_CHASING;
			monster->canAttack = false;
		}
	}
	else {
		monster->canAttack = true;
		monster->material->texture = animations + 5;
	}
}

void dyingUpdate(wfstn3D_monster_t *const monster, const engine3D_vector3f_t *const orientation, float distance) {
	(void)orientation;
	(void)distance;

	double time = engine3D_timer_getTime() / engine3D_timer_second;
	double timeDecimals = time - (double)((int)time);

	if (monster->deathTime == 0)
	{
		monster->deathTime = time;
	}
	double deathTime = monster->deathTime;

	const double time1 = 0.1f;
	const double time2 = 0.3f;
	const double time3 = 0.45f;
	const double time4 = 0.6f;

	if (time < deathTime + time1)
	{
		monster->material->texture = animations + 8;
		monster->transform.scale.y = 0.96429f;
	}
	else if (time < deathTime + time2)
	{
		monster->material->texture = animations + 9;
		monster->transform.scale.x = 1.7f;
		monster->transform.scale.y = 0.9f;
	}
	else if (time < deathTime + time3)
	{
		monster->material->texture = animations + 10;
	}
	else if (time < deathTime + time4)
	{
		monster->material->texture = animations + 11;
		monster->transform.scale.y = 0.5f;
	}
	else
	{
		monster->state = WFSTN3D_MONSTER_STATE_DEAD;
	}
}

void deadUpdate(wfstn3D_monster_t *const monster, const engine3D_vector3f_t *const orientation, float distance) {
	(void)orientation;
	(void)distance;
	monster->material->texture = animations + 12;
	monster->transform.scale.x = 1.75862f;
	monster->transform.scale.y = 0.28571f;
}

void alignWithGround(wfstn3D_monster_t *const monster) {
	monster->transform.translation.y = 0;
}

void faceCamera(wfstn3D_monster_t *const monster, const engine3D_vector3f_t *const direction) {
	float angle = atanf(direction->z / direction->x) / 3.14159265359f * 180.0f + 90.0f;
	if (direction->x < 0)
		angle += 180.0f;
	monster->transform.rotation.y = angle;
}

void wfstn3D_monster_update(wfstn3D_monster_t *const monster) {
	float distance;
	engine3D_vector3f_t directionToCamera, orientation;
	engine3D_vector3f_sub(&engine3D_transform_camera->pos, &monster->transform.translation, &directionToCamera);
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
	free(monster->material);
}

void wfstn3D_monster_cleanupAll(void) {
	free(animations);
	animations = NULL;
}

void wfstn3D_monster_damage(wfstn3D_monster_t *const monster, int amount) {
	monster->health -= amount;
	if (monster->health <= 0) {
		monster->state = WFSTN3D_MONSTER_STATE_DYING;
	}
	else if (monster->state = WFSTN3D_MONSTER_STATE_IDLE) {
		monster->state = WFSTN3D_MONSTER_STATE_CHASING;
	}
}

void wfstn3D_monster_getSize(const wfstn3D_monster_t * const monster, engine3D_vector2f_t *const size) {
	size->x = MONSTER_WIDTH;
	size->y = MONSTER_LENGTH;
}
