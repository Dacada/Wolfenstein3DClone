#include <wfstn3D_bitmap.h>

#include <Engine3D/engine3D_main.h>
#include <Engine3D/engine3D_window.h>
#include <Engine3D/engine3D_input.h>
#include <Engine3D/engine3D_time.h>
#include <Engine3D/engine3D_vector.h>
#include <Engine3D/engine3D_transform.h>
#include <Engine3D/engine3D_resourceLoader.h>
#include <Engine3D/engine3D_basicShader.h>
#include <Engine3D/engine3D_texture.h>
#include <Engine3D/engine3D_util.h>

#include <stdio.h>
#include <stdlib.h>

#define SPOT_WIDTH (1.0f)
#define SPOT_LENGTH (1.0f)
#define SPOT_HEIGHT (1.0f)

wfstn3D_bitmap_t level;
engine3D_basicShader_t shader;
engine3D_texture_t texture;
engine3D_vector3f_t color;
engine3D_material_t material;
engine3D_mesh_t mesh;
engine3D_transform_t transform;

engine3D_vertex_t *vertices;
unsigned int *indices;

static void generateLevel(engine3D_vertex_t **vertices, size_t *vertices_len, unsigned int **indices, size_t *indices_len) {
	size_t vertices_capacity = 64;
	size_t indices_capacity = 64;
	engine3D_vertex_t *vertices_array = engine3D_util_safeMalloc(sizeof(engine3D_vertex_t) * vertices_capacity);
	unsigned int *indices_array = engine3D_util_safeMalloc(sizeof(unsigned int) * indices_capacity);
	size_t vertices_index = 0;
	size_t indices_index = 0;

	for (size_t i = 0; i < level.height; i++) {
		for (size_t j = 0; j < level.width; j++) {
			float XHigher = 1;
			float XLower = 0;
			float YHigher = 1;
			float YLower = 0;

			if (vertices_index + 4 >= vertices_capacity) {
				vertices_capacity *= 2;
				vertices_array = engine3D_util_safeRealloc(vertices_array, sizeof(engine3D_vertex_t) * vertices_capacity);
			}

			if (indices_index + 6 >= indices_capacity) {
				indices_capacity *= 2;
				indices_array = engine3D_util_safeRealloc(indices_array, sizeof(unsigned int) * indices_capacity);
			}

			// FLOOR //
			indices_array[indices_index++] = vertices_index + 2;
			indices_array[indices_index++] = vertices_index + 1;
			indices_array[indices_index++] = vertices_index + 0;
			indices_array[indices_index++] = vertices_index + 3;
			indices_array[indices_index++] = vertices_index + 2;
			indices_array[indices_index++] = vertices_index + 0;

			vertices_array[vertices_index].vec.x = i * SPOT_WIDTH;
			vertices_array[vertices_index].vec.y = 0;
			vertices_array[vertices_index].vec.z = j * SPOT_LENGTH;
			vertices_array[vertices_index].texCoord.x = XLower;
			vertices_array[vertices_index].texCoord.y = YLower;
			vertices_array[vertices_index].normal.x = 0;
			vertices_array[vertices_index].normal.y = 0;
			vertices_array[vertices_index++].normal.z = 0;

			vertices_array[vertices_index].vec.x = (i + 1) * SPOT_WIDTH;
			vertices_array[vertices_index].vec.y = 0;
			vertices_array[vertices_index].vec.z = j * SPOT_LENGTH;
			vertices_array[vertices_index].texCoord.x = XHigher;
			vertices_array[vertices_index].texCoord.y = YLower;
			vertices_array[vertices_index].normal.x = 0;
			vertices_array[vertices_index].normal.y = 0;
			vertices_array[vertices_index++].normal.z = 0;

			vertices_array[vertices_index].vec.x = (i + 1) * SPOT_WIDTH;
			vertices_array[vertices_index].vec.y = 0;
			vertices_array[vertices_index].vec.z = (j + 1) * SPOT_LENGTH;
			vertices_array[vertices_index].texCoord.x = XHigher;
			vertices_array[vertices_index].texCoord.y = YHigher;
			vertices_array[vertices_index].normal.x = 0;
			vertices_array[vertices_index].normal.y = 0;
			vertices_array[vertices_index++].normal.z = 0;

			vertices_array[vertices_index].vec.x = i * SPOT_WIDTH;
			vertices_array[vertices_index].vec.y = 0;
			vertices_array[vertices_index].vec.z = (j + 1) * SPOT_LENGTH;
			vertices_array[vertices_index].texCoord.x = XLower;
			vertices_array[vertices_index].texCoord.y = YHigher;
			vertices_array[vertices_index].normal.x = 0;
			vertices_array[vertices_index].normal.y = 0;
			vertices_array[vertices_index++].normal.z = 0;

			uint32_t pixel = wfstn3D_bitmap_getPixel(&level, j, i);
			if (pixel == 0xFF000000) {
				putchar('#');
			}
			else if (pixel == 0xFFFFFFFF) {
				putchar(' ');
			}
			else {
				engine3D_util_bail("Error: Level generation bitmap must be composed of black or white pixel data and 100% alpha");
			}
		}
		putchar('\n');
	}

	vertices_array = engine3D_util_safeRealloc(vertices_array, sizeof(engine3D_vertex_t) * vertices_index);
	indices_array = engine3D_util_safeRealloc(indices_array, sizeof(unsigned int) * indices_index);

	*vertices = vertices_array;
	*indices = indices_array;
	*vertices_len = vertices_index;
	*indices_len = indices_index;
}


static void init(void) {
	engine3D_resourceLoader_setResPath("../Wolfenstein3DClone/res/");
	wfstn3D_bitmap_load("level1.png", &level);

	//for (size_t i = 0; i < level.height; i++) {
	//	for (size_t j = 0; j < level.width; j++) {
	//		uint32_t pixel = wfstn3D_bitmap_getPixel(&level, j, i);
	//		if (pixel == 0xFF000000)
	//			putchar('#');
	//		else if (pixel == 0xFFFFFFFF)
	//			putchar(' ');
	//		else
	//			putchar('?');
	//	}
	//	putchar('\n');
	//}

	engine3D_basicShader_init(&shader);
	engine3D_resourceLoader_loadTexture("test.png", &texture);
	// TODO: Create function to bind texture to material and setup default values
	color.x = color.y = color.z = 1;
	material.texture = &texture;
	material.color = &color;
	material.specularIntensity = 0;
	material.specularPower = 0;

	engine3D_mesh_init(&mesh);

	size_t vertices_len, indices_len;

	//engine3D_vertex_t vertices[4] = {
	//	{ { 0, 0, 0 },{ 0, 0 },{ 0, 0, 0 } },
	//	{ { 0, 1, 0 },{ 0, 1 },{ 0, 0, 0 } },
	//	{ { 1, 1, 0 },{ 1, 1 },{ 0, 0, 0 } },
	//	{ { 1, 0, 0 },{ 1, 0 },{ 0, 0, 0 } }
	//};
	//unsigned int indices[6] = {
	//	0, 1, 2,
	//	0, 2, 3
	//};
	generateLevel(&vertices, &vertices_len, &indices, &indices_len);

	engine3D_mesh_addVertices(&mesh, vertices, vertices_len, indices, indices_len, true);
	//engine3D_mesh_addVertices(&mesh, vertices, 4, indices, 6, true);

	engine3D_camera_init(&engine3D_transform_camera);
	engine3D_transform_reset(&transform);
	engine3D_transform_fov = 70;
	int width, height;
	engine3D_window_getSize(&width, &height);
	engine3D_transform_width = (float)width;
	engine3D_transform_height = (float)height;
	engine3D_transform_zNear = 0.01f;
	engine3D_transform_zFar = 1000;
}

// TODO: Set up cameraInput as part of the engine itself
static void cameraInput(float delta) {
	float sensitivity = 0.5f;
	float movAmt = delta * 10;

	static bool movingCamera = false;
	static engine3D_vector2f_t centerPosition;
	if (engine3D_input_getMouseDown(GLFW_MOUSE_BUTTON_1)) {
		engine3D_input_setCursor(ENGINE3D_CURSOR_DISABLED);
		engine3D_input_getMousePosition(&centerPosition);
		movingCamera = true;
	}

	if (engine3D_input_getMouseUp(GLFW_MOUSE_BUTTON_1)) {
		engine3D_input_setCursor(ENGINE3D_CURSOR_ENABLED);
		movingCamera = false;
	}

	if (engine3D_input_getKey(GLFW_KEY_W)) {
		engine3D_camera_move(&engine3D_transform_camera, &engine3D_transform_camera.forward, movAmt);
	}
	if (engine3D_input_getKey(GLFW_KEY_S)) {
		engine3D_camera_move(&engine3D_transform_camera, &engine3D_transform_camera.forward, -movAmt);
	}
	if (engine3D_input_getKey(GLFW_KEY_A)) {
		engine3D_vector3f_t vec;
		engine3D_camera_left(&engine3D_transform_camera, &vec);
		engine3D_camera_move(&engine3D_transform_camera, &vec, movAmt);
	}
	if (engine3D_input_getKey(GLFW_KEY_D)) {
		engine3D_vector3f_t vec;
		engine3D_camera_right(&engine3D_transform_camera, &vec);
		engine3D_camera_move(&engine3D_transform_camera, &vec, movAmt);
	}

	if (movingCamera) {
		engine3D_vector2f_t pos, deltaPos;
		engine3D_input_getMousePosition(&pos);
		engine3D_vector2f_sub(&pos, &centerPosition, &deltaPos);

		if (deltaPos.x != 0)
		{
			engine3D_camera_rotateY(&engine3D_transform_camera, deltaPos.x * sensitivity * delta);
		}

		if (deltaPos.y != 0)
		{
			engine3D_camera_rotateX(&engine3D_transform_camera, deltaPos.y * sensitivity * delta);
		}
	}
}

static void input(void) {
	cameraInput((float)engine3D_time_getDelta());
}

static void update(void) {

}

static void render(void) {
	engine3D_shader_bind((engine3D_shader_t*)&shader);

	engine3D_matrix4f_t worldMatrix, projectedMatrix;
	engine3D_transform_getTransformation(&transform, &worldMatrix);
	engine3D_transform_getProjectedTransformation(&transform, &projectedMatrix);
	engine3D_basicShader_updateUniforms(&shader, &worldMatrix, &projectedMatrix, &material);
	engine3D_mesh_draw(&mesh);
}

static void cleanup(void) {
	wfstn3D_bitmap_unload(&level);
	free(vertices);
	free(indices);
}

int main(void) {
	engine3D_setGame_init(init);
	engine3D_setGame_input(input);
	engine3D_setGame_update(update);
	engine3D_setGame_render(render);

	engine3D_init();
	engine3D_start();

	cleanup();

	return EXIT_SUCCESS;
}
