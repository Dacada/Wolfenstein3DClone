#include <wfstn3D_level.h>
#include <wfstn3D_player.h>
#include <wfstn3D_monster.h>

#include <Engine3D/engine3D_main.h>
#include <Engine3D/engine3D_window.h>
#include <Engine3D/engine3D_input.h>
#include <Engine3D/engine3D_time.h>
#include <Engine3D/engine3D_transform.h>
#include <Engine3D/engine3D_resourceLoader.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <time.h>

wfstn3D_level_t level;

bool isGameRunning;

static void init(void) {
	srand(time(NULL));
	isGameRunning = true;
	engine3D_resourceLoader_setResPath("../Wolfenstein3DClone/res/");

	wfstn3D_level_load("level1.png", "WolfCollection.png", &level);

	engine3D_transform_camera = level.player->camera;
	engine3D_transform_fov = 70;
	int width, height;
	engine3D_window_getSize(&width, &height);
	engine3D_transform_width = (float)width;
	engine3D_transform_height = (float)height;
	engine3D_transform_zNear = 0.01f;
	engine3D_transform_zFar = 1000;
}

static void input(void) {
	wfstn3D_level_input(&level);
}

static void update(void) {
	wfstn3D_level_update(&level);
}

static void render(void) {
	if (isGameRunning)
		wfstn3D_level_render(&level);
}

static void cleanup(void) {
	wfstn3D_level_unload(&level);
	wfstn3D_monster_cleanupAll();
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
