#ifndef ENGINE3D_VECTOR_H
#define ENGINE3D_VECTOR_H

#include <stdio.h>

typedef struct engine3D_vector2f_t {
	float x, y;
} engine3D_vector2f_t;

float engine3D_vector2f_length(const engine3D_vector2f_t *const v);

float engine3D_vector2f_dot(const engine3D_vector2f_t *const v1, const engine3D_vector2f_t *const v2);

// Normalize given vector and return given pointer to it
engine3D_vector2f_t *engine3D_vector2f_normalize(engine3D_vector2f_t *const v);

// Rotate given vector and return pointer to given result vector
// Version for angle in degrees or in radians
engine3D_vector2f_t *engine3D_vector2f_rotateRad(const engine3D_vector2f_t *const v, float angle, engine3D_vector2f_t *const result);
engine3D_vector2f_t *engine3D_vector2f_rotateDeg(const engine3D_vector2f_t *const v, float angle, engine3D_vector2f_t *const result);

// Add the two first vectors and save the result into the third.
// Return the pointer to this third vector.
engine3D_vector2f_t *engine3D_vector2f_add(const engine3D_vector2f_t *const v1, const engine3D_vector2f_t *const v2, engine3D_vector2f_t *const result);

// Add a float to a vector, then return a pointer to the given result vector
engine3D_vector2f_t *engine3D_vector2f_addf(const engine3D_vector2f_t *const v, float f, engine3D_vector2f_t *const result);

engine3D_vector2f_t *engine3D_vector2f_sub(const engine3D_vector2f_t *const v1, const engine3D_vector2f_t *const v2, engine3D_vector2f_t *const result);

engine3D_vector2f_t *engine3D_vector2f_subf(const engine3D_vector2f_t *const v, float f, engine3D_vector2f_t *const result);

engine3D_vector2f_t *engine3D_vector2f_mul(const engine3D_vector2f_t *const v1, const engine3D_vector2f_t *const v2, engine3D_vector2f_t *const result);

engine3D_vector2f_t *engine3D_vector2f_mulf(const engine3D_vector2f_t *const v, float f, engine3D_vector2f_t *const result);

engine3D_vector2f_t *engine3D_vector2f_div(const engine3D_vector2f_t *const v1, const engine3D_vector2f_t *const v2, engine3D_vector2f_t *const result);

engine3D_vector2f_t *engine3D_vector2f_divf(const engine3D_vector2f_t *const v, float f, engine3D_vector2f_t *const result);

int engine3D_vector2f_fprintf(FILE *stream, const engine3D_vector2f_t *const v);



typedef struct engine3D_vector3f_t {
	float x, y, z;
} engine3D_vector3f_t;

float engine3D_vector3f_length(const engine3D_vector3f_t *const v);

float engine3D_vector3f_dot(const engine3D_vector3f_t *const v1, const engine3D_vector3f_t *const v2);

engine3D_vector3f_t *engine3D_vector3f_cross(const engine3D_vector3f_t *const v1, const engine3D_vector3f_t *const v2, engine3D_vector3f_t *const result);

// Normalize given vector and return given pointer to it
engine3D_vector3f_t *engine3D_vector3f_normalize(engine3D_vector3f_t *const v);

// Rotate given vector around given axis and return pointer to given result vector
// Version for angle in degrees or in radians
engine3D_vector3f_t *engine3D_vector3f_rotateRad(const engine3D_vector3f_t * const v, float angle, const engine3D_vector3f_t *const axis, engine3D_vector3f_t *const result);
engine3D_vector3f_t *engine3D_vector3f_rotateDeg(const engine3D_vector3f_t * const v, float angle, const engine3D_vector3f_t *const axis, engine3D_vector3f_t *const result);

// Add the two first vectors and save the result into the third.
// Return the pointer to this third vector.
engine3D_vector3f_t *engine3D_vector3f_add(const engine3D_vector3f_t *const v1, const engine3D_vector3f_t *const v2, engine3D_vector3f_t *const result);

// Add a float to a vector, then return a pointer to the given result vector
engine3D_vector3f_t *engine3D_vector3f_addf(const engine3D_vector3f_t *const v, float f, engine3D_vector3f_t *const result);

engine3D_vector3f_t *engine3D_vector3f_sub(const engine3D_vector3f_t *const v1, const engine3D_vector3f_t *const v2, engine3D_vector3f_t *const result);

engine3D_vector3f_t *engine3D_vector3f_subf(const engine3D_vector3f_t *const v, float f, engine3D_vector3f_t *const result);

engine3D_vector3f_t *engine3D_vector3f_mul(const engine3D_vector3f_t *const v1, const engine3D_vector3f_t *const v2, engine3D_vector3f_t *const result);

engine3D_vector3f_t *engine3D_vector3f_mulf(const engine3D_vector3f_t *const v, float f, engine3D_vector3f_t *const result);

engine3D_vector3f_t *engine3D_vector3f_div(const engine3D_vector3f_t *const v1, const engine3D_vector3f_t *const v2, engine3D_vector3f_t *const result);

engine3D_vector3f_t *engine3D_vector3f_divf(const engine3D_vector3f_t *const v, float f, engine3D_vector3f_t *const result);

engine3D_vector3f_t *engine3D_vector3f_abs(const engine3D_vector3f_t *const v, engine3D_vector3f_t *const result);

int engine3D_vector3f_fprintf(FILE *stream, const engine3D_vector3f_t *const v);



typedef struct engine3D_matrix4f_t {
	float mat[4][4];
} engine3D_matrix4f_t;

void engine3D_matrix4f_setIdentity(engine3D_matrix4f_t *const matrix);

void engine3D_matrix4f_mul(const engine3D_matrix4f_t *const m1, const engine3D_matrix4f_t *const m2, engine3D_matrix4f_t *const r);

void engine3D_matrix4f_setTranslation(engine3D_matrix4f_t * const matrix, float x, float y, float z);

void engine3D_matrix4f_setRotation(engine3D_matrix4f_t * const matrix, float x, float y, float z);

void engine3D_matrix4f_setScale(engine3D_matrix4f_t * const matrix, float x, float y, float z);

void engine3D_matrix4f_setProjection(engine3D_matrix4f_t * const matrix, float zNear, float zFar, float width, float height, float fov);

void engine3D_matrix4f_setCamera(engine3D_matrix4f_t * const matrix, const engine3D_vector3f_t *const forward, const engine3D_vector3f_t *const up);



typedef struct engine3D_quaternion_t {
	float x;
	float y;
	float z;
	float w;
} engine3D_quaternion_t;

float engine3D_quaternion_length(const engine3D_quaternion_t *const v);

engine3D_quaternion_t * engine3D_quaternion_normalize(engine3D_quaternion_t * const v);

engine3D_quaternion_t * engine3D_quaternion_conjugate(const engine3D_quaternion_t * const v, engine3D_quaternion_t * const r);

engine3D_quaternion_t * engine3D_quaternion_mul(const engine3D_quaternion_t * const v1, const engine3D_quaternion_t * const v2, engine3D_quaternion_t * const r);

engine3D_quaternion_t * engine3D_quaternion_mulv(const engine3D_quaternion_t * const v1, const engine3D_vector3f_t * const v2, engine3D_quaternion_t * const r);

#endif /* ENGINE3D_VECTOR_H */
