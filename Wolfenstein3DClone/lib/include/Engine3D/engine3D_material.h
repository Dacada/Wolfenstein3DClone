#ifndef ENGINE3D_MATERIAL
#define ENGINE3D_MATERIAL

#include "engine3D_texture.h"
#include "engine3D_vector.h"

typedef struct engine3D_material_t {
	engine3D_texture_t *texture;
	engine3D_vector3f_t *color;
	float specularIntensity;
	float specularPower;
} engine3D_material_t;

#endif /* ENGINE3D_MATERIAL */