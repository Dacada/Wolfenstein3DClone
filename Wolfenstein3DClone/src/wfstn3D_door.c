#include <wfstn3D_door.h>

#include <Engine3D/engine3D_util.h>
#include <Engine3D/engine3D_vector.h>
#include <Engine3D/engine3D_shader.h>
#include <Engine3D/engine3D_vertex.h>
#include <Engine3D/engine3D_mesh.h>

#include <stdbool.h>

#include <string.h>

static engine3D_mesh_t mesh;
static bool meshIsLoaded = false;

void wfstn3D_door_init(wfstn3D_door_t *const door, const engine3D_transform_t *const transform, engine3D_material_t *const material, wfstn3D_level_t *const level) {
	memcpy(&door->transform, transform, sizeof(engine3D_transform_t));
	door->material = material;
	door->level = level;
	if (!meshIsLoaded) {
		engine3D_vertex_t vertices[16] = { { { WFSTN3D_DOOR_START ,WFSTN3D_DOOR_START ,WFSTN3D_DOOR_START },{ 0.50f,0.00f },{ 0,0,0 } },
									 	   { { WFSTN3D_DOOR_START ,WFSTN3D_DOOR_HEIGHT,WFSTN3D_DOOR_START },{ 0.50f,0.25f },{ 0,0,0 } },
								 		   { { WFSTN3D_DOOR_LENGTH,WFSTN3D_DOOR_HEIGHT,WFSTN3D_DOOR_START },{ 0.75f,0.25f },{ 0,0,0 } },
										   { { WFSTN3D_DOOR_LENGTH,WFSTN3D_DOOR_START ,WFSTN3D_DOOR_START },{ 0.75f,0.00f },{ 0,0,0 } },

										   { { WFSTN3D_DOOR_START ,WFSTN3D_DOOR_START ,WFSTN3D_DOOR_START },{ 0.73f,0.00f },{ 0,0,0 } },
										   { { WFSTN3D_DOOR_START ,WFSTN3D_DOOR_HEIGHT,WFSTN3D_DOOR_START },{ 0.73f,0.25f },{ 0,0,0 } },
										   { { WFSTN3D_DOOR_START ,WFSTN3D_DOOR_HEIGHT,WFSTN3D_DOOR_WIDTH },{ 0.75f,0.25f },{ 0,0,0 } },
										   { { WFSTN3D_DOOR_START ,WFSTN3D_DOOR_START ,WFSTN3D_DOOR_WIDTH },{ 0.75f,0.00f },{ 0,0,0 } },

										   { { WFSTN3D_DOOR_START ,WFSTN3D_DOOR_START ,WFSTN3D_DOOR_WIDTH },{ 0.50f,0.00f },{ 0,0,0 } },
										   { { WFSTN3D_DOOR_START ,WFSTN3D_DOOR_HEIGHT,WFSTN3D_DOOR_WIDTH },{ 0.50f,0.25f },{ 0,0,0 } },
										   { { WFSTN3D_DOOR_LENGTH,WFSTN3D_DOOR_HEIGHT,WFSTN3D_DOOR_WIDTH },{ 0.75f,0.25f },{ 0,0,0 } },
										   { { WFSTN3D_DOOR_LENGTH,WFSTN3D_DOOR_START ,WFSTN3D_DOOR_WIDTH },{ 0.75f,0.00f },{ 0,0,0 } },

										   { { WFSTN3D_DOOR_LENGTH,WFSTN3D_DOOR_START ,WFSTN3D_DOOR_START },{ 0.73f,0.00f },{ 0,0,0 } },
										   { { WFSTN3D_DOOR_LENGTH,WFSTN3D_DOOR_HEIGHT,WFSTN3D_DOOR_START },{ 0.73f,0.25f },{ 0,0,0 } },
										   { { WFSTN3D_DOOR_LENGTH,WFSTN3D_DOOR_HEIGHT,WFSTN3D_DOOR_WIDTH },{ 0.75f,0.25f },{ 0,0,0 } },
										   { { WFSTN3D_DOOR_LENGTH,WFSTN3D_DOOR_START ,WFSTN3D_DOOR_WIDTH },{ 0.75f,0.00f },{ 0,0,0 } } };

		unsigned int indices[24] = {  0, 1, 2,
								      0, 2, 3,

									  6, 5, 4,
									  7, 6, 4,

									 10, 9, 8,
									 11,10, 8,

			                         12,13,14,
			                         12,14,15 };

		engine3D_mesh_init(&mesh);
		engine3D_mesh_addVertices(&mesh, vertices, 16, indices, 24, true);

		meshIsLoaded = true;
	}
}

void wfstn3D_door_input(wfstn3D_door_t *const door) {
	(void)door;
}

void wfstn3D_door_update(wfstn3D_door_t *const door) {
	(void)door;
}

void wfstn3D_door_render(wfstn3D_door_t *const door) {
	engine3D_basicShader_t *shader = door->level->shader;
	engine3D_matrix4f_t transformationMatrix, projectedTransformationMatrix;

	engine3D_transform_getTransformation(&door->transform, &transformationMatrix);
	engine3D_transform_getProjectedTransformation(&door->transform, &projectedTransformationMatrix);
	engine3D_basicShader_updateUniforms(shader, &transformationMatrix, &projectedTransformationMatrix, door->material);

	engine3D_mesh_draw(&mesh);
}

void wfstn3D_door_cleanup(wfstn3D_door_t *const door) {
	(void)door;
}
