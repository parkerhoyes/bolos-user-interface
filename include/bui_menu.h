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

#ifndef BUI_MENU_H_
#define BUI_MENU_H_

#include <stdbool.h>
#include <stdint.h>

#include "bui.h"

typedef struct bui_menu_menu_t_ bui_menu_menu_t;

typedef struct {
	// A big-endian sequence of bits storing the sizes of corresponding elements; 0 is regular size (128x12) and 1 is
	// full size (128x32)
	uint32_t sizes;
	// The number of elements in the menu; must be <= 32
	uint8_t count;
} bui_menu_elem_data_t;

typedef void (*bui_menu_elem_draw_callback_t)(const bui_menu_menu_t *menu, uint8_t i, bui_bitmap_128x32_t *buffer,
		int y);

// NOTE: The definition of this struct is considered internal. It may be changed between versions without warning, and
// the struct's data should never be accessed or modified by code external to this module (bui_menu).
struct bui_menu_menu_t_ {
	// The size of each element and the number of elements in the menu
	bui_menu_elem_data_t elem_data;
	// The index of the focused element
	uint8_t focus;
	// The callback used to draw menu elements
	bui_menu_elem_draw_callback_t elem_draw_callback;
	// The current y-coordinate of the viewport, relative to the target y-coordinate (if animations are enabled)
	int16_t scroll_pos;
	// True if animations are enabled, false otherwise
	bool animations : 1;
	// The number of milliseconds elapsed but not processed by the animation algorithm
	uint8_t elapsed : 7;
};

/*
 * Initialize a preallocated menu with the specified parameters.
 *
 * Args:
 *     menu: the preallocated menu
 *     elem_data: the number of elements and the size of each element in the menu
 *     focus: the index of the element to be initially focused in the menu; must be < elem_data.count if elem_data.count
 *            != 0
 *     elem_draw_callback: the callback to be used to draw the elements in the menu
 *     animations: true if scrolling the menu should be animated, false otherwise
 */
void bui_menu_init(bui_menu_menu_t *menu, bui_menu_elem_data_t elem_data, uint8_t focus,
		bui_menu_elem_draw_callback_t elem_draw_callback, bool animations);

/*
 * Get the number of elements and the size of each element in the menu.
 *
 * Args:
 *     menu: the menu
 * Returns:
 *     the number of elements and the size of each element in the menu
 */
bui_menu_elem_data_t bui_menu_get_elem_data(const bui_menu_menu_t *menu);

/*
 * Set the number of elements and the size of each element in the menu. If animations are enabled, the progress of the
 * animations are reset by calling this function.
 *
 * Args:
 *     menu: the menu
 *     elem_data: the number of elements and the size of each element in the menu
 */
void bui_menu_set_elem_data(bui_menu_menu_t *menu, bui_menu_elem_data_t elem_data);

/*
 * Modify the menu's element data such that an element of the specified size is inserted at the specified index, and all
 * subsequent elements are shifted over to greater indices; the size of the menu is increased by 1. The size of the menu
 * must initially be <= 31.
 *
 * Args:
 *     menu: the menu
 *     i: the index at which the element is to be inserted; must be <= the size of the menu
 *     size: the size of the inserted element; false is regular size (128x12) and true is full size (128x32)
 */
void bui_menu_insert_elem(bui_menu_menu_t *menu, uint8_t i, bool size);

/*
 * Modify the menu's element data such that the element at the specified index is removed and all subsequent elements
 * are shifted to lesser indices to fill the gap; the size of the menu is decreased by 1. The size of the menu must
 * initially be >= 1.
 *
 * Args:
 *     menu: the menu
 *     i: the index of the element to be removed; must be < the size of the menu
 */
void bui_menu_remove_elem(bui_menu_menu_t *menu, uint8_t i);

/*
 * Scroll the menu by one element in either the up or down direction. If the menu cannot scroll any further in the
 * specified direction, this function does not modify the menu.
 *
 * Args:
 *     menu: the menu
 *     dir: true to scroll up, false to scroll down
 * Returns:
 *     true if the menu was modified as a result of this function call, false otherwise
 */
bool bui_menu_scroll(bui_menu_menu_t *menu, bool dir);

/*
 * Progress the menu's animations for the specified amount of time.
 *
 * Args:
 *     menu: the menu; the menu must have animations enabled
 *     elapsed: the number of milliseconds for which the menu's animations should be progressed; if this is 0, the menu
 *              is not modified and if this is 0xFFFFFFFF, the menu's animations are completed
 * Returns:
 *     true if the appearance of the menu may have been modified by this function, false otherwise
 */
bool bui_menu_animate(bui_menu_menu_t *menu, uint32_t elapsed);

/*
 * Draw the menu onto the specified display buffer.
 *
 * Args:
 *     menu: the menu
 *     buffer: the display buffer onto which the menu is to be drawn
 */
void bui_menu_draw(const bui_menu_menu_t *menu, bui_bitmap_128x32_t *buffer);

/*
 * Get the index of the currently focused element.
 *
 * Args:
 *     menu: the menu
 * Returns:
 *     the index of the currently focused element; if there are no elements, 0xFF is returned
 */
uint8_t bui_menu_get_focused(const bui_menu_menu_t *menu);

#endif
