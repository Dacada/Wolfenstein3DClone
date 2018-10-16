#include <wfstn3D_door.h>

#include <Engine3D/engine3D_util.h>
#include <Engine3D/engine3D_time.h>
#include <Engine3D/engine3D_vector.h>
#include <Engine3D/engine3D_shader.h>
#include <Engine3D/engine3D_vertex.h>
#include <Engine3D/engine3D_mesh.h>

#include <stdbool.h>

#include <string.h>

static engine3D_mesh_t mesh;
static bool meshIsLoaded = false;

void wfstn3D_door_init(wfstn3D_door_t *const door, const engine3D_transform_t *const transform, engine3D_material_t *const material, wfstn3D_level_t *const level, const engine3D_vector3f_t *const openPosition) {
	memcpy(&door->transform, transform, sizeof(engine3D_transform_t));
	door->material = material;
	door->level = level;

	door->isOpening = false;
	memcpy(&door->closePosition, &door->transform.translation, sizeof(engine3D_vector3f_t));
	memcpy(&door->openPosition, openPosition, sizeof(engine3D_vector3f_t));

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

static engine3D_vector3f_t *vectorLerp(const engine3D_vector3f_t *const startPos, const engine3D_vector3f_t *const endPos, float lerpFactor, engine3D_vector3f_t *const result) {
	engine3D_vector3f_t tmp, tmp2;
	return engine3D_vector3f_add(startPos, engine3D_vector3f_mulf(engine3D_vector3f_sub(endPos, startPos, &tmp), lerpFactor, &tmp2), result);
}

void wfstn3D_door_update(wfstn3D_door_t *const door) {
	if (door->isOpening)
	{
		double time = engine3D_timer_getTime() / engine3D_timer_second;
		if (time < door->openTime) {
			engine3D_vector3f_t newPos;
			float lerpFactor = (time - door->openingStartTime) / WFSTN3D_DOOR_TIME_TO_OPEN;
			vectorLerp(&door->closePosition, &door->openPosition, lerpFactor, &newPos);
			memcpy(&door->transform.translation, &newPos, sizeof(engine3D_vector3f_t));
		}
		else if (time > door->openTime && time < closingStartTime) {
			memcpy(&door->transform.translation, &door->openPosition, sizeof(engine3D_vector3f_t));
		}
	}
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

void wfstn3D_door_open(wfstn3D_door_t *const door) {
	if (door->isOpening)
		return;

	door->openingStartTime = engine3D_timer_getTime() / engine3D_timer_second;
	door->openTime = door->openingStartTime + WFSTN3D_DOOR_TIME_TO_OPEN;
	door->closingStartTime = door->openTime + WFSTN3D_DOOR_CLOSE_DELAY;
	door->closeTime = door->closingStartTime + WFSTN3D_DOOR_TIME_TO_OPEN;

	door->isOpening = true;
}
