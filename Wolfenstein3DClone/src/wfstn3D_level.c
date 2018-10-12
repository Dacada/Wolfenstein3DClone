#include <wfstn3D_level.h>
#include <wfstn3D_bitmap.h>

#include <Engine3D/engine3D_mesh.h>
#include <Engine3D/engine3D_texture.h>
#include <Engine3D/engine3D_vector.h>
#include <Engine3D/engine3D_resourceLoader.h>
#include <Engine3D/engine3D_util.h>

#include <stdbool.h>

#define SPOT_WIDTH (1.0f)
#define SPOT_LENGTH (1.0f)
#define SPOT_HEIGHT (1.0f)
#define NUM_TEX_EXP (4)
#define NUM_TEXTURES (NUM_TEX_EXP * NUM_TEX_EXP)

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

static void generateLevel(engine3D_vertex_t **vertices, size_t *vertices_len, unsigned int **indices, size_t *indices_len, wfstn3D_bitmap_t *levelBitmap) {
	size_t vertices_capacity = 1024;
	size_t indices_capacity = 1024;
	engine3D_vertex_t *vertices_array = engine3D_util_safeMalloc(sizeof(engine3D_vertex_t) * vertices_capacity);
	unsigned int *indices_array = engine3D_util_safeMalloc(sizeof(unsigned int) * indices_capacity);
	size_t vertices_index = 0;
	size_t indices_index = 0;

	for (size_t i = 0; i < levelBitmap->width; i++) {
		for (size_t j = 0; j < levelBitmap->height; j++) {
			uint32_t pixel = wfstn3D_bitmap_getPixel(levelBitmap, i, j);
			if ((wfstn3D_bitmap_getPixel(levelBitmap, i, j) & 0xFFFFFF) != 0) {
				float XLower, XHigher, YLower, YHigher;
				getTexCoords(((pixel & 0x00FF00) >> 8) / NUM_TEXTURES, &XLower, &XHigher, &YLower, &YHigher);

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
	level->bitmap = malloc(sizeof(wfstn3D_bitmap_t));
	wfstn3D_bitmap_load(levelname, level->bitmap);

	level->shader = malloc(sizeof(engine3D_basicShader_t));
	engine3D_basicShader_init(level->shader);
	engine3D_texture_t *texture = malloc(sizeof(engine3D_texture_t));
	engine3D_vector3f_t *color = malloc(sizeof(engine3D_vector3f_t));
	level->material = malloc(sizeof(engine3D_material_t));
	engine3D_resourceLoader_loadTexture(texturename, texture);

	// TODO: Create function to bind texture to material and setup default values
	color->x = color->y = color->z = 1;
	level->material->texture = texture;
	level->material->color = color;
	level->material->specularIntensity = 0;
	level->material->specularPower = 0;

	level->mesh = malloc(sizeof(engine3D_mesh_t));
	engine3D_mesh_init(level->mesh);

	size_t vertices_len, indices_len;
	engine3D_vertex_t *vertices;
	unsigned int *indices;
	generateLevel(&vertices, &vertices_len, &indices, &indices_len, level->bitmap);

	engine3D_mesh_addVertices(level->mesh, vertices, vertices_len, indices, indices_len, true);
	//engine3D_mesh_addVertices(&mesh, vertices, 4, indices, 6, true);

	free(vertices);
	free(indices);

	level->transform = malloc(sizeof(engine3D_transform_t));
	engine3D_transform_reset(level->transform);
}

void wfstn3D_level_input(const wfstn3D_level_t *const level) {
	(void)level;
}

void wfstn3D_level_update(const wfstn3D_level_t *const level) {
	(void)level;
}

void wfstn3D_level_render(const wfstn3D_level_t *const level) {
	engine3D_shader_bind((engine3D_shader_t*)(level->shader));

	engine3D_matrix4f_t worldMatrix, projectedMatrix;
	engine3D_transform_getTransformation(level->transform, &worldMatrix);
	engine3D_transform_getProjectedTransformation(level->transform, &projectedMatrix);
	engine3D_basicShader_updateUniforms(level->shader, &worldMatrix, &projectedMatrix, level->material);
	engine3D_mesh_draw(level->mesh);
}

void wfstn3D_level_unload(wfstn3D_level_t *const level) {
	wfstn3D_bitmap_unload(level->bitmap);
	free(level->bitmap);
	free(level->shader);
	free(level->material->texture);
	free(level->material->color);
	free(level->material);
	free(level->mesh);
	free(level->transform);
}