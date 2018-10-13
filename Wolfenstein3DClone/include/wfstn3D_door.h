#ifndef WFSTN3D_DOOR_H
#define WFSTN3D_DOOR_H

#include "wfstn3D_level.h"

#include <Engine3D/engine3D_transform.h>

#define WFSTN3D_DOOR_LENGTH (1.0f)
#define WFSTN3D_DOOR_HEIGHT (1.0f)
#define WFSTN3D_DOOR_WIDTH (0.125f)
#define WFSTN3D_DOOR_START (0.0f)

typedef struct wfstn3D_door_t {
	engine3D_transform_t transform;
	engine3D_material_t *material;
	wfstn3D_level_t *level;
} wfstn3D_door_t;

void wfstn3D_door_init(wfstn3D_door_t *const door, const engine3D_transform_t *const transform, engine3D_material_t *const material, wfstn3D_level_t *const level);

void wfstn3D_door_input(wfstn3D_door_t *const door);

void wfstn3D_door_update(wfstn3D_door_t *const door);

void wfstn3D_door_render(wfstn3D_door_t *const door);

void wfstn3D_door_cleanup(wfstn3D_door_t *const door);

#endif /* WFSTN3D_DOOR_H */
