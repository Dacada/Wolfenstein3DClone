#ifndef ENGINE3D_RENDERUTIL_H
#define ENGINE3D_RENDERUTIL_H

#include "engine3D_vector.h"

#include <stdbool.h>

void engine3D_renderUtil_clearScreen(void);

void engine3D_renderUtil_setTextures(bool enabled);

void engine3D_renderUtils_setClearColor(const engine3D_vector3f_t *const color);

void engine3D_renderUtil_initGraphics(void);

void engine3D_renderUtil_unbindTextures(void);

const char *engine3D_renderUtil_getOpenGLVersion(void);

#endif /* ENGINE3D_RENDERUTIL_H */