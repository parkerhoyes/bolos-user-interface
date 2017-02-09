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

static inline bool bui_menu_get_elem_size(const bui_menu_menu_t *menu, uint8_t i) {
	return ((menu->elem_data.sizes << i) & (uint32_t) 0x80000000) != 0;
}

void bui_menu_init(bui_menu_menu_t *menu, bui_menu_elem_data_t elem_data, uint8_t focus,
		bui_menu_elem_draw_callback_t elem_draw_callback, bool animations) {
	menu->elem_data = elem_data;
	menu->focus = focus;
	menu->elem_draw_callback = elem_draw_callback;
	menu->animations = animations;
	if (animations) {
		menu->scroll_pos = 0;
		menu->elapsed = 0;
	}
}

bui_menu_elem_data_t bui_menu_get_elem_data(const bui_menu_menu_t *menu) {
	return menu->elem_data;
}

void bui_menu_set_elem_data(bui_menu_menu_t *menu, bui_menu_elem_data_t elem_data) {
	menu->elem_data = elem_data;
	if (menu->focus >= menu->elem_data.count) {
		// Possible integer underflow here is anticipated and acceptable; focus is ignored if count is 0.
		menu->focus = menu->elem_data.count - 1;
	}
	// Reset animations (only has an effect if animations are enabled)
	menu->scroll_pos = 0;
	menu->elapsed = 0;
}

void bui_menu_insert_elem(bui_menu_menu_t *menu, uint8_t i, bool size) {
	if (i == 0) {
		menu->elem_data.sizes >>= 1;
		if (size)
			menu->elem_data.sizes |= 0x80000000;
		goto done;
	}
	if (i == menu->elem_data.count) {
		if (size)
			menu->elem_data.sizes |= 0x80000000 >> i;
		else
			menu->elem_data.sizes &= ~(0x80000000 >> i);
		goto done;
	}
	uint32_t sizes = menu->elem_data.sizes;
	sizes &= ~(0xFFFFFFFF >> i);
	if (size)
		sizes |= 0x80000000 >> i;
	sizes |= (menu->elem_data.sizes & (0xFFFFFFFF >> i)) >> 1;
	menu->elem_data.sizes = sizes;
done:
	menu->elem_data.count += 1;
}

void bui_menu_remove_elem(bui_menu_menu_t *menu, uint8_t i) {
	if (i == 0) {
		menu->elem_data.sizes <<= 1;
		goto done;
	}
	if (i == menu->elem_data.count - 1)
		goto done;
	uint32_t sizes = menu->elem_data.sizes;
	sizes &= ~(0xFFFFFFFF >> i);
	sizes |= (menu->elem_data.sizes & (0xFFFFFFFF >> (i + 1))) << 1;
	menu->elem_data.sizes = sizes;
done:
	menu->elem_data.count -= 1;
}

bool bui_menu_scroll(bui_menu_menu_t *menu, bool dir) {
	if (menu->elem_data.count == 0)
		return false;
	if (dir && menu->focus != 0) {
		menu->focus -= 1;
		if (menu->animations) {
			menu->scroll_pos += bui_menu_get_elem_size(menu, menu->focus) ? 32 : 12;
		}
		return true;
	}
	if (!dir && menu->focus + 1 != menu->elem_data.count) {
		menu->focus += 1;
		if (menu->animations)
			menu->scroll_pos -= bui_menu_get_elem_size(menu, menu->focus) ? 32 : 12;
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
	uint8_t elem_count = menu->elem_data.count;
	if (elem_count == 0)
		return;
	int8_t focus = menu->focus;

	// Draw arrows
	if (focus != 0) {
		bui_draw_bitmap(buffer, bui_bitmap_up_bitmap, bui_bitmap_up_w, 0, 0, 3, 14, bui_bitmap_up_w,
				bui_bitmap_up_h);
	}
	if (focus + 1 != elem_count) {
		bui_draw_bitmap(buffer, bui_bitmap_down_bitmap, bui_bitmap_down_w, 0, 0, 118, 14, bui_bitmap_down_w,
				bui_bitmap_down_h);
	}

	if (!menu->animations) {
		bool focus_size = bui_menu_get_elem_size(menu, focus);
		if (focus_size) {
			(*menu->elem_draw_callback)(menu, focus, buffer, 0);
		} else {
			(*menu->elem_draw_callback)(menu, focus, buffer, 10);
			if (focus != 0) {
				bool above_size = bui_menu_get_elem_size(menu, focus - 1);
				(*menu->elem_draw_callback)(menu, focus - 1, buffer, above_size ? -22 : -2);
			}
			if (focus + 1 != elem_count) {
				(*menu->elem_draw_callback)(menu, focus + 1, buffer, 22);
			}
		}
	} else {
		// Adjust focus and scroll_pos to point to the element nearest to the viewport
		int16_t scroll_pos = menu->scroll_pos;
		while (focus != 0 && focus + 1 != elem_count) {
			int8_t size = bui_menu_get_elem_size(menu, focus) ? 32 : 12;
			if (scroll_pos > size) {
				scroll_pos -= size;
				focus += 1;
			} else if (scroll_pos < -size) {
				scroll_pos += size;
				focus -= 1;
			} else {
				break;
			}
		}

		// Draw elements
		uint8_t focus_size = bui_menu_get_elem_size(menu, focus) ? 32 : 12;
		int16_t focus_pos = (32 / 2) - (focus_size / 2) - (focus_size % 2) - scroll_pos;
		// Draw focused element
		(*menu->elem_draw_callback)(menu, focus, buffer, focus_pos);
		// Draw the visible elements below the focused element
		{
			uint8_t target = focus + 1;
			int16_t target_pos = focus_pos + focus_size;
			while (target_pos < 32 && target != elem_count) {
				(*menu->elem_draw_callback)(menu, target, buffer, target_pos);
				target_pos += bui_menu_get_elem_size(menu, target) ? 32 : 12;
				target += 1;
			}
		}
		// Draw the visible elements above the focused element
		{
			uint8_t target = focus;
			int16_t target_bottom = focus_pos;
			while (target_bottom > 0 && target != 0) {
				target -= 1;
				target_bottom -= bui_menu_get_elem_size(menu, target) ? 32 : 12;
				(*menu->elem_draw_callback)(menu, target, buffer, target_bottom);
			}
		}
	}
}

uint8_t bui_menu_get_focused(const bui_menu_menu_t *menu) {
	if (menu->elem_data.count != 0)
		return menu->focus;
	return 0xFF;
}
