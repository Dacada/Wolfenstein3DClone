#include <Engine3D/engine3D_main.h>

#include <stdlib.h>

static void init(void) {

}

static void input(void) {

}

static void update(void) {

}

static void render(void) {

}

int main(void) {
	engine3D_setGame_init(init);
	engine3D_setGame_input(input);
	engine3D_setGame_update(update);
	engine3D_setGame_render(render);

	engine3D_init();
	engine3D_start();

	return EXIT_SUCCESS;
}
