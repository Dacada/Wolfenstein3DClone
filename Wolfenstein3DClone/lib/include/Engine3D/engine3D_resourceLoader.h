#ifndef ENGINE3D_RESOURCELOADER_H
#define ENGINE3D_RESOURCELOADER_H

#include "engine3D_mesh.h"
#include "engine3D_texture.h"

#include <stdlib.h>

void engine3D_resourceLoader_setResPath(const char *path);

char *engine3D_resourceLoader_loadShader(const char *const filename, char *const text, size_t size);

void engine3D_resourceLoader_loadMesh(const char *const filename, engine3D_mesh_t *const mesh);

void engine3D_resourceLoader_loadTexture(const char *const filename, engine3D_texture_t *const texture);

#endif /* ENGINE3D_RESOURCELOADER_H */
