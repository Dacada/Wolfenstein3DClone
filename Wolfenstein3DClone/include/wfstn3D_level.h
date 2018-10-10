#ifndef WFSTN3D_LEVEL_H
#define WFSTN3D_LEVEL_H

#include <Engine3D\engine3D_mesh.h>
#include <Engine3D\engine3D_basicShader.h>
#include <Engine3D\engine3D_material.h>
#include <Engine3D\engine3D_transform.h>
#include "wfstn3D_bitmap.h"

typedef struct wfstn3D_level_t {
	wfstn3D_bitmap_t *bitmap;
	engine3D_basicShader_t *shader;
	engine3D_mesh_t *mesh;
	engine3D_material_t *material;
	engine3D_transform_t *transform;
} wfstn3D_level_t;

void wfstn3D_level_load(const char *const levelname, const char *const texturename, wfstn3D_level_t *const level);

void wfstn3D_level_input(const wfstn3D_level_t *const level);

void wfstn3D_level_update(const wfstn3D_level_t *const level);

void wfstn3D_level_render(const wfstn3D_level_t *const level);

void wfstn3D_level_unload(wfstn3D_level_t *const level);

#endif