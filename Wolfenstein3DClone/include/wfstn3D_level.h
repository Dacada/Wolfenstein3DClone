#ifndef WFSTN3D_LEVEL_H
#define WFSTN3D_LEVEL_H

#include <Engine3D/engine3D_mesh.h>
#include <Engine3D/engine3D_basicShader.h>
#include <Engine3D/engine3D_material.h>
#include <Engine3D/engine3D_transform.h>
#include "wfstn3D_bitmap.h"

// door already includes level in order to be able to have a reference to its level
// but level also needs a reference to door in order to hold an array of doors it owns
struct wfstn3D_door_t;

// Player needs a reference to level and level a reference to player
// So we can't just include the player's header file
struct wfstn3D_player_t;

typedef struct wfstn3D_level_t {
	wfstn3D_bitmap_t *bitmap;

	engine3D_basicShader_t *shader;
	engine3D_mesh_t *mesh;
	engine3D_material_t *material;
	engine3D_transform_t *transform;

	struct wfstn3D_door_t *doors;
	size_t doorsLen;

	struct wfstn3D_player_t *player;

	engine3D_vector2f_t *collisionPosStart;
	size_t collisionPosStartLen;
	engine3D_vector2f_t *collisionPosEnd;
	size_t collisionPosEndLen;
} wfstn3D_level_t;

void wfstn3D_level_load(const char *const levelname, const char *const texturename, wfstn3D_level_t *const level);

void wfstn3D_level_input(const wfstn3D_level_t *const level);

void wfstn3D_level_update(const wfstn3D_level_t *const level);

void wfstn3D_level_render(const wfstn3D_level_t *const level);

void wfstn3D_level_unload(wfstn3D_level_t *const level);

void wfstn3D_level_checkCollision(const engine3D_vector3f_t *const oldPos, const engine3D_vector3f_t *const newPos, float objectWidth, float objectLength, engine3D_vector3f_t *const result, const wfstn3D_level_t *const level);

void wfstn3D_level_openDoorsAt(const engine3D_vector3f_t *const position, const wfstn3D_level_t *const level);

bool wfstn3D_level_checkIntersections(const wfstn3D_level_t *const level, const engine3D_vector2f_t *const lineStart, const engine3D_vector2f_t *const lineEnd, engine3D_vector2f_t *const collisionVector);

bool wfstn3D_level_lineIntersectRect(const wfstn3D_level_t *const level, const engine3D_vector2f_t *const lineStart, const engine3D_vector2f_t *const lineEnd, const engine3D_vector2f_t *const pos, const engine3D_vector2f_t *const size, engine3D_vector2f_t *const result);

#endif
