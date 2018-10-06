#include <wfstn3D_bitmap.h>

#include <Engine3D/engine3D_main.h>

#include <stdio.h>
#include <stdlib.h>

wfstn3D_bitmap_t level;

static void init(void) {
	wfstn3D_bitmap_load("level1.png", &level);

	for (int i = 0; i < level.height; i++) {
		for (int j = 0; j < level.width; j++) {
			uint32_t pixel = wfstn3D_bitmap_getPixel(&level, j, i);
			if (pixel == 0xFF000000)
				putchar('#');
			else if (pixel == 0xFFFFFFFF)
				putchar(' ');
			else
				putchar('?');
		}
		putchar('\n');
	}
}

static void input(void) {

}

static void update(void) {

}

static void render(void) {

}

static void cleanup(void) {
	wfstn3D_bitmap_unload(&level);
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
