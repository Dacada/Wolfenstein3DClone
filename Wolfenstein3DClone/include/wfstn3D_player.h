#ifndef WFSTN3D_PLAYER_H
#define WFSTN3D_PLAYER_H

#include <Engine3D\engine3D_vector.h>
#include <Engine3D\engine3D_camera.h>

typedef struct wfstn3D_player_t {
	engine3D_camera_t *camera;
	engine3D_vector3f_t movementVector;
} wfstn3D_player_t;

void wfstn3D_player_init(const engine3D_vector3f_t *const position, wfstn3D_player_t *const player);

void wfstn3D_player_input(wfstn3D_player_t *const player);

void wfstn3D_player_update(wfstn3D_player_t *const player);

void wfstn3D_player_render(wfstn3D_player_t *const player);

void wfstn3D_player_cleanup(wfstn3D_player_t *const player);

#endif /* WFSTN3D_PLAYER_H */
