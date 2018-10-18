#include <wfstn3D_player.h>

#include <Engine3D/engine3D_util.h>
#include <Engine3D/engine3D_input.h>
#include <Engine3D/engine3D_time.h>
#include <Engine3D/engine3D_window.h>

#include <stdbool.h>

#include <string.h>

#define MOUSE_SENSITIVITY (1000.0f)
#define MOVE_SPEED (10.0f)
#define PLAYER_SIZE (0.3f)

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
}

void wfstn3D_player_input(wfstn3D_player_t *const player) {
	float delta = (float)engine3D_time_getDelta();

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
		engine3D_input_setCursor(ENGINE3D_CURSOR_DISABLED);
		engine3D_input_setMousePosition(&centerPosition);
		mouseLock = true;
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
	wfstn3D_level_checkCollision(&player->camera->pos, &newPos, PLAYER_SIZE, PLAYER_SIZE, &collisionVector, player->level);
	engine3D_vector3f_mul(&player->movementVector, &collisionVector, &tmp);
	memcpy(&player->movementVector, &tmp, sizeof(engine3D_vector3f_t));

	engine3D_camera_move(player->camera, &player->movementVector, movAmt);
}

void wfstn3D_player_render(wfstn3D_player_t *const player) {
	(void)player;
}

void wfstn3D_player_cleanup(wfstn3D_player_t *const player) {
	free(player->camera);
}
