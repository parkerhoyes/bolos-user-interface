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

#ifndef BUI_FONT_H_
#define BUI_FONT_H_

#include <stdint.h>

#include "bui.h"

// NOTE: The definition of this type is considered internal. It may be changed between versions without warning.
typedef const void* bui_font_t;

// This is not a valid font; it may be used as a unique identifier, and is guaranteed to be equal to the
// zero-initialized value of bui_font_t.
extern const bui_font_t bui_font_null;
extern const bui_font_t bui_font_comic_sans_ms_20;
extern const bui_font_t bui_font_lucida_console_8;
extern const bui_font_t bui_font_lucida_console_15;
extern const bui_font_t bui_font_open_sans_bold_13;
extern const bui_font_t bui_font_open_sans_bold_21;
extern const bui_font_t bui_font_open_sans_extrabold_11;
extern const bui_font_t bui_font_open_sans_light_13;
extern const bui_font_t bui_font_open_sans_light_14;
extern const bui_font_t bui_font_open_sans_light_16;
extern const bui_font_t bui_font_open_sans_light_20;
extern const bui_font_t bui_font_open_sans_light_21;
extern const bui_font_t bui_font_open_sans_light_32;
extern const bui_font_t bui_font_open_sans_regular_11;
extern const bui_font_t bui_font_open_sans_semibold_18;

// NOTE: Despite the font's range, they never include characters in the range 0x80 to 0x9F (both inclusive)
typedef struct {
	uint8_t char_height;
	uint8_t baseline_height;
	uint8_t char_kerning;
	uint8_t first_char; // Character code of the first character with a bitmap in this font
	uint8_t last_char; // Character code of the last character with a bitmap in this font
} bui_font_info_t;

/*
 * Get the info for a particular font.
 *
 * Args:
 *     font: the font
 * Returns:
 *     a pointer to a structure containing info about the specified font
 */
const bui_font_info_t* bui_font_get_font_info(bui_font_t font);

/*
 * Get the width of a given character in the specified font.
 *
 * Args:
 *     font: the font
 *     ch: the character code
 * Returns:
 *     the width of the given character, in pixels
 */
uint8_t bui_font_get_char_width(bui_font_t font, char ch);

/*
 * Get the pointer to the bitmap for a character in a particular font.
 *
 * Args:
 *     font: the font
 *     ch: the character code
 *     w_dest: a pointer to an int in which the width of the character will be stored; if this is NULL, it is not
 *             accessed
 * Returns:
 *     the pointer to the bitmap for the specified character in the specified font
 */
const uint8_t* bui_font_get_char_bitmap(bui_font_t font, char ch, int16_t *w_dest);

/*
 * Draw a character in the specified font in the specified BUI context. Any part of the character out of bounds of the
 * display will not be drawn. The coordinates provided determine the position of the text anchor. The actual bounds the
 * text is drawn within are determined by the anchor and the alignment. The alignment determines where, in the text's
 * bounds, the anchor is located. For example, an alignment of BUI_DIR_RIGHT_TOP will place the anchor at the top-most,
 * right-most position of the text's boundaries. Note that for purposes of alignment, the character's boundaries are
 * considered to extend from the font's baseline to the ascender height.
 *
 * Args:
 *     ctx: the BUI context in which the character is to be drawn
 *     ch: the character code of the character to be drawn
 *     x: the x-coordinate of the text anchor; must be >= -32,768 and <= 32,767
 *     y: the y-coordinate of the text anchor; must be >= -32,768 and <= 32,767
 *     alignment: the position of the anchor within the text boundaries
 *     font: the font to be used to render the character
 */
void bui_font_draw_char(bui_ctx_t *ctx, char ch, int16_t x, int16_t y, bui_dir_t alignment, bui_font_t font);

/*
 * Draw a string in the specified font in the specified BUI context. Any part of the string out of bounds of the display
 * will not be drawn (the string will not wrap). The coordinates provided determine the position of the text anchor. The
 * actual bounds the text is drawn within are determined by the anchor and the alignment. The alignment determines
 * where, in the text's bounds, the anchor is located. For example, an alignment of BUI_DIR_RIGHT_TOP will place the
 * anchor at the top-most, right-most position of the text's boundaries. Note that for purposes of alignment, the
 * character's boundaries are considered to extend from the font's baseline to the ascender height.
 *
 * Args:
 *     ctx: the BUI context in which the string is to be drawn
 *     str: the null-terminated string to be drawn; may not be NULL
 *     x: the x-coordinate of the text anchor; must be >= -32,768 and <= 32,767
 *     y: the y-coordinate of the text anchor; must be >= -32,768 and <= 32,767
 *     alignment: the position of the anchor within the text boundaries
 *     font: the font to be used to render the string
 */
void bui_font_draw_string(bui_ctx_t *ctx, const char *str, int16_t x, int16_t y, bui_dir_t alignment, bui_font_t font);

#endif
