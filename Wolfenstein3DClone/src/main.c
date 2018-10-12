#include <wfstn3D_level.h>
#include <wfstn3D_player.h>

#include <Engine3D/engine3D_main.h>
#include <Engine3D/engine3D_window.h>
#include <Engine3D/engine3D_input.h>
#include <Engine3D/engine3D_time.h>
#include <Engine3D/engine3D_transform.h>
#include <Engine3D/engine3D_resourceLoader.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

wfstn3D_level_t level;
wfstn3D_player_t player;

static void init(void) {
	engine3D_resourceLoader_setResPath("../Wolfenstein3DClone/res/");

	wfstn3D_level_load("level1.png", "WolfCollection.png", &level);

	engine3D_vector3f_t playerPos = { 0, 0.4375f, 0 };
	wfstn3D_player_init(&playerPos, &player);

	engine3D_transform_camera = player.camera;
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
	wfstn3D_player_input(&player);
}

static void update(void) {
	wfstn3D_level_update(&level);
	wfstn3D_player_update(&player);
}

static void render(void) {
	wfstn3D_level_render(&level);
	wfstn3D_player_render(&player);
}

static void cleanup(void) {
	wfstn3D_level_unload(&level);
	wfstn3D_player_cleanup(&player);
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
