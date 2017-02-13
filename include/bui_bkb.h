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

#ifndef BUI_BKB_H_
#define BUI_BKB_H_

#include <stdbool.h>
#include <stdint.h>

#include "bui.h"

#define BUI_BKB_OPTION_NUMERICS    '\x01'
#define BUI_BKB_OPTION_SYMBOLS     '\x02'
#define BUI_BKB_OPTION_TOGGLE_CASE '\x03'

// NOTE: The definition of this struct is considered internal. It may be changed between versions without warning, and
// the struct's data should never be modified by code external to this module (bui_bkb).
typedef struct {
	char layout[35]; // The buffer that stores the possible "keys" the user may choose from (the order matters)
	uint8_t layout_size; // The number of "keys" in layout
	char *type_buff; // The buffer that stores the characters that the user has typed
	uint8_t type_buff_size; // The number of characters in type_buff
	uint8_t type_buff_cap; // The maximum capacity of type_buff (in bytes)
	uint8_t bits_typed; // The sequence of "bits" inputted by the user, starting at the MSB (0 is left, 1 is right)
	uint8_t bits_typed_size; // The number of "bits" inputted by the user (the number of left / right choices)
	char option; // Specifies the active sub-menu for a particular set of characters; '\0' is none
	uint8_t keys_tick; // The animation ticker for key animations, in 10 ms increments; 255 is no animations,
	                   // KEYS_ANIMATION_LEN is done
	uint8_t cursor_tick; // The animation ticker for the cursor blink animation, in 10 ms increments
} bui_bkb_bkb_t;

extern const char bui_bkb_layout_alphabetic[26];
extern const char bui_bkb_layout_numeric[10];
extern const char bui_bkb_layout_alphanumeric[27];
extern const char bui_bkb_layout_hexadecimal[16];
extern const char bui_bkb_layout_symbols[32];
extern const char bui_bkb_layout_standard[30];

/*
 * Initialize a preallocated keyboard with the specified parameters.
 *
 * Args:
 *     bkb: the preallocated keyboard
 *     layout: a string containing all of the characters to be displayed on the keyboard, in order; all characters must
 *            be displayable in the font bui_font_lucida_console_8; the only whitespace character allowed is a space;
 *            the character OPTION_NUMERICS is a special character that will provide an option to type in numeric digits
 *            from 0 to 9; the character OPTION_SYMBOLS is a special character that will provide an option to type in
 *            symbols; the character OPTION_TOGGLE_CASE is a special character that will toggle the case of all
 *            alphabetic characters available for the user to choose from; if empty, this may be NULL
 *     layout_size: the length of the layout string; must be <= 35
 *     type_buff: the buffer used to contain the text in the keyboard's textbox, encoded in ASCII with no
 *            null-terminator; all characters must be displayable in the font bui_font_lucida_console_8; the only
 *            whitespace character allowed is a space
 *     type_buff_size: the number of characters currently in type_buff
 *     type_buff_cap: the maximum capacity of type_buff; also the maximum number of characters the keyboard will allow
 *                    the user to type in; must be >= 1 and <= 255
 *     animations: true if the keyboard is to be animated, false otherwise
 */
void bui_bkb_init(bui_bkb_bkb_t *bkb, const char *layout, uint8_t layout_size, char *type_buff, uint8_t type_buff_size,
		uint8_t type_buff_cap, bool animations);

/*
 * Indicate that the user has chosen a letter on the specified side of the screen for a specified keyboard.
 *
 * Args:
 *     bkb: the keyboard
 *     side: must be either BUI_DIR_LEFT or BUI_DIR_RIGHT
 * Returns:
 *     if a character was chosen, that character is returned; if no character was chosen, 0x1FF is returned; if
 *     backspace is chosen, 0x2FF is returned; if a special option character is chosen then 0x1FF is returned
 */
int bui_bkb_choose(bui_bkb_bkb_t *bkb, bui_dir_e side);

/*
 * Indicate that a single animation tick has passed. If animations for the specified keyboard are disabled, this
 * function should never be called. If they are enabled, it is recommended that this function is called at a rate of 25
 * Hz (and preferrably the keyboard should be drawn at the same rate), passing 4 as the value of elapsed.
 *
 * Args:
 *     bkb: the keyboard, with animations enabled
 *     elapsed: the time elapsed since the last call to this function, in 10 ms increments (value of 4 recommended)
 * Returns:
 *     true if the tick may have resulted in a change in the way the keyboard is drawn, false otherwise
 */
bool bui_bkb_tick(bui_bkb_bkb_t *bkb, uint32_t elapsed);

/*
 * Draw the specified keyboard onto the specified display buffer.
 *
 * Args:
 *     bkb: the keyboard to be drawn
 *     buffer: the display buffer onto which the keyboard is to be drawn
 */
void bui_bkb_draw(const bui_bkb_bkb_t *bkb, bui_bitmap_128x32_t *buffer);

/*
 *
 *
 * Args:
 *     bkb: the keyboard
 *     type_buff: the buffer used to contain the text in the keyboard's textbox, encoded in ASCII with no
 *            null-terminator; all characters must be displayable in the font bui_font_lucida_console_8; the only
 *            whitespace character allowed is a space
 *     type_buff_size: the number of characters currently in type_buff
 *     type_buff_cap: the maximum capacity of type_buff; also the maximum number of characters the keyboard will allow
 *                    the user to type in; must be >= 1 and <= 255
 */
void bui_bkb_set_type_buff(bui_bkb_bkb_t *bkb, char *type_buff, uint8_t type_buff_size, uint8_t type_buff_cap);

/*
 * Get the current size of the type buffer (the number of characters currently typed into the keyboard's textbox).
 *
 * Args:
 *     bkb: the keyboard
 * Returns:
 *     the current size of the type buffer
 */
uint8_t bui_bkb_get_type_buff_size(const bui_bkb_bkb_t *bkb);

#endif
