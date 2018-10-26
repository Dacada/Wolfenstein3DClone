#ifndef WFSTN3D_PLAYER_H
#define WFSTN3D_PLAYER_H

#include "wfstn3D_level.h"

#include <Engine3D/engine3D_vector.h>
#include <Engine3D/engine3D_camera.h>

#define WFSTN3D_PLAYER_SIZE (0.3f)
#define WFSTN3D_PLAYER_MAX_HEALTH (100)

typedef struct wfstn3D_player_t {
	engine3D_camera_t *camera;
	wfstn3D_level_t *level;
	engine3D_vector3f_t movementVector;
	int health;
} wfstn3D_player_t;

void wfstn3D_player_init(const engine3D_vector3f_t *const position, wfstn3D_level_t *const level, wfstn3D_player_t *const player);

void wfstn3D_player_input(wfstn3D_player_t *const player);

void wfstn3D_player_update(wfstn3D_player_t *const player);

void wfstn3D_player_render(wfstn3D_player_t *const player);

void wfstn3D_player_cleanup(wfstn3D_player_t *const player);

int wfstn3D_player_getDamage(void);

void wfstn3D_player_damage(wfstn3D_player_t *const player, int amount);

#endif /* WFSTN3D_PLAYER_H */
