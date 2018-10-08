#ifndef WFSTN3D_BITMAP_H
#define WFSTN3D_BITMAP_H

#include <stdint.h>
#include <stdlib.h>

typedef struct wfstn3D_bitmap_t {
	size_t width;
	size_t height;
	uint32_t *pixels;
} wfstn3D_bitmap_t;

void wfstn3D_bitmap_load(const char *const filename, wfstn3D_bitmap_t *const bitmap);

void wfstn3D_bitmap_unload(wfstn3D_bitmap_t *const bitmap);

wfstn3D_bitmap_t *wfstn3D_bitmap_flipX(wfstn3D_bitmap_t *const bitmap);
wfstn3D_bitmap_t *wfstn3D_bitmap_flipY(wfstn3D_bitmap_t *const bitmap);

uint32_t wfstn3D_bitmap_getPixel(const wfstn3D_bitmap_t *const bitmap, size_t x, size_t y);

void wfstn3D_bitmap_setPixel(const wfstn3D_bitmap_t *const bitmap, size_t x, size_t y, uint32_t value);

#endif
