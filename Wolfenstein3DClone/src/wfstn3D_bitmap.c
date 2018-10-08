#include <wfstn3D_bitmap.h>

#include <Engine3D/engine3D_util.h>
#include <FreeImage.h>

#include <stdint.h>
#include <stdlib.h>

#include <string.h>

void wfstn3D_bitmap_load(const char *const filename, wfstn3D_bitmap_t *const bitmap) {
	char filepath[256] = "../Wolfenstein3DClone/res/bitmaps/";
	strncat(filepath, filename, 128);

	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP *dib = NULL;
	BYTE* bits;
	unsigned int width, height;

	fif = FreeImage_GetFileType(filepath, 0);
	if (fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilename(filepath);
	if (fif == FIF_UNKNOWN)
		engine3D_util_bail("attempt to load texture of unknown image type");

	if (FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, filepath, 0);

	if (!dib)
		engine3D_util_bail("failed to load image for texture");

	FIBITMAP *dib32 = FreeImage_ConvertTo32Bits(dib);
	FreeImage_Unload(dib);

	bits = FreeImage_GetBits(dib32);
	width = FreeImage_GetWidth(dib32);
	height = FreeImage_GetHeight(dib32);
	if (bits == NULL || width == 0 || height == 0 || FreeImage_GetBPP(dib32) != 32)
		engine3D_util_bail("failed to load image data for texture");

	bitmap->width = width;
	bitmap->height = height;
	bitmap->pixels = engine3D_util_safeMalloc(height * width * sizeof(uint32_t));
	memcpy(bitmap->pixels, bits, height * width * sizeof(uint32_t));

	FreeImage_Unload(dib32);
}

void wfstn3D_bitmap_unload(wfstn3D_bitmap_t *const bitmap) {
	free(bitmap->pixels);
	bitmap->pixels = NULL;
	bitmap->width = 0;
	bitmap->height = 0;
}

wfstn3D_bitmap_t *wfstn3D_bitmap_flipX(wfstn3D_bitmap_t *const bitmap) {
	uint32_t *tmp = engine3D_util_safeMalloc(bitmap->height * bitmap->width * sizeof(uint32_t));

	for (int i = 0; i < bitmap->width; i++) {
		for (int j = 0; j < bitmap->height; j++) {
			tmp[i + j * bitmap->width] = bitmap->pixels[(bitmap->width - i - 1) + j * bitmap->width];
		}
	}

	free(bitmap->pixels);
	bitmap->pixels = tmp;
	return bitmap;
}
wfstn3D_bitmap_t *wfstn3D_bitmap_flipY(wfstn3D_bitmap_t *const bitmap) {
	uint32_t *tmp = engine3D_util_safeMalloc(bitmap->height * bitmap->width * sizeof(uint32_t));

	for (int i = 0; i < bitmap->width; i++) {
		for (int j = 0; j < bitmap->height; j++) {
			tmp[i + j * bitmap->width] = bitmap->pixels[i + (bitmap->height - j - 1) * bitmap->width];
		}
	}

	free(bitmap->pixels);
	bitmap->pixels = tmp;
	return bitmap;
}

uint32_t wfstn3D_bitmap_getPixel(const wfstn3D_bitmap_t *const bitmap, size_t x, size_t y) {
	return bitmap->pixels[x + y * bitmap->width];
}

void wfstn3D_bitmap_setPixel(const wfstn3D_bitmap_t *const bitmap, size_t x, size_t y, uint32_t value) {
	bitmap->pixels[x + y * bitmap->width] = value;
}
