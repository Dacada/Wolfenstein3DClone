#ifndef ENGINE3D_PHONGSHADER
#define ENGINE3D_PHONGSHADER

#include "engine3D_shader.h"
#include "engine3D_material.h"
#include "engine3D_vector.h"

#define ENGINE3D_PHONGSHADER_MAXPOINTLIGHTS 4
#define ENGINE3D_PHONGSHADER_MAXSPOTLIGHTS 4

typedef struct engine3D_phongShader_t {
	engine3D_shader_t shader;
} engine3D_phongShader_t;

typedef struct engine3D_phongShader_baseLight_t {
	engine3D_vector3f_t color;
	float intensity;
} engine3D_phongShader_baseLight_t;

typedef struct engine3D_phongShader_directionalLight_t {
	engine3D_phongShader_baseLight_t base;
	engine3D_vector3f_t direction;
} engine3D_phongShader_directionalLight_t;

typedef struct engine3D_phongShader_attenuation_t {
	float constant;
	float linear;
	float exponent;
} engine3D_phongShader_attenuation_t;

typedef struct engine3D_phongShader_pointLight_t {
	engine3D_phongShader_baseLight_t base;
	engine3D_phongShader_attenuation_t atten;
	engine3D_vector3f_t position;
	float range;
} engine3D_phongShader_pointLight_t;

typedef struct engine3D_phongShader_spotLight_t {
	engine3D_phongShader_pointLight_t pointLight;
	engine3D_vector3f_t direction;
	float cutoff;
} engine3D_phongShader_spotLight_t;

extern engine3D_vector3f_t engine3D_phongShader_ambientLight;

extern engine3D_phongShader_directionalLight_t engine3D_phongShader_directionalLight;

extern engine3D_phongShader_pointLight_t engine3D_phongShader_pointLights[ENGINE3D_PHONGSHADER_MAXPOINTLIGHTS];
extern size_t engine3D_phongShader_numberOfPointLights;

extern engine3D_phongShader_spotLight_t engine3D_phongShader_spotLights[ENGINE3D_PHONGSHADER_MAXSPOTLIGHTS];
extern size_t engine3D_phongShader_numberOfSpotLights;

engine3D_phongShader_t *engine3D_phongShader_init(engine3D_phongShader_t *const shader);

void engine3D_phongShader_updateUniforms(engine3D_phongShader_t * const shader, const engine3D_matrix4f_t *const worldMatrix, const engine3D_matrix4f_t *projectedMatrix, const engine3D_material_t *const material);

void engine3D_phongShader_setUniformBaseLight(const char *const uniform, const engine3D_phongShader_baseLight_t *const value, const engine3D_phongShader_t *const shader);

void engine3D_phongShader_setUniformDirectionalLight(const char *const uniform, const engine3D_phongShader_directionalLight_t *const value, const engine3D_phongShader_t *const shader);

void engine3D_phongShader_setUniformPointLight(const char *const uniform, const engine3D_phongShader_pointLight_t *const value, const engine3D_phongShader_t *const shader);

void engine3D_phongShader_setUniformSpotLight(const char *const uniform, const engine3D_phongShader_spotLight_t *const value, const engine3D_phongShader_t *const shader);

#endif /* ENGINE3D_BASICSHADER */
