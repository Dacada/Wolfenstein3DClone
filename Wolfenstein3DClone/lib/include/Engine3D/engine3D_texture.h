#ifndef ENGINE3D_TEXTURE_H
#define ENGINE3D_TEXTURE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>

typedef struct engine3D_texture_t {
	GLuint id;
} engine3D_texture_t;

void engine3D_texture_bind(engine3D_texture_t *const texture);

#endif /* ENGINE3D_TEXTURE_H */