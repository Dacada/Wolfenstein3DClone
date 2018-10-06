#ifndef ENGINE3D_SHADER_H
#define ENGINE3D_SHADER_H

#include "engine3D_vector.h"
#include "engine3D_trie.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>

typedef struct engine3D_shader_t {
	GLuint program;
	engine3D_trie_t *uniforms;
} engine3D_shader_t;

engine3D_shader_t *engine3D_shader_init(engine3D_shader_t *const shader);

engine3D_shader_t *engine3D_shader_addVertexShader(const char *const text, engine3D_shader_t * const shader);

engine3D_shader_t *engine3D_shader_addGeometryShader(const char *const text, engine3D_shader_t * const shader);

engine3D_shader_t *engine3D_shader_addFragmentShader(const char *const text, engine3D_shader_t * const shader);

engine3D_shader_t *engine3D_shader_addVertexShaderFromFile(const char *const text, engine3D_shader_t * const shader);

engine3D_shader_t *engine3D_shader_addGeometryShaderFromFile(const char *const text, engine3D_shader_t * const shader);

engine3D_shader_t *engine3D_shader_addFragmentShaderFromFile(const char *const text, engine3D_shader_t * const shader);

void engine3D_shader_compile(const engine3D_shader_t * const shader);

void engine3D_shader_bind(const engine3D_shader_t * const shader);

void engine3D_shader_addUniform(const char *const uniform, const engine3D_shader_t *const shader);

void engine3D_shader_setUniformi(const char *const uniform, const int value, const engine3D_shader_t *const shader);

void engine3D_shader_setUniformf(const char *const uniform, const float value, const engine3D_shader_t *const shader);

void engine3D_shader_setUniformVec3f(const char *const uniform, const engine3D_vector3f_t *const value, const engine3D_shader_t *const shader);

void engine3D_shader_setUniformMat4f(const char *const uniform, const engine3D_matrix4f_t *const value, const engine3D_shader_t *const shader);

#endif
