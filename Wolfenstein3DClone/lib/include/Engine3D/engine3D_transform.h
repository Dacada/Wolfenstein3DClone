#ifndef ENGINE3D_TRANSFORM_H
#define ENGINE3D_TRANSFORM_H

#include "engine3D_vector.h"
#include "engine3D_camera.h"

extern engine3D_camera_t *engine3D_transform_camera;
extern float engine3D_transform_zNear;
extern float engine3D_transform_zFar;
extern float engine3D_transform_width;
extern float engine3D_transform_height;
extern float engine3D_transform_fov;

typedef struct engine3D_transform_t {
	engine3D_vector3f_t translation;
	engine3D_vector3f_t rotation;
	engine3D_vector3f_t scale;
} engine3D_transform_t;

void engine3D_transform_reset(engine3D_transform_t *const transform);

void engine3D_transform_getTransformation(const engine3D_transform_t *const transform, engine3D_matrix4f_t *const transformationMatrix);

void engine3D_transform_getProjectedTransformation(const engine3D_transform_t *const transform, engine3D_matrix4f_t *const transformationMatrix);

#endif /* ENGINE3D_TRANSFORM_H */