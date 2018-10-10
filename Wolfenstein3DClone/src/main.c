#include <wfstn3D_level.h>

#include <Engine3D/engine3D_input.h>
#include <Engine3D/engine3D_time.h>
#include <Engine3D/engine3D_transform.h>
#include <Engine3D/engine3D_resourceLoader.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

wfstn3D_level_t level;

static void init(void) {
	engine3D_resourceLoader_setResPath("../Wolfenstein3DClone/res/");

	engine3D_camera_init(&engine3D_transform_camera);

	engine3D_transform_fov = 70;
	int width, height;
	engine3D_window_getSize(&width, &height);
	engine3D_transform_width = (float)width;
	engine3D_transform_height = (float)height;
	engine3D_transform_zNear = 0.01f;
	engine3D_transform_zFar = 1000;

	wfstn3D_level_load("level1.png", "WolfCollection.png", &level);
}

// TODO: Set up cameraInput as part of the engine itself
static void cameraInput(float delta) {
	float sensitivity = 0.5f;
	float movAmt = delta * 10;

	static bool movingCamera = false;
	static engine3D_vector2f_t centerPosition;
	if (engine3D_input_getMouseDown(GLFW_MOUSE_BUTTON_1)) {
		engine3D_input_setCursor(ENGINE3D_CURSOR_DISABLED);
		engine3D_input_getMousePosition(&centerPosition);
		movingCamera = true;
	}

	if (engine3D_input_getMouseUp(GLFW_MOUSE_BUTTON_1)) {
		engine3D_input_setCursor(ENGINE3D_CURSOR_ENABLED);
		movingCamera = false;
	}

	if (engine3D_input_getKey(GLFW_KEY_W)) {
		engine3D_camera_move(&engine3D_transform_camera, &engine3D_transform_camera.forward, movAmt);
	}
	if (engine3D_input_getKey(GLFW_KEY_S)) {
		engine3D_camera_move(&engine3D_transform_camera, &engine3D_transform_camera.forward, -movAmt);
	}
	if (engine3D_input_getKey(GLFW_KEY_A)) {
		engine3D_vector3f_t vec;
		engine3D_camera_left(&engine3D_transform_camera, &vec);
		engine3D_camera_move(&engine3D_transform_camera, &vec, movAmt);
	}
	if (engine3D_input_getKey(GLFW_KEY_D)) {
		engine3D_vector3f_t vec;
		engine3D_camera_right(&engine3D_transform_camera, &vec);
		engine3D_camera_move(&engine3D_transform_camera, &vec, movAmt);
	}

	if (movingCamera) {
		engine3D_vector2f_t pos, deltaPos;
		engine3D_input_getMousePosition(&pos);
		engine3D_vector2f_sub(&pos, &centerPosition, &deltaPos);

		if (deltaPos.x != 0)
		{
			engine3D_camera_rotateY(&engine3D_transform_camera, deltaPos.x * sensitivity * delta);
		}

		if (deltaPos.y != 0)
		{
			engine3D_camera_rotateX(&engine3D_transform_camera, deltaPos.y * sensitivity * delta);
		}
	}
}

static void input(void) {
	cameraInput((float)engine3D_time_getDelta());
	wfstn3D_level_input(&level);
}

static void update(void) {
	wfstn3D_level_update(&level);
}

static void render(void) {
	wfstn3D_level_render(&level);
}

static void cleanup(void) {
	wfstn3D_level_unload(&level);
}

int main(void) {
	engine3D_setGame_init(init);
	engine3D_setGame_input(input);
	engine3D_setGame_update(update);
	engine3D_setGame_render(render);

	engine3D_init();
	engine3D_start();

	cleanup();

	return EXIT_SUCCESS;
}
