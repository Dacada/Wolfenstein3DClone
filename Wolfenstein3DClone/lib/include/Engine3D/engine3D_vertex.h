#ifndef ENGINE3D_VERTEX_H
#define ENGINE3D_VERTEX_H

#include "engine3D_vector.h"

typedef struct engine3D_vertex_t {
	engine3D_vector3f_t vec;
	engine3D_vector2f_t texCoord;
	engine3D_vector3f_t normal;
} engine3D_vertex_t;

void engine3D_vertex_initZero(engine3D_vertex_t *vertex);

#endif /* ENGINE3D_VERTEX_H */