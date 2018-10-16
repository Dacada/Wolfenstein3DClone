#include <wfstn3D_level.h>
#include <wfstn3D_player.h>
#include <wfstn3D_bitmap.h>
#include <wfstn3D_door.h>

#include <Engine3D/engine3D_mesh.h>
#include <Engine3D/engine3D_texture.h>
#include <Engine3D/engine3D_vector.h>
#include <Engine3D/engine3D_resourceLoader.h>
#include <Engine3D/engine3D_util.h>
#include <Engine3D/engine3D_input.h>

#include <stdbool.h>

#include <string.h>

#define SPOT_WIDTH (1.0f)
#define SPOT_LENGTH (1.0f)
#define SPOT_HEIGHT (1.0f)
#define NUM_TEX_EXP (4)
#define NUM_TEXTURES (NUM_TEX_EXP * NUM_TEX_EXP)
#define OPEN_DISTANCE (1.0f)
#define OPEN_MOVEMENT_AMOUNT (0.9f)

static void getTexCoords(unsigned int p, float *XLower, float *XHigher, float *YLower, float *YHigher) {
	/*
	* 255 191 127  63
	* 239 175 111  47
	* 223 159  95  31
	* 207 143  79  15
	*/

	unsigned int texX = p / NUM_TEX_EXP;
	unsigned int texY = p % NUM_TEX_EXP;

	*XLower = 1.0f - (float)texX / (float)NUM_TEX_EXP;
	*XHigher = *XLower - 1.0f / (float)NUM_TEX_EXP;
	*YLower = (float)texY / (float)NUM_TEX_EXP;
	*YHigher = *YLower + 1.0f / (float)NUM_TEX_EXP;
}

static void addFace(unsigned int *indices_array, size_t *indices_index, size_t startIndex, bool direction) {
	if (direction) {
		indices_array[(*indices_index)++] = startIndex + 2;
		indices_array[(*indices_index)++] = startIndex + 1;
		indices_array[(*indices_index)++] = startIndex + 0;
		indices_array[(*indices_index)++] = startIndex + 3;
		indices_array[(*indices_index)++] = startIndex + 2;
		indices_array[(*indices_index)++] = startIndex + 0;
	}
	else {
		indices_array[(*indices_index)++] = startIndex + 0;
		indices_array[(*indices_index)++] = startIndex + 1;
		indices_array[(*indices_index)++] = startIndex + 2;
		indices_array[(*indices_index)++] = startIndex + 0;
		indices_array[(*indices_index)++] = startIndex + 2;
		indices_array[(*indices_index)++] = startIndex + 3;
	}
}

static void addVertex(engine3D_vertex_t *vertices_array, size_t vertices_index, float vecX, float vecY, float vecZ, float texX, float texY) {
	vertices_array[vertices_index].vec.x = vecX;
	vertices_array[vertices_index].vec.y = vecY;
	vertices_array[vertices_index].vec.z = vecZ;
	vertices_array[vertices_index].texCoord.x = texX;
	vertices_array[vertices_index].texCoord.y = texY;
	vertices_array[vertices_index].normal.x = 0;
	vertices_array[vertices_index].normal.y = 0;
	vertices_array[vertices_index].normal.z = 0;
}

static void addDoor(wfstn3D_level_t *level, size_t i, size_t j) {
	wfstn3D_bitmap_t *levelBitmap = level->bitmap;

	engine3D_transform_t transform;
	engine3D_transform_reset(&transform);

	bool xDoor = (wfstn3D_bitmap_getPixel(levelBitmap, i, j - 1) & 0xFFFFFF) == 0 && (wfstn3D_bitmap_getPixel(levelBitmap, i, j + 1) & 0xFFFFFF) == 0;
	bool yDoor = (wfstn3D_bitmap_getPixel(levelBitmap, i - 1, j) & 0xFFFFFF) == 0 && (wfstn3D_bitmap_getPixel(levelBitmap, i + 1, j) & 0xFFFFFF) == 0;

	if (xDoor == yDoor) { // not xor
		engine3D_util_bail("door detected in invalid location during level generation");
	}

	engine3D_vector3f_t openPosition;

	if (yDoor) {
		transform.translation.x = i;
		transform.translation.y = 0;
		transform.translation.z = j + SPOT_WIDTH / 2;

		memcpy(&openPosition, &transform.translation, sizeof(engine3D_vector3f_t));
		openPosition.x -= OPEN_MOVEMENT_AMOUNT;
	}

	if (xDoor) {
		transform.translation.x = i + SPOT_WIDTH / 2;
		transform.translation.y = 0;
		transform.translation.z = j;

		transform.rotation.x = 0;
		transform.rotation.y = 90;
		transform.rotation.z = 0;

		memcpy(&openPosition, &transform.translation, sizeof(engine3D_vector3f_t));
		openPosition.z -= OPEN_MOVEMENT_AMOUNT;
	}

	size_t doorIndex = level->doorsLen++;
	level->doors = engine3D_util_safeRealloc(level->doors, sizeof(wfstn3D_door_t) * level->doorsLen);
	wfstn3D_door_init(level->doors + doorIndex, &transform, level->material, level, &openPosition);
}

static void addSpecial(unsigned int blueValue, wfstn3D_level_t *level, size_t i, size_t j) {
	if (blueValue == 16)
		addDoor(level, i, j);
}

static void generateLevel(engine3D_vertex_t **vertices, size_t *vertices_len, unsigned int **indices, size_t *indices_len, wfstn3D_level_t *level) {
	wfstn3D_bitmap_t *levelBitmap = level->bitmap;
	size_t vertices_capacity = 1024;
	size_t indices_capacity = 1024;
	engine3D_vertex_t *vertices_array = engine3D_util_safeMalloc(sizeof(engine3D_vertex_t) * vertices_capacity);
	unsigned int *indices_array = engine3D_util_safeMalloc(sizeof(unsigned int) * indices_capacity);
	size_t vertices_index = 0;
	size_t indices_index = 0;

	for (size_t i = 0; i < levelBitmap->width; i++) {
		for (size_t j = 0; j < levelBitmap->height; j++) {
			uint32_t pixel = wfstn3D_bitmap_getPixel(levelBitmap, i, j);
			if ((pixel & 0xFFFFFF) != 0) {
				float XLower, XHigher, YLower, YHigher;
				getTexCoords(((pixel & 0x00FF00) >> 8) / NUM_TEXTURES, &XLower, &XHigher, &YLower, &YHigher);

				addSpecial(pixel & 0x0000FF, level, i, j);

				if (vertices_index + 24 >= vertices_capacity) {
					vertices_capacity *= 2;
					vertices_array = engine3D_util_safeRealloc(vertices_array, sizeof(engine3D_vertex_t) * vertices_capacity);
				}

				if (indices_index + 36 >= indices_capacity) {
					indices_capacity *= 2;
					indices_array = engine3D_util_safeRealloc(indices_array, sizeof(unsigned int) * indices_capacity);
				}

				// FLOOR //
				addFace(indices_array, &indices_index, vertices_index, true);
				addVertex(vertices_array, vertices_index++, i * SPOT_WIDTH, 0, j * SPOT_LENGTH, XLower, YLower);
				addVertex(vertices_array, vertices_index++, (i + 1) * SPOT_WIDTH, 0, j * SPOT_LENGTH, XHigher, YLower);
				addVertex(vertices_array, vertices_index++, (i + 1) * SPOT_WIDTH, 0, (j + 1) * SPOT_LENGTH, XHigher, YHigher);
				addVertex(vertices_array, vertices_index++, i * SPOT_WIDTH, 0, (j + 1) * SPOT_LENGTH, XLower, YHigher);

				// CEILING //
				addFace(indices_array, &indices_index, vertices_index, false);
				addVertex(vertices_array, vertices_index++, i * SPOT_WIDTH, SPOT_HEIGHT, j * SPOT_LENGTH, XLower, YLower);
				addVertex(vertices_array, vertices_index++, (i + 1) * SPOT_WIDTH, SPOT_HEIGHT, j * SPOT_LENGTH, XHigher, YLower);
				addVertex(vertices_array, vertices_index++, (i + 1) * SPOT_WIDTH, SPOT_HEIGHT, (j + 1) * SPOT_LENGTH, XHigher, YHigher);
				addVertex(vertices_array, vertices_index++, i * SPOT_WIDTH, SPOT_HEIGHT, (j + 1) * SPOT_LENGTH, XLower, YHigher);

				// WALLS //
				getTexCoords(((pixel & 0xFF0000) >> 16) / NUM_TEXTURES, &XLower, &XHigher, &YLower, &YHigher);
				if ((wfstn3D_bitmap_getPixel(levelBitmap, i, j - 1) & 0xFFFFFF) == 0) {
					addFace(indices_array, &indices_index, vertices_index, false);
					addVertex(vertices_array, vertices_index++, i * SPOT_WIDTH, 0, j * SPOT_LENGTH, XLower, YLower);
					addVertex(vertices_array, vertices_index++, (i + 1) * SPOT_WIDTH, 0, j * SPOT_LENGTH, XHigher, YLower);
					addVertex(vertices_array, vertices_index++, (i + 1) * SPOT_WIDTH, SPOT_HEIGHT, j * SPOT_LENGTH, XHigher, YHigher);
					addVertex(vertices_array, vertices_index++, i * SPOT_WIDTH, SPOT_HEIGHT, j * SPOT_LENGTH, XLower, YHigher);
				}
				if ((wfstn3D_bitmap_getPixel(levelBitmap, i, j + 1) & 0xFFFFFF) == 0) {
					addFace(indices_array, &indices_index, vertices_index, true);
					addVertex(vertices_array, vertices_index++, i * SPOT_WIDTH, 0, (j + 1) * SPOT_LENGTH, XLower, YLower);
					addVertex(vertices_array, vertices_index++, (i + 1) * SPOT_WIDTH, 0, (j + 1) * SPOT_LENGTH, XHigher, YLower);
					addVertex(vertices_array, vertices_index++, (i + 1) * SPOT_WIDTH, SPOT_HEIGHT, (j + 1) * SPOT_LENGTH, XHigher, YHigher);
					addVertex(vertices_array, vertices_index++, i * SPOT_WIDTH, SPOT_HEIGHT, (j + 1) * SPOT_LENGTH, XLower, YHigher);
				}
				if ((wfstn3D_bitmap_getPixel(levelBitmap, i - 1, j) & 0xFFFFFF) == 0) {
					addFace(indices_array, &indices_index, vertices_index, true);
					addVertex(vertices_array, vertices_index++, i * SPOT_WIDTH, 0, j * SPOT_LENGTH, XLower, YLower);
					addVertex(vertices_array, vertices_index++, i * SPOT_WIDTH, 0, (j + 1) * SPOT_LENGTH, XHigher, YLower);
					addVertex(vertices_array, vertices_index++, i * SPOT_WIDTH, SPOT_HEIGHT, (j + 1) * SPOT_LENGTH, XHigher, YHigher);
					addVertex(vertices_array, vertices_index++, i * SPOT_WIDTH, SPOT_HEIGHT, j * SPOT_LENGTH, XLower, YHigher);
				}
				if ((wfstn3D_bitmap_getPixel(levelBitmap, i + 1, j) & 0xFFFFFF) == 0) {
					addFace(indices_array, &indices_index, vertices_index, false);
					addVertex(vertices_array, vertices_index++, (i + 1) * SPOT_WIDTH, 0, j * SPOT_LENGTH, XLower, YLower);
					addVertex(vertices_array, vertices_index++, (i + 1) * SPOT_WIDTH, 0, (j + 1) * SPOT_LENGTH, XHigher, YLower);
					addVertex(vertices_array, vertices_index++, (i + 1) * SPOT_WIDTH, SPOT_HEIGHT, (j + 1) * SPOT_LENGTH, XHigher, YHigher);
					addVertex(vertices_array, vertices_index++, (i + 1) * SPOT_WIDTH, SPOT_HEIGHT, j * SPOT_LENGTH, XLower, YHigher);
				}
			}
		}
	}

	vertices_array = engine3D_util_safeRealloc(vertices_array, sizeof(engine3D_vertex_t) * vertices_index);
	indices_array = engine3D_util_safeRealloc(indices_array, sizeof(unsigned int) * indices_index);

	*vertices = vertices_array;
	*indices = indices_array;
	*vertices_len = vertices_index;
	*indices_len = indices_index;
}

void wfstn3D_level_load(const char *const levelname, const char *const texturename, wfstn3D_level_t *const level) {
	level->doorsLen = 0;
	level->doors = NULL;

	level->bitmap = engine3D_util_safeMalloc(sizeof(wfstn3D_bitmap_t));
	wfstn3D_bitmap_load(levelname, level->bitmap);

	level->shader = engine3D_util_safeMalloc(sizeof(engine3D_basicShader_t));
	engine3D_basicShader_init(level->shader);
	engine3D_texture_t *texture = engine3D_util_safeMalloc(sizeof(engine3D_texture_t));
	engine3D_vector3f_t *color = engine3D_util_safeMalloc(sizeof(engine3D_vector3f_t));
	level->material = engine3D_util_safeMalloc(sizeof(engine3D_material_t));
	engine3D_resourceLoader_loadTexture(texturename, texture);

	// TODO: Create function to bind texture to material and setup default values
	color->x = color->y = color->z = 1;
	level->material->texture = texture;
	level->material->color = color;
	level->material->specularIntensity = 0;
	level->material->specularPower = 0;

	level->mesh = engine3D_util_safeMalloc(sizeof(engine3D_mesh_t));
	engine3D_mesh_init(level->mesh);

	size_t vertices_len, indices_len;
	engine3D_vertex_t *vertices;
	unsigned int *indices;
	generateLevel(&vertices, &vertices_len, &indices, &indices_len, level);

	engine3D_mesh_addVertices(level->mesh, vertices, vertices_len, indices, indices_len, true);
	//engine3D_mesh_addVertices(&mesh, vertices, 4, indices, 6, true);

	free(vertices);
	free(indices);

	level->transform = engine3D_util_safeMalloc(sizeof(engine3D_transform_t));
	engine3D_transform_reset(level->transform);

	//level->door = engine3D_util_safeMalloc(sizeof(wfstn3D_door_t));
	//engine3D_transform_t *tmp = engine3D_util_safeMalloc(sizeof(engine3D_transform_t));
	//engine3D_transform_reset(tmp);
	//tmp->translation.x = 16; tmp->translation.y = 0; tmp->translation.z = 28;
	//wfstn3D_door_init(level->door, tmp, level->material, level);
}

void wfstn3D_level_input(const wfstn3D_level_t *const level) {
	if (engine3D_input_getKeyDown(GLFW_KEY_E)) {
		for (size_t i = 0; i < level->doorsLen; i++) {
			wfstn3D_door_t *door = level->doors + i;
			engine3D_vector3f_t tmp;
			if (engine3D_vector3f_length(engine3D_vector3f_sub(&door->transform.translation, &level->player->camera->pos, &tmp)) < OPEN_DISTANCE) {
				wfstn3D_door_open(door);
			}
		}
	}

	wfstn3D_player_input(level->player);
	for (size_t i = 0; i < level->doorsLen; i++) {
		wfstn3D_door_input(level->doors + i);
	}
}

void wfstn3D_level_update(const wfstn3D_level_t *const level) {
	wfstn3D_player_update(level->player);
	for (size_t i = 0; i < level->doorsLen; i++) {
		wfstn3D_door_update(level->doors + i);
	}
}

void wfstn3D_level_render(const wfstn3D_level_t *const level) {
	engine3D_shader_bind((engine3D_shader_t*)(level->shader));

	engine3D_matrix4f_t worldMatrix, projectedMatrix;
	engine3D_transform_getTransformation(level->transform, &worldMatrix);
	engine3D_transform_getProjectedTransformation(level->transform, &projectedMatrix);
	engine3D_basicShader_updateUniforms(level->shader, &worldMatrix, &projectedMatrix, level->material);
	engine3D_mesh_draw(level->mesh);

	wfstn3D_player_render(level->player);
	for (size_t i = 0; i < level->doorsLen; i++) {
		wfstn3D_door_render(level->doors + i);
	}
}

void wfstn3D_level_unload(wfstn3D_level_t *const level) {
	wfstn3D_player_cleanup(level->player);
	for (size_t i = 0; i < level->doorsLen; i++) {
		wfstn3D_door_cleanup(level->doors + i);
	}
	free(level->doors);

	wfstn3D_bitmap_unload(level->bitmap);
	free(level->bitmap);
	free(level->shader);
	free(level->material->texture);
	free(level->material->color);
	free(level->material);
	free(level->mesh);
	free(level->transform);
}

static void rectCollide(const engine3D_vector2f_t *const oldPos, const engine3D_vector2f_t *const newPos, const engine3D_vector2f_t *const size1, const engine3D_vector2f_t *const pos2, const engine3D_vector2f_t *const size2, engine3D_vector2f_t *const result) {
	result->x = 0;
	result->y = 0;

	if (newPos->x + size1->x < pos2->x ||
		newPos->x - size1->x > pos2->x + size2->x * size2->x ||
		oldPos->y + size1->y < pos2->y ||
		oldPos->y - size1->y > pos2->y + size2->y * size2->y)
		result->x = 1;

	if (oldPos->x + size1->x < pos2->x ||
		oldPos->x - size1->x > pos2->x + size2->x * size2->x ||
		newPos->y + size1->y < pos2->y ||
		newPos->y - size1->y > pos2->y + size2->y * size2->y)
		result->y = 1;
}

void wfstn3D_level_checkCollision(const engine3D_vector3f_t *const oldPos, const engine3D_vector3f_t *const newPos, float objectWidth, float objectLength, engine3D_vector3f_t *const result, const wfstn3D_level_t *const level) {
	engine3D_vector2f_t collisionVector = { 1, 1 };
	engine3D_vector3f_t movementVector;
	engine3D_vector3f_sub(newPos, oldPos, &movementVector);

	if (engine3D_vector3f_length(&movementVector) > 0) {
		engine3D_vector2f_t blockSize = { SPOT_WIDTH, SPOT_LENGTH };
		engine3D_vector2f_t objectSize = { objectWidth, objectLength };

		engine3D_vector2f_t oldPos2 = { oldPos->x, oldPos->z };
		engine3D_vector2f_t newPos2 = { newPos->x, newPos->z };

		for (size_t i = 0; i < level->bitmap->width; i++) {
			for (size_t j = 0; j < level->bitmap->height; j++) {
				uint32_t pixel = wfstn3D_bitmap_getPixel(level->bitmap, i, j);
				if ((pixel & 0xFFFFFF) == 0) {
					engine3D_vector2f_t tmp, tmp2, pixPos = { i, j };
					rectCollide(&oldPos2, &newPos2, &objectSize, engine3D_vector2f_mul(&blockSize, &pixPos, &tmp), &blockSize, &tmp2);
					engine3D_vector2f_mul(&collisionVector, &tmp2, &tmp);
					memcpy(&collisionVector, &tmp, sizeof(engine3D_vector2f_t));
				}
			}
		}

		// TODO: Orientation
		for (size_t i = 0; i < level->doorsLen; i++) {
			engine3D_vector2f_t tmp, tmp2, doorPos2, doorSize = { WFSTN3D_DOOR_LENGTH, WFSTN3D_DOOR_WIDTH };
			doorPos2.x = level->doors[i].transform.translation.x;
			doorPos2.y = level->doors[i].transform.translation.z;
			rectCollide(&oldPos2, &newPos2, &objectSize, &doorPos2, &doorSize, &tmp);
			engine3D_vector2f_mul(&collisionVector, &tmp, &tmp2);
			memcpy(&collisionVector, &tmp2, sizeof(engine3D_vector2f_t));
		}
	}

	result->x = collisionVector.x;
	result->y = 0;
	result->z = collisionVector.y;
}
