#ifndef WFSTN3D_MEDKIT_H
#define WFSTN3D_MEDKIT_H

#include "wfstn3D_level.h"

typedef struct wfstn3D_medkit_t {
	wfstn3D_level_t *level;
	engine3D_transform_t transform;
	bool enabled;
} wfstn3D_medkit_t;

void wfstn3D_medkit_init(wfstn3D_medkit_t *const medkit, const engine3D_vector3f_t *const position, wfstn3D_level_t *const level);
			 
void wfstn3D_medkit_input(wfstn3D_medkit_t *const medkit);
			 
void wfstn3D_medkit_update(wfstn3D_medkit_t *const medkit);
			 
void wfstn3D_medkit_render(wfstn3D_medkit_t *const medkit);
			 
void wfstn3D_medkit_cleanup(wfstn3D_medkit_t *const medkit);

#endif