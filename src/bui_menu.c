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

#include "bui_menu.h"

#include <stdbool.h>
#include <stdint.h>

#include "bui.h"

void bui_menu_init(bui_menu_menu_t *menu, uint8_t count, uint8_t focus, bool animations) {
	menu->count = count;
	menu->focus = focus;
	menu->animations = animations;
	menu->elapsed = 0;
	menu->scroll_pos = 0;
}

void bui_menu_change_elems(bui_menu_menu_t *menu, uint16_t count) {
	if (count != 0xFFFF && count != menu->count) {
		menu->count = count;
		if (menu->focus >= menu->count) {
			// Possible integer underflow here is anticipated and acceptable; focus is ignored if count is 0.
			menu->focus = menu->count - 1;
		}
	}
	// Reset animations (only has an effect if animations are enabled)
	menu->elapsed = 0;
	menu->scroll_pos = 0;
}

bool bui_menu_scroll(bui_menu_menu_t *menu, bool dir) {
	if (menu->count == 0)
		return false;
	if (dir && menu->focus != 0) {
		menu->focus -= 1;
		if (menu->animations) {
			menu->scroll_pos += menu->elem_size_callback(menu, menu->focus);
		}
		return true;
	}
	if (!dir && menu->focus + 1 != menu->count) {
		menu->focus += 1;
		if (menu->animations)
			menu->scroll_pos -= menu->elem_size_callback(menu, menu->focus);
		return true;
	}
	return false;
}

bool bui_menu_animate(bui_menu_menu_t *menu, uint32_t elapsed) {
	static const uint8_t interval = 30; // The number of ms per halving of scroll position

	if (elapsed == 0)
		return false;
	if (menu->scroll_pos == 0)
		return false;
	if (elapsed >= 10 * interval || elapsed + menu->elapsed >= 10 * interval) {
		bool changed = menu->scroll_pos != 0;
		menu->scroll_pos = 0;
		menu->elapsed = 0;
		return changed;
	}
	elapsed += menu->elapsed;
	menu->scroll_pos /= 1 << (elapsed / interval);
	menu->elapsed = elapsed % interval;
	return true;
}

void bui_menu_draw(const bui_menu_menu_t *menu, bui_bitmap_128x32_t *buffer) {
	uint8_t count = menu->count;
	if (count == 0)
		return;
	uint8_t focus = menu->focus;

	// Draw arrows
	if (focus != 0)
		bui_draw_bitmap(buffer, BUI_BITMAP_ICON_UP, 0, 0, 3, 14, BUI_BITMAP_ICON_UP.w, BUI_BITMAP_ICON_UP.h);
	if (focus + 1 != count)
		bui_draw_bitmap(buffer, BUI_BITMAP_ICON_DOWN, 0, 0, 118, 14, BUI_BITMAP_ICON_DOWN.w, BUI_BITMAP_ICON_DOWN.h);

	uint8_t focus_size;
	int32_t focus_pos;
	if (!menu->animations) {
		focus_size = menu->elem_size_callback(menu, focus);
		focus_pos = (32 / 2) - (focus_size / 2) - (focus_size % 2);
	} else {
		// Adjust focus and scroll_pos to point to the element nearest to the viewport
		int32_t scroll_pos = menu->scroll_pos;
		while (focus != 0 && focus + 1 != count) {
			uint8_t size = menu->elem_size_callback(menu, focus);
			if (scroll_pos > size) {
				scroll_pos -= size;
				focus += 1;
			} else if (-scroll_pos > size) {
				scroll_pos += size;
				focus -= 1;
			} else {
				break;
			}
		}

		focus_size = menu->elem_size_callback(menu, focus);
		focus_pos = (32 / 2) - (focus_size / 2) - (focus_size % 2) - scroll_pos;
	}

	// Draw focused element
	menu->elem_draw_callback(menu, focus, buffer, focus_pos);

	// Draw the visible elements below the focused element
	{
		uint8_t target = focus + 1;
		int32_t target_pos = focus_pos + focus_size;
		while (target_pos < 32 && target != count) {
			menu->elem_draw_callback(menu, target, buffer, target_pos);
			target_pos += menu->elem_size_callback(menu, target);
			target += 1;
		}
	}

	// Draw the visible elements above the focused element
	{
		uint8_t target = focus;
		int32_t target_bottom = focus_pos;
		while (target_bottom > 0 && target != 0) {
			target -= 1;
			target_bottom -= menu->elem_size_callback(menu, target);
			menu->elem_draw_callback(menu, target, buffer, target_bottom);
		}
	}
}

uint16_t bui_menu_get_focused(const bui_menu_menu_t *menu) {
	if (menu->count == 0)
		return 0xFFFF;
	return menu->focus;
}
