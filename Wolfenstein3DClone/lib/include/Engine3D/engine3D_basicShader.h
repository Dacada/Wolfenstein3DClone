#ifndef ENGINE3D_BASICSHADER
#define ENGINE3D_BASICSHADER

#include "engine3D_shader.h"
#include "engine3D_material.h"

typedef struct engine3D_basicShader_t {
	engine3D_shader_t shader;
} engine3D_basicShader_t;

engine3D_basicShader_t *engine3D_basicShader_init(engine3D_basicShader_t *const shader);

void engine3D_basicShader_updateUniforms(engine3D_basicShader_t * const shader, const engine3D_matrix4f_t *const worldMatrix, const engine3D_matrix4f_t *projectedMatrix, const engine3D_material_t *const material);

#endif /* ENGINE3D_BASICSHADER */