#ifndef ENGINE3D_WINDOW_H
#define ENGINE3D_WINDOW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include <stdbool.h>

// Create the window. Bail on error.
GLFWwindow *engine3D_window_create(const int width, const int height, const char *const title);

// Poll events and perform the framebuffer swap.
// If on debug mode, query for OpenGL errors.
void engine3D_window_update(void);

// True if window must be closed.
bool engine3D_window_closeRequested(void);

// Set the ints which pointers are given to the window's current width and height.
// NULL is an accepted value for any of the two pointers.
void engine3D_window_getSize(int *const width, int *const height);

// Terminate and destroy the window.
void engine3D_window_destroy(void);

#endif /* ENGINE3D_WINDOW_H */
