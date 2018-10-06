#ifndef ENGINE3D_UTIL_H
#define ENGINE3D_UTIL_H

#include <GL/glew.h>

#include <stdlib.h>

#ifdef _MSC_VER
__declspec(noreturn) void engine3D_util_bail(const char *const message);
#endif

#ifdef __GNUC__
void engine3D_util_bail(const char *const message) __attribute__((noreturn));
#endif

void engine3D_util_reportGlError(void);

int engine3D_util_errPrintf(const char *format, ...);

void *engine3D_util_safeMalloc(size_t size);

void *engine3D_util_safeRealloc(void *ptr, size_t size);

#endif /* ENGINE3D_UTIL_H */
