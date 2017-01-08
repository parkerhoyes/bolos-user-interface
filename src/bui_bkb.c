/*
 * License for the BOLOS User Interface project, originally found here:
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

#include "bui_bkb.h"

#include <stdbool.h>
#include <stdint.h>

#include "os.h"

#include "bui.h"

#define CEIL_DIV(x, y) (1 + (((x) - 1) / (y)))
#define NTH_BIT(n, i) (((n) >> (7 - (i))) & 1) // Only to be used with uint8_t

static const unsigned char bitmap_toggle_case_bitmap[] = {
	0x03, 0xA2, 0x38, 0xE2, 0x2E,
};
static const uint8_t bitmap_toggle_case_w = 5;
static const uint8_t bitmap_toggle_case_h = 8;

const char bui_bkb_layout_alphabetic[26] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
	'X', 'Y', 'Z',
};
const char bui_bkb_layout_numeric[10] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
};
const char bui_bkb_layout_alphanumeric[27] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
	'X', 'Y', 'Z', BUI_BKB_OPTION_NUMERICS,
};
const char bui_bkb_layout_hexadecimal[16] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
};
const char bui_bkb_layout_symbols[32] = {
	'!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/', ':', ';', '<', '=', '>', '?', '@', '[',
	'\\', ']', '^', '_', '`', '{', '|', '}', '~',
};
const char bui_bkb_layout_standard[30] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
	'X', 'Y', 'Z', ' ', BUI_BKB_OPTION_NUMERICS, BUI_BKB_OPTION_SYMBOLS, BUI_BKB_OPTION_TOGGLE_CASE,
};

/*
 * Toggle the case of all characters in the specified string. All non-alphabetic characters in the string will not be
 * modified.
 *
 * Args:
 *     str: the string
 *     n: the length of the string, not including the null terminator if there is one
 */
static void bui_bkb_toggle_case(char *str, uint8_t n) {
	char *end = str + n;
	for (; str != end; str++) {
		if (*str >= 'A' && *str <= 'Z')
			*str += 'a' - 'A';
		else if (*str >= 'a' && *str <= 'z')
			*str -= 'a' - 'A';
	}
}

/*
 * Draw a key onto the keyboard at the specified position.
 *
 * Args:
 *     buffer: the display buffer
 *     key: the key to be drawn; may be a character displayable in BUI_FONT_LUCIDA_CONSOLE_8, or an option key; the only
 *          whitespace character allowed is a space, which is drawn as an underscore
 *     x: the x-coordinate of the top-left corner of the destination
 *     y: the y-coordinate of the top-left corner of the destination
 */
static void bui_bkb_draw_key(bui_bitmap_128x32_t *buffer, char key, int x, int y) {
	switch (key) {
	case BUI_BKB_OPTION_NUMERICS:
		key = '#';
		break;
	case BUI_BKB_OPTION_SYMBOLS:
		key = '@';
		break;
	case BUI_BKB_OPTION_TOGGLE_CASE:
		bui_draw_bitmap(buffer, bitmap_toggle_case_bitmap, bitmap_toggle_case_w, 0, 0, x, y, bitmap_toggle_case_w,
				bitmap_toggle_case_h);
		return;
	case ' ':
		key = '_';
		break;
	}
	bui_draw_char(buffer, key, x, y, BUI_DIR_LEFT_TOP, BUI_FONT_LUCIDA_CONSOLE_8);
}

void bui_bkb_init(bui_bkb_bkb_t *bkb, const char *layout, unsigned int layout_size, const char *typed,
		unsigned int typed_size) {
	if (layout_size != 0)
		os_memcpy(bkb->chars, layout, layout_size);
	bkb->chars_size = layout_size;
	if (typed_size != 0)
		os_memcpy(bkb->type_buff, typed, typed_size);
	bkb->type_buff_size = typed_size;
	bkb->bits_typed = 0;
	bkb->bits_typed_size = 0;
	bkb->option = '\0';
}

int bui_bkb_choose(bui_bkb_bkb_t *bkb, bui_dir_e side) {
	// Handle full type buffer
	if (bkb->type_buff_size == 19) {
		if (side == BUI_DIR_LEFT) { // If backspace key was chosen
			bkb->type_buff_size -= 1;
			return 0x2FF;
		}
		return 0x1FF; // No character was chosen
	}

	// Get currently active layout
	const char *chars;
	uint8_t chars_size;
	switch (bkb->option) {
	case '\0':
		chars = bkb->chars;
		chars_size = bkb->chars_size;
		break;
	case BUI_BKB_OPTION_NUMERICS:
		chars = bui_bkb_layout_numeric;
		chars_size = sizeof(bui_bkb_layout_numeric);
		break;
	case BUI_BKB_OPTION_SYMBOLS:
		chars = bui_bkb_layout_symbols;
		chars_size = sizeof(bui_bkb_layout_symbols);
		break;
	}

	// Add choice to sequence
	if (side == BUI_DIR_RIGHT)
		bkb->bits_typed |= 0x80 >> bkb->bits_typed_size;
	bkb->bits_typed_size += 1;

	// Find chosen character
	uint8_t charsn = chars_size;
	if (bkb->type_buff_size != 0 && bkb->option == '\0') // Handle backspace key, if applicable
		charsn += 1;
	uint8_t charsi = 0;
	for (uint8_t i = 0; i < bkb->bits_typed_size; i++) {
		if (NTH_BIT(bkb->bits_typed, i) == 0) {
			charsn = CEIL_DIV(charsn, 2);
		} else {
			charsi += CEIL_DIV(charsn, 2);
			charsn /= 2;
		}
	}

	// If a single key has been chosen, apply choice
	if (charsn == 1) {
		bkb->bits_typed = 0;
		bkb->bits_typed_size = 0;
		if (charsi == chars_size) { // If backspace key was chosen
			bkb->type_buff_size -= 1;
			return 0x2FF;
		}
		char ch = chars[charsi];
		switch (ch) {
		case BUI_BKB_OPTION_NUMERICS:
			bkb->option = BUI_BKB_OPTION_NUMERICS;
			return 0x1FF; // No character was chosen
		case BUI_BKB_OPTION_SYMBOLS:
			bkb->option = BUI_BKB_OPTION_SYMBOLS;
			return 0x1FF; // No character was chosen
		case BUI_BKB_OPTION_TOGGLE_CASE:
			bui_bkb_toggle_case(bkb->chars, bkb->chars_size);
			bkb->option = '\0';
			return 0x1FF; // No character was chosen
		default:
			bkb->type_buff[bkb->type_buff_size++] = ch;
			bkb->option = '\0';
			return ch;
		}
	}

	return 0x1FF; // No character was chosen
}

void bui_bkb_draw(const bui_bkb_bkb_t *bkb, bui_bitmap_128x32_t *buffer) {
	// Draw textbox border
	bui_fill_rect(buffer, 1 - 1, 20 - 1, 126, 1, true); // These coords aren't right, are they?
	bui_set_pixel(buffer, 1, 21, true);
	bui_set_pixel(buffer, 126, 21, true);
	bui_fill_rect(buffer, 1 - 1, 31 - 1, 126, 1, true); // These coords aren't right, are they?
	bui_set_pixel(buffer, 1, 30, true);
	bui_set_pixel(buffer, 126, 30, true);

	// Draw textbox contents
	for (uint8_t i = 0; i < bkb->type_buff_size; i++) {
		bui_draw_char(buffer, bkb->type_buff[i], i * 6 + 3, 22, BUI_DIR_LEFT_TOP, BUI_FONT_LUCIDA_CONSOLE_8);
	}
	bui_fill_rect(buffer, bkb->type_buff_size * 6 + 3, 28, 5, 1, true);

	// Draw center icons
	bui_draw_bitmap(buffer, bui_bitmap_left_bitmap, bui_bitmap_left_w, 0, 0, 58, 5, bui_bitmap_left_w,
			bui_bitmap_left_h);
	bui_draw_bitmap(buffer, bui_bitmap_right_bitmap, bui_bitmap_right_w, 0, 0, 66, 5, bui_bitmap_right_w,
			bui_bitmap_right_h);

	// Draw keyboard "keys"
	if (bkb->type_buff_size != 19) { // If the textbox is not full
		// Get currently active layout
		const char *chars;
		uint8_t chars_size;
		switch (bkb->option) {
		case '\0':
			chars = bkb->chars;
			chars_size = bkb->chars_size;
			break;
		case BUI_BKB_OPTION_NUMERICS:
			chars = bui_bkb_layout_numeric;
			chars_size = sizeof(bui_bkb_layout_numeric);
			break;
		case BUI_BKB_OPTION_SYMBOLS:
			chars = bui_bkb_layout_symbols;
			chars_size = sizeof(bui_bkb_layout_symbols);
			break;
		}

		// Find currently displayed sections
		uint8_t charsi = 0;
		uint8_t charsn = chars_size;
		if (bkb->type_buff_size != 0 && bkb->option == '\0') // Handle backspace key, if applicable
			charsn += 1;
		for (uint8_t i = 0; i < bkb->bits_typed_size; i++) {
			if (NTH_BIT(bkb->bits_typed, i) == 0) {
				charsn = CEIL_DIV(charsn, 2);
			} else {
				charsi += CEIL_DIV(charsn, 2);
				charsn /= 2;
			}
		}

		// Draw left side
		uint8_t lefti = charsi;
		uint8_t leftn = CEIL_DIV(charsn, 2);
		for (uint8_t i = 0; i < leftn; i++) {
			bui_bkb_draw_key(buffer, chars[lefti + i], 1 + 6 * (i % 9), i < 9 ? 0 : 9);
		}

		// Draw right side
		uint8_t righti = lefti + leftn;
		uint8_t rightn = charsn / 2;
		for (uint8_t i = 0; i < rightn; i++) {
			if (righti + i == chars_size) { // Draw backspace key, if applicable
				bui_draw_bitmap(buffer, bui_bitmap_left_filled_bitmap, bui_bitmap_left_filled_w, 0, 0, 75 + 6 * (i % 9),
						i < 9 ? 0 : 9, bui_bitmap_left_filled_w, bui_bitmap_left_filled_h);
				continue;
			}
			bui_bkb_draw_key(buffer, chars[righti + i], 74 + 6 * (i % 9), i < 9 ? 0 : 9);
		}
	} else {
		// Draw backspace key
		bui_draw_bitmap(buffer, bui_bitmap_left_filled_bitmap, bui_bitmap_left_filled_w, 0, 0, 1, 0,
				bui_bitmap_left_filled_w, bui_bitmap_left_filled_h);
	}
}

unsigned int bui_bkb_get_typed(const bui_bkb_bkb_t *bkb, char *dest) {
	if (bkb->type_buff_size != 0)
		os_memcpy(dest, bkb->type_buff, bkb->type_buff_size);
	return bkb->type_buff_size;
}
