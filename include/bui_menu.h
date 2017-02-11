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

typedef struct bui_menu_menu_t bui_menu_menu_t;

/*
 * Get the height, in pixels, of a menu element (the width is assumed to be 128).
 *
 * Args:
 *     menu: the menu
 *     i: the index of the menu element
 * Returns:
 *     the height, in pixels, of the menu element; must be >= 1 and <= 32
 */
typedef uint8_t (*bui_menu_elem_size_callback_t)(const bui_menu_menu_t *menu, uint8_t i);

/*
 * Draw a menu element.
 *
 * Args:
 *     menu: the menu
 *     i: the index of the menu element
 *     buffer: the buffer onto which the element is to be drawn
 *     y: the y-coordinate of the top of the destination in the buffer onto which the element is to be drawn
 */
typedef void (*bui_menu_elem_draw_callback_t)(const bui_menu_menu_t *menu, uint8_t i, bui_bitmap_128x32_t *buffer,
		int y);

// NOTE: The definition of all fields in this struct except for the callbacks are considered internal. All other fields
// may be changed between versions without warning, and all fields other than the callback fields should never be
// accessed or modified by code external to this module (bui_menu).
struct bui_menu_menu_t {
	// The number of elements in the menu
	uint8_t count;
	// The index of the focused element
	uint8_t focus : 8;
	// True if animations are enabled, false otherwise
	bool animations : 1;
	// The number of milliseconds elapsed but not processed by the animation algorithm
	uint8_t elapsed : 5;
	// The current y-coordinate of the viewport, relative to the target y-coordinate (if animations are enabled)
	int32_t scroll_pos : 18;
	// The callback used to get the size of menu elements
	bui_menu_elem_size_callback_t elem_size_callback;
	// The callback used to draw menu elements
	bui_menu_elem_draw_callback_t elem_draw_callback;
};

/*
 * Initialize a preallocated menu with the specified parameters. The callback fields in the menu must have already been
 * set before calling this function (the fields menu->elem_size_callback and menu->elem_draw_callback).
 *
 * Args:
 *     menu: the preallocated menu, with menu->elem_size_callback and menu->elem_draw_callback already set
 *     count: the number of elements initially in the menu
 *     focus: the index of the element to be initially focused in the menu; must be < the number of elements in the
 *            menu, if there are any elements in the menu
 *     elem_size_callback: the callback used to get the size of the elements in the menu; the values produced by this
 *                         function must be consistent unless bui_menu_change_elems(...) is called
 *     elem_draw_callback: the callback to be used to draw the elements in the menu
 *     animations: true if scrolling the menu should be animated, false otherwise
 */
void bui_menu_init(bui_menu_menu_t *menu, uint8_t count, uint8_t focus, bool animations);

/*
 * Indicate that either the number of elements in the menu has changed, or the size of any of the elements in the menu
 * has changed. This function must be called if either of these properties are changed. Calling this function resets
 * the scroll animations, as if by calling bui_menu_animate(menu, 0xFFFFFFFF).
 *
 * Args:
 *     menu: the menu
 *     count: the new number of elements in the menu (must be <= 255), or 0xFFFF if the number of elements is unchanged
 */
void bui_menu_change_elems(bui_menu_menu_t *menu, uint16_t count);

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
 *     the index of the currently focused element; if there are no elements, 0xFFFF is returned
 */
uint16_t bui_menu_get_focused(const bui_menu_menu_t *menu);

#endif
