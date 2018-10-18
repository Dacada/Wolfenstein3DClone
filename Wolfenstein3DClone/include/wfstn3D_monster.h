#ifndef WFSTN3D_MONSTER_H
#define WFSTN3D_MONSTER_H

#include <Engine3D/engine3D_transform.h>
#include <Engine3D/engine3D_material.h>

#include "wfstn3D_level.h"

typedef enum wfstn3D_monster_state_t {
	WFSTN3D_MONSTER_STATE_IDLE,
	WFSTN3D_MONSTER_STATE_CHASING,
	WFSTN3D_MONSTER_STATE_ATTACKING,
	WFSTN3D_MONSTER_STATE_DYING,
	WFSTN3D_MONSTER_STATE_DEAD
} wfstn3D_monster_state_t;

typedef struct wfstn3D_monster_t {
	engine3D_transform_t transform;
	engine3D_material_t *material;
	wfstn3D_level_t *level;
	wfstn3D_monster_state_t state;
} wfstn3D_monster_t;

void wfstn3D_monster_init(wfstn3D_monster_t *const monster, const engine3D_transform_t *const transform, wfstn3D_level_t *const level);
			 
void wfstn3D_monster_input(wfstn3D_monster_t *const monster);
			 
void wfstn3D_monster_update(wfstn3D_monster_t *const monster);
			 
void wfstn3D_monster_render(wfstn3D_monster_t *const monster);
			 
void wfstn3D_monster_cleanup(wfstn3D_monster_t *const monster);

#endif /* WFSTN3D_MONSTER_H */