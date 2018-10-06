#ifndef ENGINE3D_CAMERA_H
#define ENGINE3D_CAMERA_H

#include "engine3D_vector.h"

typedef struct engine3D_camera_t {
	engine3D_vector3f_t pos;
	engine3D_vector3f_t forward;
	engine3D_vector3f_t up;
} engine3D_camera_t;

void engine3D_camera_init(engine3D_camera_t *const camera);

void engine3D_camera_normalize(engine3D_camera_t *const camera);

void engine3D_camera_move(engine3D_camera_t *const camera, const engine3D_vector3f_t *const dir, const float amnt);

void engine3D_camera_rotateX(engine3D_camera_t *const camera, const float angle);

void engine3D_camera_rotateY(engine3D_camera_t *const camera, const float angle);

void engine3D_camera_left(const engine3D_camera_t *const camera, engine3D_vector3f_t *const left);

void engine3D_camera_right(const engine3D_camera_t *const camera, engine3D_vector3f_t *const right);

#endif /* ENGINE3D_CAMERA_H */