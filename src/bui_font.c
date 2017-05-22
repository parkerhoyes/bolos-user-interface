/*
 * License for the BOLOS User Interface Library project, originally found here:
 * https://github.com/parkerhoyes/bolos-user-interface
 *
 * Copyright (C) 2016, 2017 Parker Hoyes <contact@parkerhoyes.com>
 *
 * This software is provided "as-is", without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the
 * use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not claim
 *    that you wrote the original software. If you use this software in a
 *    product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include "bui_font.h"

#include <stdint.h>
#include <stddef.h>

#include "os.h"

#include "bui.h"

typedef struct __attribute__((packed)) {
	uint16_t bitmap_offset; // The starting index of the character's bitmap in the font bitmap
	uint8_t char_width; // Character width, in pixels
} bui_font_char_t;

// NOTE: Despite the font's range, they never include characters in the range 0x80 to 0x9F (both inclusive)
typedef struct __attribute__((packed)) {
	const bui_font_char_t *chars;
	const uint8_t *bitmaps; // Array of bitmaps for all characters
	bui_font_info_t info;
} bui_font_data_t;

static const uint32_t bui_font_palette[] = {
	BUI_CLR_TRANSPARENT,
	BUI_CLR_WHITE,
};

#include "bui_font_fonts.inc"

const bui_font_t bui_font_null = NULL;

#define BUI_FONT_DATA_FOR_ID(id) ((const bui_font_data_t*) PIC(id))

const bui_font_info_t* bui_font_get_font_info(bui_font_t font) {
	return &BUI_FONT_DATA_FOR_ID(font)->info;
}

uint8_t bui_font_get_char_width(bui_font_t font, char ch) {
	const bui_font_data_t *font_data = BUI_FONT_DATA_FOR_ID(font);
	uint8_t chari = ch;
	if (chari >= 0x80)
		chari -= 0xA0 - 0x80;
	chari -= font_data->info.first_char;
	return ((const bui_font_char_t*) PIC(font_data->chars))[chari].char_width;
}

const uint8_t* bui_font_get_char_bitmap(bui_font_t font, char ch, int16_t *w_dest) {
	const bui_font_data_t *font_data = BUI_FONT_DATA_FOR_ID(font);
	uint8_t chari = ch;
	if (chari >= 0x80)
		chari -= 0xA0 - 0x80;
	chari -= font_data->info.first_char;
	bui_font_char_t font_char;
	os_memcpy(&font_char, &((const bui_font_char_t*) PIC(font_data->chars))[chari], sizeof(bui_font_char_t));
	if (w_dest != NULL)
		*w_dest = font_char.char_width;
	return (const uint8_t*) PIC(font_data->bitmaps) + font_char.bitmap_offset;
}

void bui_font_draw_char(bui_ctx_t *ctx, char ch, int16_t x, int16_t y, bui_dir_e alignment, bui_font_t font) {
	const bui_font_info_t *font_info = bui_font_get_font_info(font);
	int16_t h = font_info->char_height;
	int16_t w;
	const uint8_t *bitmap = bui_font_get_char_bitmap(font, ch, &w);
	if (BUI_DIR_IS_HTL_CENTER(alignment)) {
		x -= w / 2;
		if (w % 2 == 1)
			x -= 1;
	} else if (BUI_DIR_IS_RIGHT(alignment)) {
		x -= w;
	}
	if (BUI_DIR_IS_VTL_CENTER(alignment)) {
		y -= h / 2;
		if (h % 2 == 1)
			y -= 1;
	} else if (BUI_DIR_IS_BOTTOM(alignment)) {
		y -= h;
	}
	bui_ctx_draw_bitmap_full(ctx, (bui_const_bitmap_t) {
		.w = w,
		.h = h,
		.bb = bitmap,
		.plt = bui_font_palette,
		.bpp = 1,
	}, x, y);
}

void bui_font_draw_string(bui_ctx_t *ctx, const char *str, int16_t x, int16_t y, bui_dir_e alignment, bui_font_t font) {
	const bui_font_info_t *font_info = bui_font_get_font_info(font);
	if (BUI_DIR_IS_VTL_CENTER(alignment)) {
		y -= font_info->baseline_height / 2;
		if (font_info->baseline_height % 2 == 1)
			y -= 1;
	} else if (BUI_DIR_IS_BOTTOM(alignment)) {
		y -= font_info->baseline_height;
	}
	if (y >= 32 || y + font_info->char_height <= 0)
		return;
	if (!BUI_DIR_IS_LEFT(alignment)) {
		int16_t w = 0;
		for (const char *s = str; *s != '\0'; s++) {
			w += bui_font_get_char_width(font, *s);
			w += font_info->char_kerning;
		}
		if (BUI_DIR_IS_HTL_CENTER(alignment)) {
			x -= w / 2;
			if (w % 2 == 1)
				x -= 1;
		} else {
			x -= w;
		}
		if (x + w <= 0)
			return;
	}
	for (; *str != '\0' && x < 128; str++) {
		int16_t w;
		const uint8_t *bitmap = bui_font_get_char_bitmap(font, *str, &w);
		bui_ctx_draw_bitmap_full(ctx, (bui_const_bitmap_t) {
			.w = w,
			.h = font_info->char_height,
			.bb = bitmap,
			.plt = bui_font_palette,
			.bpp = 1,
		}, x, y);
		x += w;
		x += font_info->char_kerning;
	}
}
