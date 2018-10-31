#include <wfstn3D_level.h>
#include <wfstn3D_player.h>
#include <wfstn3D_bitmap.h>
#include <wfstn3D_door.h>
#include <wfstn3D_monster.h>
#include <wfstn3D_medkit.h>

#include <Engine3D/engine3D_mesh.h>
#include <Engine3D/engine3D_texture.h>
#include <Engine3D/engine3D_vector.h>
#include <Engine3D/engine3D_resourceLoader.h>
#include <Engine3D/engine3D_util.h>
#include <Engine3D/engine3D_input.h>

#include <stdbool.h>

#include <string.h>
#include <math.h>

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

static void addPlayer(wfstn3D_level_t *level, size_t i, size_t j) {
	level->player = engine3D_util_safeMalloc(sizeof(wfstn3D_player_t));
	engine3D_vector3f_t playerPos = { (i + 0.5) * SPOT_WIDTH, 0.4375f, (j + 0.5) * SPOT_LENGTH };
	wfstn3D_player_init(&playerPos, level, level->player);
}

static void addMonster(wfstn3D_level_t *level, size_t i, size_t j) {
	size_t newMonsterIndex = level->monstersLen++;
	level->monsters = engine3D_util_safeRealloc(level->monsters, sizeof(wfstn3D_monster_t) * level->monstersLen);

	engine3D_transform_t tmp;
	engine3D_transform_reset(&tmp);
	tmp.translation.x = (i + 0.5) * SPOT_WIDTH;
	tmp.translation.y = 0.4375f;
	tmp.translation.z = (j + 0.5) * SPOT_LENGTH;
	wfstn3D_monster_init(level->monsters + newMonsterIndex, &tmp, level);
}

static void addMedkit(wfstn3D_level_t *level, size_t i, size_t j) {
	size_t newMedkitIndex = level->medkitsLen++;
	level->medkits = engine3D_util_safeRealloc(level->medkits, sizeof(wfstn3D_medkit_t) * level->medkitsLen);

	engine3D_vector3f_t tmp;
	tmp.x = (i + 0.5) * SPOT_WIDTH;
	tmp.y = 0;
	tmp.z = (j + 0.5) * SPOT_LENGTH;
	wfstn3D_medkit_init(level->medkits + newMedkitIndex, &tmp, level);
}

static void addExitPoint(wfstn3D_level_t *level, size_t i, size_t j) {
	size_t newExitPointIndex = level->exitPointsLen++;
	level->exitPoints = engine3D_util_safeRealloc(level->exitPoints, sizeof(wfstn3D_medkit_t) * level->exitPointsLen);
	level->exitPoints[newExitPointIndex].x = (i + 0.5) * SPOT_WIDTH;
	level->exitPoints[newExitPointIndex].y = 0;
	level->exitPoints[newExitPointIndex].z = (j + 0.5) * SPOT_LENGTH;
}

static void addSpecial(unsigned int blueValue, wfstn3D_level_t *level, size_t i, size_t j) {
	if (blueValue == 1)
		addPlayer(level, i, j);
	else if (blueValue == 16)
		addDoor(level, i, j);
	else if (blueValue == 97)
		addExitPoint(level, i, j);
	else if (blueValue == 128)
		addMonster(level, i, j);
	else if (blueValue == 192)
		addMedkit(level, i, j);
}

static void generateLevel(engine3D_vertex_t **vertices, size_t *vertices_len,
	                      unsigned int **indices, size_t *indices_len,
	                      engine3D_vector2f_t **collisionPosStart, size_t *collisionPosStartLen,
	                      engine3D_vector2f_t **collisionPosEnd, size_t *collisionPosEndLen,
	                      wfstn3D_level_t *level) {
	wfstn3D_bitmap_t *levelBitmap = level->bitmap;

	size_t vertices_capacity = 1024;
	size_t indices_capacity = 1024;
	size_t collisionPosStart_capacity = 1024;
	size_t collisionPosEnd_capacity = 1024;

	engine3D_vertex_t *vertices_array = engine3D_util_safeMalloc(sizeof(engine3D_vertex_t) * vertices_capacity);
	unsigned int *indices_array = engine3D_util_safeMalloc(sizeof(unsigned int) * indices_capacity);
	engine3D_vector2f_t * collisionPosStart_array = engine3D_util_safeMalloc(sizeof(engine3D_vector2f_t) * collisionPosStart_capacity);
	engine3D_vector2f_t * collisionPosEnd_array = engine3D_util_safeMalloc(sizeof(engine3D_vector2f_t) * collisionPosEnd_capacity);

	size_t vertices_index = 0;
	size_t indices_index = 0;
	size_t collisionPosStart_index = 0;
	size_t collisionPosEnd_index = 0;

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

				if (collisionPosStart_index + 4 >= collisionPosStart_capacity) {
					collisionPosStart_capacity *= 2;
					collisionPosStart_array = engine3D_util_safeRealloc(collisionPosStart_array, sizeof(engine3D_vector2f_t) * collisionPosStart_capacity);
				}

				if (collisionPosEnd_index + 4 >= collisionPosEnd_capacity) {
					collisionPosEnd_capacity *= 2;
					collisionPosEnd_array = engine3D_util_safeRealloc(collisionPosEnd_array, sizeof(engine3D_vector2f_t) * collisionPosEnd_capacity);
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
					collisionPosStart_array[collisionPosStart_index].x = i * SPOT_WIDTH; collisionPosStart_array[collisionPosStart_index++].y = j * SPOT_LENGTH;
					collisionPosEnd_array[collisionPosEnd_index].x = (i + 1) * SPOT_WIDTH; collisionPosEnd_array[collisionPosEnd_index++].y = j * SPOT_LENGTH;
					addFace(indices_array, &indices_index, vertices_index, false);
					addVertex(vertices_array, vertices_index++, i * SPOT_WIDTH, 0, j * SPOT_LENGTH, XLower, YLower);
					addVertex(vertices_array, vertices_index++, (i + 1) * SPOT_WIDTH, 0, j * SPOT_LENGTH, XHigher, YLower);
					addVertex(vertices_array, vertices_index++, (i + 1) * SPOT_WIDTH, SPOT_HEIGHT, j * SPOT_LENGTH, XHigher, YHigher);
					addVertex(vertices_array, vertices_index++, i * SPOT_WIDTH, SPOT_HEIGHT, j * SPOT_LENGTH, XLower, YHigher);
				}
				if ((wfstn3D_bitmap_getPixel(levelBitmap, i, j + 1) & 0xFFFFFF) == 0) {
					collisionPosStart_array[collisionPosStart_index].x = i * SPOT_WIDTH; collisionPosStart_array[collisionPosStart_index++].y = (j + 1) * SPOT_LENGTH;
					collisionPosEnd_array[collisionPosEnd_index].x = (i + 1) * SPOT_WIDTH; collisionPosEnd_array[collisionPosEnd_index++].y = (j + 1) * SPOT_LENGTH;
					addFace(indices_array, &indices_index, vertices_index, true);
					addVertex(vertices_array, vertices_index++, i * SPOT_WIDTH, 0, (j + 1) * SPOT_LENGTH, XLower, YLower);
					addVertex(vertices_array, vertices_index++, (i + 1) * SPOT_WIDTH, 0, (j + 1) * SPOT_LENGTH, XHigher, YLower);
					addVertex(vertices_array, vertices_index++, (i + 1) * SPOT_WIDTH, SPOT_HEIGHT, (j + 1) * SPOT_LENGTH, XHigher, YHigher);
					addVertex(vertices_array, vertices_index++, i * SPOT_WIDTH, SPOT_HEIGHT, (j + 1) * SPOT_LENGTH, XLower, YHigher);
				}
				if ((wfstn3D_bitmap_getPixel(levelBitmap, i - 1, j) & 0xFFFFFF) == 0) {
					collisionPosStart_array[collisionPosStart_index].x = i * SPOT_WIDTH; collisionPosStart_array[collisionPosStart_index++].y = j * SPOT_LENGTH;
					collisionPosEnd_array[collisionPosEnd_index].x = i * SPOT_WIDTH; collisionPosEnd_array[collisionPosEnd_index++].y = (j + 1) * SPOT_LENGTH;
					addFace(indices_array, &indices_index, vertices_index, true);
					addVertex(vertices_array, vertices_index++, i * SPOT_WIDTH, 0, j * SPOT_LENGTH, XLower, YLower);
					addVertex(vertices_array, vertices_index++, i * SPOT_WIDTH, 0, (j + 1) * SPOT_LENGTH, XHigher, YLower);
					addVertex(vertices_array, vertices_index++, i * SPOT_WIDTH, SPOT_HEIGHT, (j + 1) * SPOT_LENGTH, XHigher, YHigher);
					addVertex(vertices_array, vertices_index++, i * SPOT_WIDTH, SPOT_HEIGHT, j * SPOT_LENGTH, XLower, YHigher);
				}
				if ((wfstn3D_bitmap_getPixel(levelBitmap, i + 1, j) & 0xFFFFFF) == 0) {
					collisionPosStart_array[collisionPosStart_index].x = (i + 1) * SPOT_WIDTH; collisionPosStart_array[collisionPosStart_index++].y = j * SPOT_LENGTH;
					collisionPosEnd_array[collisionPosEnd_index].x = (i + 1) * SPOT_WIDTH; collisionPosEnd_array[collisionPosEnd_index++].y = (j + 1) * SPOT_LENGTH;
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
	collisionPosStart_array = engine3D_util_safeRealloc(collisionPosStart_array, sizeof(engine3D_vector2f_t) * collisionPosStart_index);
	collisionPosEnd_array = engine3D_util_safeRealloc(collisionPosEnd_array, sizeof(engine3D_vector2f_t) * collisionPosEnd_index);

	*vertices = vertices_array;
	*indices = indices_array;
	*collisionPosStart = collisionPosStart_array;
	*collisionPosEnd = collisionPosEnd_array;

	*vertices_len = vertices_index;
	*indices_len = indices_index;
	*collisionPosStartLen = collisionPosStart_index;
	*collisionPosEndLen = collisionPosEnd_index;
}

void wfstn3D_level_load(const char *const levelname, const char *const texturename, wfstn3D_level_t *const level) {
	level->player = NULL;

	level->doors = NULL;
	level->doorsLen = 0;

	level->monsters = NULL;
	level->monstersLen = 0;

	level->medkits = NULL;
	level->medkitsLen = 0;

	level->exitPoints = NULL;
	level->exitPointsLen = 0;

	level->bitmap = engine3D_util_safeMalloc(sizeof(wfstn3D_bitmap_t));
	wfstn3D_bitmap_load(levelname, level->bitmap);

	level->shader = engine3D_util_safeMalloc(sizeof(engine3D_basicShader_t));
	engine3D_basicShader_init(level->shader);
	engine3D_texture_t *texture = engine3D_util_safeMalloc(sizeof(engine3D_texture_t));
	engine3D_vector3f_t *color = engine3D_util_safeMalloc(sizeof(engine3D_vector3f_t));
	level->material = engine3D_util_safeMalloc(sizeof(engine3D_material_t));

	// TODO: Fix this madness
	engine3D_texture_t t = engine3D_texture_loadFromFile(texturename);
	texture->id = t.id;

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
	generateLevel(&vertices, &vertices_len, &indices, &indices_len, &level->collisionPosStart, &level->collisionPosStartLen, &level->collisionPosEnd, &level->collisionPosEndLen, level);

	engine3D_mesh_addVertices(level->mesh, vertices, vertices_len, indices, indices_len, true);
	free(vertices);
	free(indices);

	level->transform = engine3D_util_safeMalloc(sizeof(engine3D_transform_t));
	engine3D_transform_reset(level->transform);
}

void wfstn3D_level_input(const wfstn3D_level_t *const level) {
	wfstn3D_player_input(level->player);
	for (size_t i = 0; i < level->doorsLen; i++) {
		wfstn3D_door_input(level->doors + i);
	}
	for (size_t i = 0; i < level->monstersLen; i++) {
		wfstn3D_monster_input(level->monsters + i);
	}
	for (size_t i = 0; i < level->medkitsLen; i++) {
		wfstn3D_medkit_input(level->medkits + i);
	}
}

void wfstn3D_level_update(const wfstn3D_level_t *const level) {
	wfstn3D_player_update(level->player);
	for (size_t i = 0; i < level->doorsLen; i++) {
		wfstn3D_door_update(level->doors + i);
	}
	for (size_t i = 0; i < level->monstersLen; i++) {
		wfstn3D_monster_update(level->monsters + i);
	}
	for (size_t i = 0; i < level->medkitsLen; i++) {
		wfstn3D_medkit_update(level->medkits + i);
	}
}

void wfstn3D_level_render(const wfstn3D_level_t *const level) {
	engine3D_shader_bind((engine3D_shader_t*)(level->shader));

	engine3D_matrix4f_t worldMatrix, projectedMatrix;
	engine3D_transform_getTransformation(level->transform, &worldMatrix);
	engine3D_transform_getProjectedTransformation(level->transform, &projectedMatrix);
	engine3D_basicShader_updateUniforms(level->shader, &worldMatrix, &projectedMatrix, level->material);
	engine3D_mesh_draw(level->mesh);

	for (size_t i = 0; i < level->doorsLen; i++) {
		wfstn3D_door_render(level->doors + i);
	}
	for (size_t i = 0; i < level->monstersLen; i++) {
		wfstn3D_monster_render(level->monsters + i);
	}
	for (size_t i = 0; i < level->medkitsLen; i++) {
		wfstn3D_medkit_render(level->medkits + i);
	}
	wfstn3D_player_render(level->player);
}

void wfstn3D_level_unload(wfstn3D_level_t *const level) {
	wfstn3D_player_cleanup(level->player);
	for (size_t i = 0; i < level->doorsLen; i++) {
		wfstn3D_door_cleanup(level->doors + i);
	}
	free(level->doors);
	for (size_t i = 0; i < level->monstersLen; i++) {
		wfstn3D_monster_cleanup(level->monsters + i);
	}
	free(level->monsters);
	for (size_t i = 0; i < level->medkitsLen; i++) {
		wfstn3D_medkit_cleanup(level->medkits + i);
	}
	free(level->medkits);

	wfstn3D_bitmap_unload(level->bitmap);
	free(level->bitmap);
	free(level->shader);
	free(level->material->texture);
	free(level->material->color);
	free(level->material);
	free(level->mesh);
	free(level->transform);
	free(level->player);
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

		for (size_t i = 0; i < level->doorsLen; i++) {
			engine3D_vector2f_t tmp, tmp2, doorPos2, doorSize;
			wfstn3D_door_getSize(level->doors + i, &doorSize);
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

extern bool hasLevelEnded; // It might be more proper to pass this as a pointer on level creation
void wfstn3D_level_openDoorsAt(const engine3D_vector3f_t *const position, const wfstn3D_level_t *const level, bool tryExitLevel) {
	for (size_t i = 0; i < level->doorsLen; i++) {
		wfstn3D_door_t *door = level->doors + i;
		engine3D_vector3f_t tmp;
		if (engine3D_vector3f_length(engine3D_vector3f_sub(&door->transform.translation, position, &tmp)) < OPEN_DISTANCE) {
			wfstn3D_door_open(door);
		}
	}

	if (tryExitLevel) {
		for (size_t i = 0; i < level->doorsLen; i++) {
			engine3D_vector3f_t *exitPoint = level->exitPoints + i;
			engine3D_vector3f_t tmp;
			if (engine3D_vector3f_length(engine3D_vector3f_sub(exitPoint, position, &tmp)) < OPEN_DISTANCE) {
				hasLevelEnded = true;
			}
		}
	}
}

static inline float vector2f_crossProduct(const engine3D_vector2f_t a, const engine3D_vector2f_t b) {
	return a.x * b.y - a.y * b.x;
}

static bool lineIntersect(const engine3D_vector2f_t *const lineStart1, const engine3D_vector2f_t *const lineEnd1, const engine3D_vector2f_t *const lineStart2, const engine3D_vector2f_t *const lineEnd2, engine3D_vector2f_t *const result) {
	engine3D_vector2f_t line1, line2, distanceBetweenLineStarts, tmp;

	engine3D_vector2f_sub(lineEnd1, lineStart1, &line1);
	engine3D_vector2f_sub(lineEnd2, lineStart2, &line2);

	float cross = vector2f_crossProduct(line1, line2);
	if (cross == 0)
		return false;

	engine3D_vector2f_sub(lineStart2, lineStart1, &distanceBetweenLineStarts);

	float a = vector2f_crossProduct(distanceBetweenLineStarts, line2) / cross;
	float b = vector2f_crossProduct(distanceBetweenLineStarts, line1) / cross;

	if (0.0f < a && a < 1.0f && 0.0f < b && b < 1.0f) {
		engine3D_vector2f_add(lineStart1, engine3D_vector2f_mulf(&line1, a, &tmp), result);
		return true;
	}

	return false;
}

engine3D_vector2f_t *nearestVector2f(engine3D_vector2f_t *const a, engine3D_vector2f_t *const b, const engine3D_vector2f_t *const position) {
	engine3D_vector2f_t tmp;
	if (engine3D_vector2f_length(engine3D_vector2f_sub(a, position, &tmp)) > engine3D_vector2f_length(engine3D_vector2f_sub(b, position, &tmp))) {
		return b;
	}
	else {
		return a;
	}
}

bool wfstn3D_level_lineIntersectRect(const wfstn3D_level_t *const level, const engine3D_vector2f_t *const lineStart, const engine3D_vector2f_t *const lineEnd, const engine3D_vector2f_t *const pos, const engine3D_vector2f_t *const size, engine3D_vector2f_t *const result) {
	(void)level;
	engine3D_vector2f_t collisionVector, posAlt, tmp;
	bool i, res = false;

	posAlt.x = pos->x + size->x;
	posAlt.y = pos->y;
	if (lineIntersect(lineStart, lineEnd, pos, &posAlt, result)) {
		res = true;
	}

	posAlt.x = pos->x;
	posAlt.y = pos->y + size->y;
	i = lineIntersect(lineStart, lineEnd, pos, &posAlt, &collisionVector);
	if (res && i) {
		engine3D_vector2f_t *nearest = nearestVector2f(result, &collisionVector, lineStart);
		if (nearest != result)
			memcpy(result, &collisionVector, sizeof(engine3D_vector2f_t));
	}
	else if (i) {
		memcpy(result, &collisionVector, sizeof(engine3D_vector2f_t));
		res = true;
	}

	posAlt.x = pos->x;
	posAlt.y = pos->y + size->y;
	i = lineIntersect(lineStart, lineEnd, &posAlt, engine3D_vector2f_add(pos, size, &tmp), &collisionVector);
	if (res && i) {
		engine3D_vector2f_t *nearest = nearestVector2f(result, &collisionVector, lineStart);
		if (nearest != result)
			memcpy(result, &collisionVector, sizeof(engine3D_vector2f_t));
	}
	else if (i) {
		memcpy(result, &collisionVector, sizeof(engine3D_vector2f_t));
		res = true;
	}

	posAlt.x = pos->x + size->x;
	posAlt.y = pos->y;
	i = lineIntersect(lineStart, lineEnd, &posAlt, engine3D_vector2f_add(pos, size, &tmp), &collisionVector);
	if (res && i) {
		engine3D_vector2f_t *nearest = nearestVector2f(result, &collisionVector, lineStart);
		if (nearest != result)
			memcpy(result, &collisionVector, sizeof(engine3D_vector2f_t));
	}
	else if (i) {
		memcpy(result, &collisionVector, sizeof(engine3D_vector2f_t));
		res = true;
	}

	return res;
}

bool wfstn3D_level_checkIntersections(const wfstn3D_level_t *const level, const engine3D_vector2f_t *const lineStart, const engine3D_vector2f_t *const lineEnd, engine3D_vector2f_t *const nearestIntersection, bool hurtMonsters) {
	float minDiffference = INFINITY;
	bool ret = false;
	for (size_t i = 0; i < level->collisionPosStartLen; i++) {
		engine3D_vector2f_t collisionVector, tmp;
		float diff;
		if (lineIntersect(lineStart, lineEnd, level->collisionPosStart + i, level->collisionPosEnd + i, &collisionVector) &&
			minDiffference > (diff = engine3D_vector2f_length(engine3D_vector2f_sub(&collisionVector, lineStart, &tmp))))
		{
			minDiffference = diff;
			memcpy(nearestIntersection, &collisionVector, sizeof(engine3D_vector2f_t));
			ret = true;
		}
	}
	for (size_t i = 0; i < level->doorsLen; i++) {
		engine3D_vector2f_t collisionVector, tmp, doorPos2, doorSize;
		wfstn3D_door_getSize(level->doors + i, &doorSize);
		doorPos2.x = level->doors[i].transform.translation.x;
		doorPos2.y = level->doors[i].transform.translation.z;

		float diff;
		if (wfstn3D_level_lineIntersectRect(level, lineStart, lineEnd, &doorPos2, &doorSize, &collisionVector) &&
			minDiffference > (diff = engine3D_vector2f_length(engine3D_vector2f_sub(&collisionVector, lineStart, &tmp))))
		{
			minDiffference = diff;
			memcpy(nearestIntersection, &collisionVector, sizeof(engine3D_vector2f_t));
			ret = true;
		}
	}

	if (hurtMonsters) {
		float nearestMonsterDiff = INFINITY;
		engine3D_vector2f_t nearestMonsterIntersect, tmp;
		wfstn3D_monster_t *nearestMonster = NULL;
		bool seenAnyMonsters = false;

		for (size_t i = 0; i < level->monstersLen; i++) {
			engine3D_vector2f_t collisionVector, tmp, monsterPos2, monsterSize;
			wfstn3D_monster_getSize(level->monsters + i, &monsterSize);
			monsterPos2.x = level->monsters[i].transform.translation.x;
			monsterPos2.y = level->monsters[i].transform.translation.z;

			float diff;
			if (wfstn3D_level_lineIntersectRect(level, lineStart, lineEnd, &monsterPos2, &monsterSize, &collisionVector) &&
				nearestMonsterDiff >(diff = engine3D_vector2f_length(engine3D_vector2f_sub(&collisionVector, lineStart, &tmp))))
			{
				nearestMonsterDiff = diff;
				memcpy(&nearestMonsterIntersect, &collisionVector, sizeof(engine3D_vector2f_t));
				nearestMonster = level->monsters + i;
				seenAnyMonsters = true;
			}
		}

		if (seenAnyMonsters && (!ret ||
			engine3D_vector2f_length(engine3D_vector2f_sub(&nearestMonsterIntersect, lineStart, &tmp)) <
			engine3D_vector2f_length(engine3D_vector2f_sub(nearestIntersection, lineStart, &tmp))))
		{
			wfstn3D_monster_damage(nearestMonster, wfstn3D_player_getDamage());
		}
	}

	return ret;
}
