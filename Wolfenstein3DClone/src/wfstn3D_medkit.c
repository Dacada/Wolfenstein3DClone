#include <wfstn3D_medkit.h>

#include <Engine3D/engine3D_mesh.h>

#define SCALE (0.7f)

#define START (0.0f)
#define SIZEY SCALE
#define SIZEX (SIZEY / (2.0f * 1.93103f))

#define TEX_MIN_X (0.0f)
#define TEX_MAX_X (-1.0f)
#define TEX_MIN_Y (0.0f)
#define TEX_MAX_Y (-1.0f)

engine3D_mesh_t mesh;
engine3D_material_t material;
bool meshIsLoaded = false;
bool materialIsLoaded = false;

void wfstn3D_medkit_init(wfstn3D_medkit_t *const medkit, const engine3D_vector3f_t *const position, wfstn3D_level_t *const level) {
	medkit->level = level;

	if (!meshIsLoaded) {
		engine3D_vertex_t vertices[4] = { { { -SIZEX,START,START },{ TEX_MAX_X,TEX_MAX_Y },{ 0,0,0 } },
		                                  { { -SIZEX,SIZEY,START },{ TEX_MAX_X,TEX_MIN_Y },{ 0,0,0 } },
		                                  { {  SIZEX,SIZEY,START },{ TEX_MIN_X,TEX_MIN_Y },{ 0,0,0 } },
		                                  { {  SIZEX,START,START },{ TEX_MIN_X,TEX_MAX_Y },{ 0,0,0 } } };

		unsigned int indices[6] = { 0, 1, 2,
			0, 2, 3 };

		engine3D_mesh_init(&mesh);
		engine3D_mesh_addVertices(&mesh, vertices, 4, indices, 6, true);

		meshIsLoaded = true;
	}

	if (!materialIsLoaded) {
		material.color = engine3D_util_safeMalloc(sizeof(engine3D_vector3f_t));
		material.texture = engine3D_util_safeMalloc(sizeof(engine3D_texture_t));

		material.color->x = 1;
		material.color->y = 1;
		material.color->z = 1;
		engine3D_resourceLoader_loadTexture("MEDIA0.png", material.texture);
		material.specularIntensity = 0;
		material.specularPower = 0;
	}
}

void wfstn3D_medkit_input(wfstn3D_medkit_t *const medkit) {
}

void wfstn3D_medkit_update(wfstn3D_medkit_t *const medkit) {
}

void wfstn3D_medkit_render(wfstn3D_medkit_t *const medkit) {
	engine3D_basicShader_t *shader = medkit->level->shader;
	engine3D_matrix4f_t transformationMatrix, projectedTransformationMatrix;

	engine3D_transform_getTransformation(&medkit->transform, &transformationMatrix);
	engine3D_transform_getProjectedTransformation(&medkit->transform, &projectedTransformationMatrix);
	engine3D_basicShader_updateUniforms(shader, &transformationMatrix, &projectedTransformationMatrix, &material);

	engine3D_mesh_draw(&mesh);
}

void wfstn3D_medkit_cleanup(wfstn3D_medkit_t *const medkit) {
}
