#ifndef ENGINE3D_MESH_H
#define ENGINE3D_MESH_H

#include "engine3D_vertex.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include <stdbool.h>

typedef struct engine3D_mesh_t {
	GLuint vbo;
	GLuint ibo;
	GLsizei len_vbo; //number of elements in the underlying vertex array vbo refers to
	GLsizei len_ibo; //number of elements in the underlying index array ibo refers to
} engine3D_mesh_t;

// Takes pointer to mesh struct, initializes it, returns same pointer
engine3D_mesh_t *engine3D_mesh_init(engine3D_mesh_t *const mesh);

// Create vbo for the vertices
engine3D_mesh_t *engine3D_mesh_addVertices(engine3D_mesh_t *const mesh, engine3D_vertex_t vertices[], size_t vertices_len, unsigned int indices[], size_t indices_len, bool doCalcNormals);

void engine3D_mesh_draw(const engine3D_mesh_t *const mesh);

#endif /* ENGINE3D_MESH_H */
