/*
 * License for the BOLOS User Interface Library project, originally found here:
 * https://github.com/parkerhoyes/bolos-user-interface
 *
 * Copyright (C) 2016 Parker Hoyes <contact@parkerhoyes.com>
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

typedef enum {
	BUI_FONT_NONE = -1, // Not a real font
	BUI_FONT_COMIC_SANS_MS_20,
	BUI_FONT_LUCIDA_CONSOLE_8,
	BUI_FONT_LUCIDA_CONSOLE_15,
	BUI_FONT_OPEN_SANS_BOLD_13,
	BUI_FONT_OPEN_SANS_BOLD_21,
	BUI_FONT_OPEN_SANS_EXTRABOLD_11,
	BUI_FONT_OPEN_SANS_LIGHT_13,
	BUI_FONT_OPEN_SANS_LIGHT_14,
	BUI_FONT_OPEN_SANS_LIGHT_16,
	BUI_FONT_OPEN_SANS_LIGHT_20,
	BUI_FONT_OPEN_SANS_LIGHT_21,
	BUI_FONT_OPEN_SANS_LIGHT_32,
	BUI_FONT_OPEN_SANS_REGULAR_11,
	BUI_FONT_OPEN_SANS_SEMIBOLD_18,
	BUI_FONT_LAST // Not a real font
} bui_font_id_e;

// NOTE: Despite the font's range, they never include characters in the range 0x80 to 0x9F (both inclusive)
typedef struct {
	uint8_t char_height;
	uint8_t baseline_height;
	uint8_t char_kerning;
	uint8_t first_char; // Character code of the first character with a bitmap in this font
	uint8_t last_char; // Character code of the last character with a bitmap in this font
} bui_font_info_t;

/*
 * Get the info for a particular font given its ID.
 *
 * Args:
 *     font_id: the ID of the font
 * Returns:
 *     a pointer to a structure containing info about the specified font, or NULL if font_id is invalid
 */
const bui_font_info_t* bui_font_get_font_info(bui_font_id_e font_id);

/*
 * Get the width of a given character in the specified font.
 *
 * Args:
 *     font_id: the ID of the font
 *     ch: the character code
 * Returns:
 *     the width of the given character, in pixels; if font_id is invalid, 0 is returned
 */
uint8_t bui_font_get_char_width(bui_font_id_e font_id, char ch);

/*
 * Get the pointer to the bitmap for a character in a particular font.
 *
 * Args:
 *     font_id: the ID of the font
 *     ch: the character code
 *     w_dest: a pointer to an int in which the width of the character will be stored; if font_id is invalid, this is
 *             set to 0; if this is NULL, it is not accessed
 * Returns:
 *     the pointer to the bitmap for the specified character in the specified font or NULL if font_id is invalid
 */
const unsigned char* bui_font_get_char_bitmap(bui_font_id_e font_id, char ch, int *w_dest);

/*
* Draw a character in the specified font onto the bottom display buffer. Any part of the character out of bounds of the
* display will not be drawn. The coordinates provided determine the position of the text anchor. The actual bounds the
* text is drawn within are determined by the anchor and the alignment. The alignment determines where, in the text's
* bounds, the anchor is located. For example, an alignment of BUI_RIGHT_TOP will place the anchor at the top-most,
* right-most position of the text's boundaries. Note that for purposes of alignment, the character's boundaries are
* considered to extend from the font's baseline to the ascender height.
*
* Args:
*     buffer: the display buffer onto which to draw the character
*     ch: the character code of the character to be drawn
*     x: the x-coordinate of the text anchor; must be >= -32,768 and <= 32,767
*     y: the y-coordinate of the text anchor; must be >= -32,768 and <= 32,767
*     alignment: the position of the anchor within the text boundaries
*     font_id: the ID of the font to be used to render the character
*/
void bui_font_draw_char(bui_bitmap_128x32_t *buffer, char ch, int x, int y, bui_dir_e alignment, bui_font_id_e font_id);

/*
* Draw a string in the specified font onto the bottom display buffer. Any part of the string out of bounds of the
* buffer will not be drawn (the string will not wrap). The coordinates provided determine the position of the text
* anchor. The actual bounds the text is drawn within are determined by the anchor and the alignment. The alignment
* determines where, in the text's bounds, the anchor is located. For example, an alignment of BUI_RIGHT_TOP will place
* the anchor at the top-most, right-most position of the text's boundaries. Note that for purposes of alignment, the
* character's boundaries are considered to extend from the font's baseline to the ascender height.
*
* Args:
*     buffer: the display buffer onto which to draw the string
*     str: the null-terminated string to be drawn; may not be NULL
*     x: the x-coordinate of the text anchor; must be >= -32,768 and <= 32,767
*     y: the y-coordinate of the text anchor; must be >= -32,768 and <= 32,767
*     alignment: the position of the anchor within the text boundaries
*     font_id: the ID of the font to be used to render the string
*/
void bui_font_draw_string(bui_bitmap_128x32_t *buffer, const char *str, int x, int y, bui_dir_e alignment,
		bui_font_id_e font_id);

#endif
