#ifndef ENGINE3D_INPUT_H
#define ENGINE3D_INPUT_H

#include "engine3D_vector.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include <stdbool.h>

typedef enum engine3D_input_cursorMode {
	ENGINE3D_CURSOR_ENABLED,  // Normal
	ENGINE3D_CURSOR_HIDDEN,   // Invisible but usable
	ENGINE3D_CURSOR_DISABLED, // Invisible and limitless movement (camera control mode)
} engine3D_input_cursorMode;

extern GLFWwindow *engine3D_input_window;

void engine3D_input_update(void);

bool engine3D_input_getKey(const int keyCode);

bool engine3D_input_getKeyDown(const int keyCode);

bool engine3D_input_getKeyUp(const int keyCode);

bool engine3D_input_getMouse(const int mouseButton);

bool engine3D_input_getMouseDown(const int mouseButton);

bool engine3D_input_getMouseUp(const int mouseButton);

void engine3D_input_getMousePosition(engine3D_vector2f_t *const position);

void engine3D_input_setMousePosition(const engine3D_vector2f_t *const position);

void engine3D_input_setCursor(engine3D_input_cursorMode mode);

void _engine3D_input_keyCallback(const GLFWwindow *const window, const int key, const int scancode, const int action, const int mods);

void _engine3D_input_mouseButtonCallback(const GLFWwindow *const window, const int button, const int action, const int mods);

#endif /* ENGINE3D_INPUT_H */
