#include <wfstn3D_player.h>

#include <Engine3D/engine3D_util.h>
#include <Engine3D/engine3D_resourceLoader.h>
#include <Engine3D/engine3D_input.h>
#include <Engine3D/engine3D_time.h>
#include <Engine3D/engine3D_window.h>

#include <stdbool.h>

#include <string.h>
#include <math.h>

#define MOUSE_SENSITIVITY (1000.0f)
#define MOVE_SPEED (10.0f)
#define SHOOT_DISTANCE (1000.0f)
#define SHOOT_DAMAGE_MIN (20)
#define SHOOT_DAMAGE_MAX (60)
#define MAX_HEALTH (100)

#define SCALE (0.0625f)
#define START (0.0f)
#define SIZEY SCALE
#define SIZEX (SIZEY / (2.0f * 1.83797))

#define TEX_MIN_X (0.0f)
#define TEX_MAX_X (-1.0f)
#define TEX_MIN_Y (0.0f)
#define TEX_MAX_Y (-1.0f)

#define GUN_OFFSET (-0.0875f)

engine3D_mesh_t gunMesh;
bool gunMeshIsLoaded = false;
engine3D_material_t gunMaterial;
bool gunMaterialIsLoaded = false;
engine3D_transform_t gunTransform;

void wfstn3D_player_init(const engine3D_vector3f_t *const position, wfstn3D_level_t *const level, wfstn3D_player_t *const player) {
	player->camera = engine3D_util_safeMalloc(sizeof(engine3D_camera_t));
	player->level = level;
	engine3D_camera_init(player->camera);
	player->camera->pos.x = position->x;
	player->camera->pos.y = position->y;
	player->camera->pos.z = position->z;
	player->camera->forward.x = 0;
	player->camera->forward.y = 0;
	player->camera->forward.z = -1;
	player->camera->up.x = 0;
	player->camera->up.y = 1;
	player->camera->up.z = 0;
	player->health = MAX_HEALTH;

	if (!gunMeshIsLoaded) {
		engine3D_vertex_t vertices[4] = { { { -SIZEX,START,START },{ TEX_MAX_X,TEX_MAX_Y },{ 0,0,0 } },
										  { { -SIZEX,SIZEY,START },{ TEX_MAX_X,TEX_MIN_Y },{ 0,0,0 } },
										  { {  SIZEX,SIZEY,START },{ TEX_MIN_X,TEX_MIN_Y },{ 0,0,0 } },
										  { {  SIZEX,START,START },{ TEX_MIN_X,TEX_MAX_Y },{ 0,0,0 } } };

		unsigned int indices[6] = { 0, 1, 2,
			0, 2, 3 };

		engine3D_mesh_init(&gunMesh);
		engine3D_mesh_addVertices(&gunMesh, vertices, 4, indices, 6, true);

		gunMeshIsLoaded = true;
	}

	if (!gunMaterialIsLoaded) {
		gunMaterial.color = engine3D_util_safeMalloc(sizeof(engine3D_vector3f_t));
		gunMaterial.texture = engine3D_util_safeMalloc(sizeof(engine3D_texture_t));

		gunMaterial.color->x = 1;
		gunMaterial.color->y = 1;
		gunMaterial.color->z = 1;
		engine3D_resourceLoader_loadTexture("PISGB0.png", gunMaterial.texture);
		gunMaterial.specularIntensity = 0;
		gunMaterial.specularPower = 0;
	}

	engine3D_transform_reset(&gunTransform);
	gunTransform.translation.x = position->x;
	gunTransform.translation.y = position->y;
	gunTransform.translation.z = position->z;
}

void wfstn3D_player_input(wfstn3D_player_t *const player) {
	float delta = (float)engine3D_time_getDelta();

	if (engine3D_input_getKeyDown(GLFW_KEY_E)) {
		wfstn3D_level_openDoorsAt(&player->camera->pos, player->level);
	}

	static bool mouseLock = false;
	int centerPositionX, centerPositionY;
	engine3D_window_getSize(&centerPositionX, &centerPositionY);
	engine3D_vector2f_t centerPosition;
	centerPosition.x = (float)centerPositionX;
	centerPosition.y = (float)centerPositionY;

	if (engine3D_input_getKey(GLFW_KEY_ESCAPE)) {
		engine3D_input_setCursor(ENGINE3D_CURSOR_ENABLED);
		mouseLock = false;
	}

	if (engine3D_input_getMouseDown(GLFW_MOUSE_BUTTON_1)) {
		if (!mouseLock)
		{
			engine3D_input_setCursor(ENGINE3D_CURSOR_DISABLED);
			engine3D_input_setMousePosition(&centerPosition);
			mouseLock = true;
		}
		else {
			engine3D_vector2f_t lineStart, lineEnd, castDirection, tmp, collisionVector;
			lineStart.x = player->camera->pos.x;
			lineStart.y = player->camera->pos.z;
			castDirection.x = player->camera->forward.x;
			castDirection.y = player->camera->forward.z;
			engine3D_vector2f_normalize(&castDirection);
			engine3D_vector2f_add(&lineStart, engine3D_vector2f_mulf(&castDirection, SHOOT_DISTANCE, &tmp), &lineEnd);

			wfstn3D_level_checkIntersections(player->level, &lineStart, &lineEnd, &collisionVector, true);
		}
	}

	player->movementVector.x = 0;
	player->movementVector.y = 0;
	player->movementVector.z = 0;

	if (engine3D_input_getKey(GLFW_KEY_W)) {
		//engine3D_camera_move(player->camera, &player->camera->forward, movAmt);
		engine3D_vector3f_add(&player->movementVector, &player->camera->forward, &player->movementVector);
	}
	if (engine3D_input_getKey(GLFW_KEY_S)) {
		//engine3D_camera_move(player->camera, &player->camera->forward, -movAmt);
		engine3D_vector3f_sub(&player->movementVector, &player->camera->forward, &player->movementVector);
	}
	if (engine3D_input_getKey(GLFW_KEY_A)) {
		engine3D_vector3f_t vec;
		engine3D_camera_left(player->camera, &vec);
		engine3D_vector3f_add(&player->movementVector, &vec, &player->movementVector);
		//engine3D_camera_move(player->camera, &vec, movAmt);
	}
	if (engine3D_input_getKey(GLFW_KEY_D)) {
		engine3D_vector3f_t vec;
		engine3D_camera_right(player->camera, &vec);
		engine3D_vector3f_add(&player->movementVector, &vec, &player->movementVector);
		//engine3D_camera_move(player->camera, &vec, movAmt);
	}

	if (mouseLock) {
		engine3D_vector2f_t pos, deltaPos;
		engine3D_input_getMousePosition(&pos);
		engine3D_vector2f_sub(&pos, &centerPosition, &deltaPos);

		if (deltaPos.x != 0)
		{
			engine3D_camera_rotateY(player->camera, deltaPos.x * MOUSE_SENSITIVITY * delta);
		}

		if (deltaPos.y != 0)
		{
			engine3D_camera_rotateX(player->camera, deltaPos.y * MOUSE_SENSITIVITY * delta);
		}

		engine3D_input_setMousePosition(&centerPosition);
	}

	if (engine3D_input_getKey(GLFW_KEY_ENTER)) {
		engine3D_vector3f_fprintf(stderr, &player->camera->pos);
		engine3D_vector3f_fprintf(stderr, &player->camera->forward);
		engine3D_vector3f_fprintf(stderr, &player->camera->up);
	}
}

void wfstn3D_player_update(wfstn3D_player_t *const player) {
	float delta = (float)engine3D_time_getDelta();
	float movAmt = delta * MOVE_SPEED;

	player->movementVector.y = 0;
	if (engine3D_vector3f_length(&player->movementVector) > 0) {
		engine3D_vector3f_normalize(&player->movementVector);
	}

	engine3D_vector3f_t newPos, tmp, collisionVector;
	engine3D_vector3f_mulf(&player->movementVector, movAmt, &tmp);
	engine3D_vector3f_add(&player->camera->pos, &tmp, &newPos);
	wfstn3D_level_checkCollision(&player->camera->pos, &newPos, WFSTN3D_PLAYER_SIZE, WFSTN3D_PLAYER_SIZE, &collisionVector, player->level);
	engine3D_vector3f_mul(&player->movementVector, &collisionVector, &tmp);
	memcpy(&player->movementVector, &tmp, sizeof(engine3D_vector3f_t));

	if (engine3D_vector3f_length(&player->movementVector) > 0)
		engine3D_camera_move(player->camera, &player->movementVector, movAmt);

	// Gun movement
	engine3D_vector3f_t tmp2, normalizedCameraForward;
	memcpy(&normalizedCameraForward, &engine3D_transform_camera->forward, sizeof(engine3D_vector3f_t));
	engine3D_vector3f_normalize(&normalizedCameraForward);
	engine3D_vector3f_mulf(&normalizedCameraForward, 0.105f, &tmp);
	engine3D_vector3f_add(&engine3D_transform_camera->pos, &tmp, &gunTransform.translation);
	gunTransform.translation.y += GUN_OFFSET;

	float distance;
	engine3D_vector3f_t directionToCamera, orientation;
	engine3D_vector3f_sub(&engine3D_transform_camera->pos, &gunTransform.translation, &directionToCamera);
	distance = engine3D_vector3f_length(&directionToCamera);
	engine3D_vector3f_divf(&directionToCamera, distance, &orientation);

	float angle = atanf(orientation.z / orientation.x) / 3.14159265359f * 180.0f + 90.0f;
	if (orientation.x < 0)
		angle += 180.0f;
	gunTransform.rotation.y = angle;
}

void wfstn3D_player_render(wfstn3D_player_t *const player) {
	engine3D_basicShader_t *shader = player->level->shader;
	engine3D_matrix4f_t transformationMatrix, projectedTransformationMatrix;

	engine3D_transform_getTransformation(&gunTransform, &transformationMatrix);
	engine3D_transform_getProjectedTransformation(&gunTransform, &projectedTransformationMatrix);
	engine3D_basicShader_updateUniforms(shader, &transformationMatrix, &projectedTransformationMatrix, &gunMaterial);

	engine3D_mesh_draw(&gunMesh);
}

void wfstn3D_player_cleanup(wfstn3D_player_t *const player) {
	free(player->camera);
}

int wfstn3D_player_getDamage(void) {
	return rand() % (SHOOT_DAMAGE_MAX - SHOOT_DAMAGE_MIN) + SHOOT_DAMAGE_MIN;
}

extern bool isGameRunning;
void wfstn3D_player_damage(wfstn3D_player_t *const player, int amount) {
	player->health -= amount;
	if (player->health > MAX_HEALTH)
		player->health = MAX_HEALTH;

	fprintf(stderr, "HP %d/100\n", player->health);

	if (player->health <= 0) {
		isGameRunning = false;
		fprintf(stderr, "ur ded - Thanks Obama\n");
	}
}
