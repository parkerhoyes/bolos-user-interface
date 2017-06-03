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

#include "bui.h"

#include <stdbool.h>
#include <stdint.h>

#include "os.h"
#include "os_io_seproxyhal.h"

#include "bui_bitmaps.inc"

// The duration, in milliseconds, longer than which a button must be held for it to not be considered a "click" anymore
#ifndef BUI_BUTTON_LONG_THRESHOLD
#define BUI_BUTTON_LONG_THRESHOLD 800
#endif

#ifndef BUI_BUTTON_FAST_THRESHOLD
#define BUI_BUTTON_FAST_THRESHOLD 300
#endif

#define BUI_ABS_DIST(a, b) ((a) > (b) ? (a) - (b) : (b) - (a))

/*
 * Perform a bitwise Boolean operation between a source squence of bits and a destination sequence of bits, storing the
 * result in the destination sequence of bits. The source and destination sequences may not be overlapping. No bytes
 * that do not contain bits in either of the source or destination sequences are accessed.
 *
 * Args:
 *     src: the pointer to the byte containing the first bit in the source sequence
 *     src_o: the index of the first bit in the source sequence in its byte (0 for the most significant bit, 7 for the
 *            least); must be <= 7
 *     dest: the pointer to the byte containing the first bit in the destination sequence
 *     dest_o: the index of the first bit in the destination sequence in its byte (0 for the most significant bit, 7 for
 *             the least); must be <= 7
 *     n: the number of bits in each of the source and destination sequences
 */
typedef void (*bui_bitblit_func_t)(const uint8_t *src, uint8_t src_o, uint8_t *dest, uint8_t dest_o, uint32_t n);

static const uint32_t bui_ctx_palette[] = {
	BUI_CLR_BLACK,
	BUI_CLR_WHITE,
};

/*
 * An implementation of bui_bitblit_func_t that performs dest = src.
 */
static inline void bui_bitblit_set(const uint8_t *src, uint8_t src_o, uint8_t *dest, uint8_t dest_o, uint32_t n) {
	while (true) {
		if (n >= 8) {
			uint8_t bits = src[0] << src_o;
			if (src_o != 0)
				bits |= src[1] >> (8 - src_o);
			dest[0] &= ~(0xFF >> dest_o);
			dest[0] |= bits >> dest_o;
			if (dest_o != 0) {
				dest[1] &= 0xFF >> dest_o;
				dest[1] |= bits << (8 - dest_o);
			}
			src++;
			dest++;
			n -= 8;
		} else if (n == 0) {
			break;
		} else { // n is in [1, 7]
			uint8_t bits = src[0] << src_o;
			if (8 - src_o < n)
				bits |= src[1] >> (8 - src_o);
			bits &= ~(0xFF >> n);
			uint8_t mask = ~(0xFF >> n);
			dest[0] &= ~(mask >> dest_o);
			dest[0] |= bits >> dest_o;
			if (8 - dest_o < n) {
				dest[1] &= ~(mask << (8 - dest_o));
				dest[1] |= bits << (8 - dest_o);
			}
			break;
		}
	}
}

/*
 * An implementation of bui_bitblit_func_t that performs dest = ~src.
 */
static inline void bui_bitblit_not_set(const uint8_t *src, uint8_t src_o, uint8_t *dest, uint8_t dest_o, uint32_t n) {
	while (true) {
		if (n >= 8) {
			uint8_t bits = src[0] << src_o;
			if (src_o != 0)
				bits |= src[1] >> (8 - src_o);
			bits = ~bits;
			dest[0] &= ~(0xFF >> dest_o);
			dest[0] |= bits >> dest_o;
			if (dest_o != 0) {
				dest[1] &= 0xFF >> dest_o;
				dest[1] |= bits << (8 - dest_o);
			}
			src++;
			dest++;
			n -= 8;
		} else if (n == 0) {
			break;
		} else { // n is in [1, 7]
			uint8_t bits = src[0] << src_o;
			if (8 - src_o < n)
				bits |= src[1] >> (8 - src_o);
			bits &= ~(0xFF >> n);
			bits = ~bits;
			uint8_t mask = ~(0xFF >> n);
			dest[0] &= ~(mask >> dest_o);
			dest[0] |= bits >> dest_o;
			if (8 - dest_o < n) {
				dest[1] &= ~(mask << (8 - dest_o));
				dest[1] |= bits << (8 - dest_o);
			}
			break;
		}
	}
}

/*
 * An implementation of bui_bitblit_func_t that performs dest = dest | src.
 */
static inline void bui_bitblit_or(const uint8_t *src, uint8_t src_o, uint8_t *dest, uint8_t dest_o, uint32_t n) {
	while (true) {
		if (n >= 8) {
			uint8_t bits = src[0] << src_o;
			if (src_o != 0)
				bits |= src[1] >> (8 - src_o);
			dest[0] |= bits >> dest_o;
			if (dest_o != 0) {
				dest[1] |= bits << (8 - dest_o);
			}
			src++;
			dest++;
			n -= 8;
		} else if (n == 0) {
			break;
		} else { // n is in [1, 7]
			uint8_t bits = src[0] << src_o;
			if (8 - src_o < n)
				bits |= src[1] >> (8 - src_o);
			bits &= ~(0xFF >> n);
			dest[0] |= bits >> dest_o;
			if (8 - dest_o < n) {
				dest[1] |= bits << (8 - dest_o);
			}
			break;
		}
	}
}

/*
 * An implementation of bui_bitblit_func_t that performs dest = dest & src.
 */
static inline void bui_bitblit_and(const uint8_t *src, uint8_t src_o, uint8_t *dest, uint8_t dest_o, uint32_t n) {
	while (true) {
		if (n >= 8) {
			uint8_t bits = src[0] << src_o;
			if (src_o != 0)
				bits |= src[1] >> (8 - src_o);
			dest[0] &= bits >> dest_o;
			if (dest_o != 0) {
				dest[1] &= bits << (8 - dest_o);
			}
			src++;
			dest++;
			n -= 8;
		} else if (n == 0) {
			break;
		} else { // n is in [1, 7]
			uint8_t bits = src[0] << src_o;
			if (8 - src_o < n)
				bits |= src[1] >> (8 - src_o);
			bits &= ~(0xFF >> n);
			dest[0] &= bits >> dest_o;
			if (8 - dest_o < n) {
				dest[1] &= bits << (8 - dest_o);
			}
			break;
		}
	}
}

/*
 * An implementation of bui_bitblit_func_t that performs dest = dest | ~src.
 */
static inline void bui_bitblit_or_not(const uint8_t *src, uint8_t src_o, uint8_t *dest, uint8_t dest_o, uint32_t n) {
	while (true) {
		if (n >= 8) {
			uint8_t bits = src[0] << src_o;
			if (src_o != 0)
				bits |= src[1] >> (8 - src_o);
			dest[0] |= ~(bits >> dest_o);
			if (dest_o != 0) {
				dest[1] |= ~(bits << (8 - dest_o));
			}
			src++;
			dest++;
			n -= 8;
		} else if (n == 0) {
			break;
		} else { // n is in [1, 7]
			uint8_t bits = src[0] << src_o;
			if (8 - src_o < n)
				bits |= src[1] >> (8 - src_o);
			bits &= ~(0xFF >> n);
			dest[0] |= ~(bits >> dest_o);
			if (8 - dest_o < n) {
				dest[1] |= ~(bits << (8 - dest_o));
			}
			break;
		}
	}
}

/*
 * An implementation of bui_bitblit_func_t that performs dest = dest & ~src.
 */
static inline void bui_bitblit_and_not(const uint8_t *src, uint8_t src_o, uint8_t *dest, uint8_t dest_o, uint32_t n) {
	while (true) {
		if (n >= 8) {
			uint8_t bits = src[0] << src_o;
			if (src_o != 0)
				bits |= src[1] >> (8 - src_o);
			dest[0] &= ~(bits >> dest_o);
			if (dest_o != 0) {
				dest[1] &= ~(bits << (8 - dest_o));
			}
			src++;
			dest++;
			n -= 8;
		} else if (n == 0) {
			break;
		} else { // n is in [1, 7]
			uint8_t bits = src[0] << src_o;
			if (8 - src_o < n)
				bits |= src[1] >> (8 - src_o);
			bits &= ~(0xFF >> n);
			dest[0] &= ~(bits >> dest_o);
			if (8 - dest_o < n) {
				dest[1] &= ~(bits << (8 - dest_o));
			}
			break;
		}
	}
}

/*
 * Reverse the bytes in a byte buffer.
 *
 * Args:
 *     buffer: the byte buffer
 *     size: the number of bytes to be reversed
 */
static inline void bui_reverse_bytes(uint8_t *buffer, uint32_t size) {
	// Integer underflow on j is anticipated and acceptable
	for (uint32_t i = 0, j = size - 1; i < size / 2; i++, j--) {
		uint8_t temp = buffer[i];
		buffer[i] = buffer[j];
		buffer[j] = temp;
	}
}

/*
 * Send some data contained within the provided BUI context's display buffer to the MCU to be displayed. The data is
 * sent using a display status, and as such the MCU must be ready to receive a status when calling this function. There
 * must be additional data within the display buffer ready to be sent (bui_ctx_is_displayed(ctx) must be false).
 *
 * Args:
 *     ctx: the BUI context
 */
static inline void bui_ctx_send_display_status(bui_ctx_t *ctx) {
	uint8_t sub_w = ctx->dirty_w;
	uint8_t sub_h = ctx->dirty_h;
	// Constrain the bounds of the subrectangle of the dirty rectangle such that it fits in 64 bytes
	uint16_t size;
	if (sub_w > sub_h) {
		while ((size = ((uint16_t) sub_w * sub_h + 7) / 8) > 64)
			sub_w -= 1;
	} else {
		while ((size = ((uint16_t) sub_w * sub_h + 7) / 8) > 64)
			sub_h -= 1;
	}
	// Encode the subrectangle for transport
	uint8_t sub[64];
	os_memset(sub, 0, size);
	uint8_t xr = 128 - ctx->dirty_x - sub_w;
	uint8_t yr = 32 - ctx->dirty_y - sub_h;
	for (uint8_t i = 0; i < sub_h; i++) {
		uint16_t src_i = 128 * (yr + i) + xr;
		uint16_t dest_i = sub_w * i;
		bui_bitblit_or(&ctx->bb[src_i / 8], src_i % 8, &sub[dest_i / 8], dest_i % 8, sub_w);
	}
	bui_reverse_bytes(sub, size);
	// Display the subrectangle
	uint32_t palette[] = {0x00000000, 0x00FFFFFF};
	io_seproxyhal_display_bitmap(ctx->dirty_x, ctx->dirty_y, sub_w, sub_h, palette, 1, sub);
	// Exclude subrectangle from the dirty rectangle
	if (sub_w != ctx->dirty_w) {
		ctx->dirty_x += sub_w;
		ctx->dirty_w -= sub_w;
	} else {
		ctx->dirty_y += sub_h;
		ctx->dirty_h -= sub_h;
	}
}

/*
 * Extend the provided BUI context's dirty rectangle by the minimum amount such that it encloses the provided rectangle.
 * The provided rectangle must be entirely within the display's coordinate plane.
 *
 * Args:
 *     ctx: the BUI context
 *     x: the x-coordinate of the top-left corner of the rectangle
 *     y: the y-coordinate of the top-left corner of the rectangle
 *     w: the width of the rectangle; must be != 0
 *     h: the height of the rectangle; must be != 0
 */
static inline void bui_ctx_dirty(bui_ctx_t *ctx, uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
	uint8_t x2 = x + w;
	uint8_t y2 = y + h;
	uint8_t dx2 = ctx->dirty_x + ctx->dirty_w;
	uint8_t dy2 = ctx->dirty_y + ctx->dirty_h;
	if (x < ctx->dirty_x)
		ctx->dirty_x = x;
	if (y < ctx->dirty_y)
		ctx->dirty_y = y;
	if (dx2 < x2)
		dx2 = x2;
	if (dy2 < y2)
		dy2 = y2;
	ctx->dirty_w = dx2 - ctx->dirty_x;
	ctx->dirty_h = dy2 - ctx->dirty_y;
}

int16_t bui_palette_find(const uint32_t *palette, uint16_t size, uint32_t color) {
	for (uint16_t i = 0; i < size; i++) {
		if (palette[i] == color)
			return i;
	}
	return -1;
}

uint8_t bui_palette_find_best(const uint32_t *palette, uint16_t size, uint32_t needle) {
	if (size == 1)
		return 0;
	for (uint16_t i = 0; i < size; i++) {
		if (palette[i] == needle)
			return i;
	}
	uint8_t needle_b = needle & 0xFF;
	needle >>= 8;
	uint8_t needle_g = needle & 0xFF;
	needle >>= 8;
	uint8_t needle_r = needle & 0xFF;
	uint32_t best_dist;
	uint8_t best_index;
	for (uint8_t i = 0; i < size; i++) {
		uint32_t hay = palette[i];
		uint8_t hay_b = hay & 0xFF;
		hay >>= 8;
		uint8_t hay_g = hay & 0xFF;
		hay >>= 8;
		uint8_t hay_r = hay & 0xFF;
		uint16_t temp;
		temp = BUI_ABS_DIST(hay_r, needle_r);
		temp *= temp;
		uint32_t dist = temp;
		temp = BUI_ABS_DIST(hay_g, needle_g);
		temp *= temp;
		dist += temp;
		temp = BUI_ABS_DIST(hay_b, needle_b);
		temp *= temp;
		dist += temp;
		if (i == 0 || dist < best_dist) {
			best_dist = dist;
			best_index = i;
		}
	}
	return best_index;
}

int16_t bui_bmp_lowest_unused_index(const bui_const_bitmap_t bmp) {
	uint8_t lowest_unused = 0;
find:
	for (int16_t y = 0; y < bmp.h; y++) {
		for (int16_t x = 0; x < bmp.w; x++) {
			uint32_t index_i = (y * bmp.w + x) * bmp.bpp;
			uint8_t color_index;
			bui_bitblit_set(&bmp.bb[index_i / 8], index_i % 8, &color_index, 8 - bmp.bpp, bmp.bpp);
			if (color_index == lowest_unused) {
				lowest_unused++;
				if (lowest_unused >= 1 << bmp.bpp)
					return -1;
				goto find;
			}
		}
	}
	return lowest_unused;
}

void bui_bmp_fill(bui_bitmap_t bmp, uint32_t color) {
	if (bmp.bpp == 0)
		return;
	if (color >> 24 <= 127)
		return;
	uint8_t best_index = bui_palette_find_best(bmp.plt, 1 << bmp.bpp, color);
	if (bmp.bpp == 1) {
		uint32_t len = bmp.w * bmp.h;
		os_memset(bmp.bb, best_index == 0 ? 0x00 : 0xFF, len / 8);
		uint8_t extra = len % 8;
		if (extra == 0)
			return;
		if (best_index == 0)
			bmp.bb[len / 8] &= (uint8_t) 0xFF >> extra;
		else
			bmp.bb[len / 8] |= ~((uint8_t) 0xFF >> extra);
	} else {
		uint32_t dest_end = bmp.w * bmp.h * bmp.bpp;
		for (uint32_t dest_i = 0; dest_i < dest_end; dest_i += bmp.bpp) {
			bui_bitblit_set(&best_index, 8 - bmp.bpp, &bmp.bb[dest_i / 8], dest_i % 8, bmp.bpp);
		}
	}
}

void bui_bmp_draw_pixel(bui_bitmap_t bmp, int16_t x, int16_t y, uint32_t color) {
	if (bmp.bpp == 0)
		return;
	if (color >> 24 <= 127)
		return;
	if (x < 0 || x >= bmp.w || y < 0 || y >= bmp.h)
		return;
	color |= 0xFF000000;
	uint8_t best_index = bui_palette_find_best(bmp.plt, 1 << bmp.bpp, color);
	// Reflect coordinates
	x = bmp.w - x;
	y = bmp.h - y;
	// Set the target pixel's color index
	uint32_t dest_i = (y * bmp.w + x) * bmp.bpp;
	bui_bitblit_set(&best_index, 8 - bmp.bpp, &bmp.bb[dest_i / 8], dest_i % 8, bmp.bpp);
}

void bui_ctx_init(bui_ctx_t *ctx) {
	os_memset(ctx->bb, 0, sizeof(ctx->bb));
	ctx->dirty_x = 0;
	ctx->dirty_y = 0;
	ctx->dirty_w = 128;
	ctx->dirty_h = 32;
	ctx->ticker_interval = 40;
	ctx->event_handler = NULL;
	ctx->button_left = false;
	ctx->button_left_duration = 0;
	ctx->button_left_prev = 0;
	ctx->button_left_clicked = true; // This prevents a button clicked event from being triggered
	ctx->button_right = false;
	ctx->button_right_duration = 0;
	ctx->button_right_prev = 0;
	ctx->button_right_clicked = true; // This prevents a button clicked event from being triggered
	bui_ctx_set_ticker(ctx, 40);
}

bool bui_ctx_display(bui_ctx_t *ctx) {
	if (bui_ctx_is_displayed(ctx))
		return false;
	bui_ctx_send_display_status(ctx);
	return true;
}

uint16_t bui_ctx_get_ticker(bui_ctx_t *ctx) {
	return ctx->ticker_interval;
}

void bui_ctx_set_ticker(bui_ctx_t *ctx, uint16_t interval) {
	// Set the ticker interval to interval ms
	G_io_seproxyhal_spi_buffer[0] = SEPROXYHAL_TAG_SET_TICKER_INTERVAL;
	G_io_seproxyhal_spi_buffer[1] = 0; // Message length, high byte
	G_io_seproxyhal_spi_buffer[2] = 2; // Message length, low byte
	G_io_seproxyhal_spi_buffer[3] = (interval >> 8) & 0xFF; // Ticker interval, high byte
	G_io_seproxyhal_spi_buffer[4] = interval & 0xFF; // Ticker interval, low byte
	io_seproxyhal_spi_send(G_io_seproxyhal_spi_buffer, 5);
}

void bui_ctx_set_event_handler(bui_ctx_t *ctx, bui_event_handler_t event_handler) {
	if (event_handler != NULL)
		event_handler = (bui_event_handler_t) PIC(event_handler);
	ctx->event_handler = event_handler;
}

bool bui_ctx_seproxyhal_event(bui_ctx_t *ctx, bool allow_status) {
	bool status_sent = false;
	switch (G_io_seproxyhal_spi_buffer[0]) {
	case SEPROXYHAL_TAG_BUTTON_PUSH_EVENT: {
		unsigned int button_mask = G_io_seproxyhal_spi_buffer[3] >> 1;
		uint8_t left = 0, right = 0; // 0 = no change, 1 = pressed, 2 = released, 3 = clicked
		if ((button_mask & BUTTON_LEFT) != 0) {
			if (!ctx->button_left) {
				left = 1;
				uint16_t prev = ctx->button_left_duration;
				ctx->button_left = true;
				ctx->button_left_duration = 0;
				ctx->button_left_prev = prev < BUI_BUTTON_FAST_THRESHOLD ? 0 :
						(prev < BUI_BUTTON_LONG_THRESHOLD ? 1 : 2);
				ctx->button_left_clicked = false;
			}
		} else {
			if (ctx->button_left) {
				left = ctx->button_left_duration < BUI_BUTTON_LONG_THRESHOLD ? 3 : 2;
				uint16_t prev = ctx->button_left_duration;
				ctx->button_left = false;
				ctx->button_left_duration = 0;
				ctx->button_left_prev = prev < BUI_BUTTON_FAST_THRESHOLD ? 0 :
						(prev < BUI_BUTTON_LONG_THRESHOLD ? 1 : 2);
			}
		}
		if ((button_mask & BUTTON_RIGHT) != 0) {
			if (!ctx->button_right) {
				right = 1;
				uint16_t prev = ctx->button_right_duration;
				ctx->button_right = true;
				ctx->button_right_duration = 0;
				ctx->button_right_prev = prev < BUI_BUTTON_FAST_THRESHOLD ? 0 :
						(prev < BUI_BUTTON_LONG_THRESHOLD ? 1 : 2);
				ctx->button_right_clicked = false;
			}
		} else {
			if (ctx->button_right) {
				right = ctx->button_right_duration < BUI_BUTTON_LONG_THRESHOLD ? 3 : 2;
				uint16_t prev = ctx->button_right_duration;
				ctx->button_right = false;
				ctx->button_right_duration = 0;
				ctx->button_right_prev = prev < BUI_BUTTON_FAST_THRESHOLD ? 0 :
						(prev < BUI_BUTTON_LONG_THRESHOLD ? 1 : 2);
			}
		}
		switch (left) {
		case 1: {
			bui_event_data_button_pressed_t data = { .button = BUI_BUTTON_NANOS_LEFT };
			bui_event_t event = { .id = BUI_EVENT_BUTTON_PRESSED, .data = &data };
			bui_ctx_dispatch_event(ctx, &event);
		} break;
		case 2: {
			bui_event_data_button_released_t data = { .button = BUI_BUTTON_NANOS_LEFT,
					.prev_state = BUI_BUTTON_STATE_HELD };
			bui_event_t event = { .id = BUI_EVENT_BUTTON_RELEASED, .data = &data };
			bui_ctx_dispatch_event(ctx, &event);
		} break;
		case 3: {
			bui_event_data_button_released_t data = { .button = BUI_BUTTON_NANOS_LEFT,
					.prev_state = BUI_BUTTON_STATE_PRESSED };
			bui_event_t event = { .id = BUI_EVENT_BUTTON_RELEASED, .data = &data };
			bui_ctx_dispatch_event(ctx, &event);
		} break;
		}
		switch (right) {
		case 1: {
			bui_event_data_button_pressed_t data = { .button = BUI_BUTTON_NANOS_RIGHT };
			bui_event_t event = { .id = BUI_EVENT_BUTTON_PRESSED, .data = &data };
			bui_ctx_dispatch_event(ctx, &event);
		} break;
		case 2: {
			bui_event_data_button_released_t data = { .button = BUI_BUTTON_NANOS_RIGHT,
					.prev_state = BUI_BUTTON_STATE_HELD };
			bui_event_t event = { .id = BUI_EVENT_BUTTON_RELEASED, .data = &data };
			bui_ctx_dispatch_event(ctx, &event);
		} break;
		case 3: {
			bui_event_data_button_released_t data = { .button = BUI_BUTTON_NANOS_RIGHT,
					.prev_state = BUI_BUTTON_STATE_PRESSED };
			bui_event_t event = { .id = BUI_EVENT_BUTTON_RELEASED, .data = &data };
			bui_ctx_dispatch_event(ctx, &event);
		} break;
		}
	} break;
	case SEPROXYHAL_TAG_DISPLAY_PROCESSED_EVENT: {
		if (allow_status && !bui_ctx_is_displayed(ctx)) {
			bui_ctx_send_display_status(ctx);
			status_sent = true;
			if (bui_ctx_is_displayed(ctx)) {
				bui_event_t event = { .id = BUI_EVENT_DISPLAYED, .data = NULL };
				bui_ctx_dispatch_event(ctx, &event);
			}
		}
	} break;
	case SEPROXYHAL_TAG_TICKER_EVENT: {
		uint16_t elapsed = ctx->ticker_interval;
		bool left_held, right_held;
		{
			// Elapse time for left button
			uint16_t left_prev = ctx->button_left_duration;
			uint16_t left_curr = left_prev + elapsed;
			if (left_curr > 0x03FF)
				left_curr = 0x03FF;
			ctx->button_left_duration = left_curr;
			left_held = ctx->button_left && left_prev < BUI_BUTTON_LONG_THRESHOLD
					&& left_curr >= BUI_BUTTON_LONG_THRESHOLD;
			// Elapse time for right button
			uint16_t right_prev = ctx->button_right_duration;
			uint16_t right_curr = right_prev + elapsed;
			if (right_curr > 0x03FF)
				right_curr = 0x03FF;
			ctx->button_right_duration = right_curr;
			right_held = ctx->button_right && right_prev < BUI_BUTTON_LONG_THRESHOLD
					&& right_curr >= BUI_BUTTON_LONG_THRESHOLD;
			// Emit button clicked events, if applicable
			if (!ctx->button_left && !ctx->button_right) {
				if (!ctx->button_left_clicked && left_prev == 0 && ctx->button_left_prev < 2) {
					bui_button_id_t button = BUI_BUTTON_NANOS_LEFT;
					ctx->button_left_clicked = true;
					if (!ctx->button_right && !ctx->button_right_clicked && right_curr < BUI_BUTTON_LONG_THRESHOLD
							&& ctx->button_right_prev < 2) {
						button = BUI_BUTTON_NANOS_BOTH;
						ctx->button_right_clicked = true;
					}
					bui_event_data_button_clicked_t data = { .button = button };
					bui_event_t event = { .id = BUI_EVENT_BUTTON_CLICKED, .data = &data };
					bui_ctx_dispatch_event(ctx, &event);
				}
				if (!ctx->button_right_clicked && right_prev == 0 && ctx->button_right_prev < 2) {
					bui_button_id_t button = BUI_BUTTON_NANOS_RIGHT;
					ctx->button_right_clicked = true;
					if (!ctx->button_left && !ctx->button_left_clicked && left_curr < BUI_BUTTON_LONG_THRESHOLD
							&& ctx->button_left_prev < 2) {
						button = BUI_BUTTON_NANOS_BOTH;
						ctx->button_left_clicked = true;
					}
					bui_event_data_button_clicked_t data = { .button = button };
					bui_event_t event = { .id = BUI_EVENT_BUTTON_CLICKED, .data = &data };
					bui_ctx_dispatch_event(ctx, &event);
				}
			}
		}
		// Dispatch button held events, if applicable
		if (left_held || right_held) {
			bui_event_data_button_held_t data;
			bui_event_t event = { .id = BUI_EVENT_BUTTON_HELD, .data = &data };
			if (left_held) {
				data.button = BUI_BUTTON_NANOS_LEFT;
				bui_ctx_dispatch_event(ctx, &event);
			}
			if (right_held) {
				data.button = BUI_BUTTON_NANOS_RIGHT;
				bui_ctx_dispatch_event(ctx, &event);
			}
		}
		// Dispatch time elapsed event
		bui_event_data_time_elapsed_t data = { .elapsed = elapsed };
		bui_event_t event = { .id = BUI_EVENT_TIME_ELAPSED, .data = &data };
		bui_ctx_dispatch_event(ctx, &event);
	} break;
	}
	return status_sent;
}

bool bui_ctx_is_displayed(const bui_ctx_t *ctx) {
	return ctx->dirty_w == 0 || ctx->dirty_h == 0;
}

bui_button_state_t bui_ctx_get_button(const bui_ctx_t *ctx, bui_button_id_t button) {
	if (button == BUI_BUTTON_NANOS_LEFT) {
		if (ctx->button_left) {
			return ctx->button_left_duration < BUI_BUTTON_LONG_THRESHOLD ?
					BUI_BUTTON_STATE_PRESSED : BUI_BUTTON_STATE_HELD;
		} else {
			return BUI_BUTTON_STATE_RELEASED;
		}
	} else {
		if (ctx->button_right) {
			return ctx->button_right_duration < BUI_BUTTON_LONG_THRESHOLD ?
					BUI_BUTTON_STATE_PRESSED : BUI_BUTTON_STATE_HELD;
		} else {
			return BUI_BUTTON_STATE_RELEASED;
		}
	}
}

void bui_ctx_dispatch_event(bui_ctx_t *ctx, const bui_event_t *event) {
	if (ctx->event_handler != NULL)
		ctx->event_handler(ctx, event);
}

void bui_ctx_fill(bui_ctx_t *ctx, uint32_t color) {
	if (color >> 24 <= 127)
		return;
	uint8_t best_index = bui_palette_find_best(bui_ctx_palette, 2, color);
	os_memset(ctx->bb, best_index == 0 ? 0x00 : 0xFF, sizeof(ctx->bb));
	// Set the new dirty rectangle
	ctx->dirty_x = 0;
	ctx->dirty_y = 0;
	ctx->dirty_w = 128;
	ctx->dirty_h = 32;
}

void bui_ctx_fill_rect(bui_ctx_t *ctx, int16_t x16, int16_t y16, int16_t w16, int16_t h16, uint32_t color) {
	if (color >> 24 <= 127)
		return;
	int32_t x = x16, y = y16, w = w16, h = h16;
	if (x < 0) {
		w += x;
		x = 0;
	}
	if (y < 0) {
		h += y;
		y = 0;
	}
	if (w <= 0 || h <= 0)
		return;
	if (x >= 128 || y >= 32)
		return;
	if (x + w > 128)
		w = 128 - x;
	if (y + h > 32)
		h = 32 - y;
	// Determine best color index
	uint8_t best_index = bui_palette_find_best(bui_ctx_palette, 2, color);
	// Extend the dirty rectangle
	bui_ctx_dirty(ctx, x, y, w, h);
	// Calculate reflected coordinates
	int32_t x1r = 128 - x - w; // index of the first column in the 2D bit array to be modified
	int32_t y1r = 32 - y - h; // index of the first row in the 2D bit array to be modified
	int32_t x2r = x1r + w; // index just beyond the last column in the 2D bit array to be modified
	int32_t y2r = y1r + h; // index just beyond the last row in the 2D bit array to be modified
	// Blit the rectangle onto the display buffer
	for (int32_t i = y1r; i < y2r; i++) {
		uint8_t *row = &ctx->bb[i * 16];
		for (int32_t j = 32; j != 160; j += 32) { // Iterate through all four 32 bit sequences in the row
			if (x1r < j && x2r > j - 32) { // Only need to blit if current 32 bit sequence is within rectangle
				uint32_t mask = 0xFFFFFFFF; // The mask used to blit onto the current 32 bit sequence
				if (x1r > j - 32) { // If some bits need to be shaved off the start of the mask
					mask >>= x1r % 32;
				}
				if (x2r < j) { // If some bits need to be shaved off the end of the mask
					uint32_t s = j - x2r;
					mask >>= s;
					mask <<= s;
				}
				// Blit the mask onto part of the row
				if (best_index == 0) {
					*row++ &= ~((uint8_t) (mask >> 24));
					*row++ &= ~((uint8_t) (mask >> 16));
					*row++ &= ~((uint8_t) (mask >> 8));
					*row++ &= ~((uint8_t) mask);
				} else {
					*row++ |= (uint8_t) (mask >> 24);
					*row++ |= (uint8_t) (mask >> 16);
					*row++ |= (uint8_t) (mask >> 8);
					*row++ |= (uint8_t) mask;
				}
			} else {
				row += 4;
			}
		}
	}
}

void bui_ctx_draw_pixel(bui_ctx_t *ctx, int16_t x, int16_t y, uint32_t color) {
	if (color >> 24 <= 127)
		return;
	if (x < 0 || x >= 128 || y < 0 || y >= 32)
		return;
	// Determine best color index
	uint8_t best_index = bui_palette_find_best(bui_ctx_palette, 2, color);
	// Extend the dirty rectangle
	bui_ctx_dirty(ctx, x, y, 1, 1);
	// Reflect coordinates
	x = 127 - x;
	y = 31 - y;
	// Find destination
	uint32_t dest_bit = y * 128 + x;
	uint32_t dest_byte = dest_bit / 8;
	dest_bit %= 8;
	// Set the target bit
	if (best_index == 0)
		ctx->bb[dest_byte] &= ~(0x80 >> dest_bit);
	else
		ctx->bb[dest_byte] |= 0x80 >> dest_bit;
}

void bui_ctx_draw_bitmap(bui_ctx_t *ctx, bui_const_bitmap_t bmp, int16_t src_x16, int16_t src_y16, int16_t dest_x16,
		int16_t dest_y16, int16_t w16, int16_t h16) {
	int32_t src_x = src_x16, src_y = src_y16, dest_x = dest_x16, dest_y = dest_y16, w = w16, h = h16;
	// Shift source and destination coordinates to fit in their coordinate planes
	if (dest_x < 0) {
		src_x -= dest_x;
		w += dest_x;
		dest_x = 0;
	}
	if (dest_y < 0) {
		src_y -= dest_y;
		h += dest_y;
		dest_y = 0;
	}
	if (src_x < 0) {
		dest_x -= src_x;
		w += src_x;
		src_x = 0;
	}
	if (src_y < 0) {
		dest_y -= src_y;
		h += src_y;
		src_y = 0;
	}
	if (w <= 0 || h <= 0)
		return;
	if (dest_x >= 128 || dest_y >= 32 || src_x >= bmp.w || src_y >= bmp.h)
		return;
	if (dest_x + w > 128)
		w = 128 - dest_x;
	if (dest_y + h > 32)
		h = 32 - dest_y;
	if (src_x + w > bmp.w)
		w = bmp.w - src_x;
	if (src_y + h > bmp.h)
		h = bmp.h - src_y;
	if (bmp.bpp == 0) {
		bui_ctx_fill_rect(ctx, dest_x, dest_y, w, h, bmp.plt[0]);
		return;
	}
	// Extend the dirty rectangle
	bui_ctx_dirty(ctx, dest_x, dest_y, w, h);
	// Blit the bitmap onto the display buffer
	if (bmp.bpp == 1) {
		// Simplify the bitmap's palette to one of nine possibilities
		// 0b0000 -> { transparent, transparent }
		// 0b0001 -> { transparent, black }
		// 0b0010 -> { transparent, white }
		// 0b0100 -> { black, transparent }
		// 0b0101 -> { black, black }
		// 0b0110 -> { black, white }
		// 0b1000 -> { white, transparent }
		// 0b1001 -> { white, black }
		// 0b1010 -> { white, white }
		uint8_t plt = 0;
		for (uint8_t i = 0; i < 2; i++) {
			plt <<= 2;
			if (bmp.plt[i] >> 24 >= 128)
				plt |= bui_palette_find_best(bui_ctx_palette, 2, bmp.plt[i]) == 0 ? 0b01 : 0b10;
		}
		// Determine the appropriate bitblit function using the simplified palette
		bui_bitblit_func_t bitblit_func;
		switch (plt) {
		case 0b0000: return;
		case 0b0001: bitblit_func = &bui_bitblit_and_not; break;
		case 0b0010: bitblit_func = &bui_bitblit_or; break;
		case 0b0100: bitblit_func = &bui_bitblit_and; break;
		case 0b0101: bui_ctx_fill_rect(ctx, dest_x, dest_y, w, h, BUI_CLR_BLACK); return;
		case 0b0110: bitblit_func = &bui_bitblit_set; break;
		case 0b1000: bitblit_func = &bui_bitblit_or_not; break;
		case 0b1001: bitblit_func = &bui_bitblit_not_set; break;
		case 0b1010: bui_ctx_fill_rect(ctx, dest_x, dest_y, w, h, BUI_CLR_WHITE); return;
		}
		// Reflect coordinates
		src_x = bmp.w - src_x - w;
		src_y = bmp.h - src_y - h;
		dest_x = 128 - dest_x - w; // index of the first column in the 2D bit array to be modified
		dest_y = 32 - dest_y - h; // index of the first row in the 2D bit array to be modified
		// Blit the bitmap onto the display buffer using the determined bitblit function
		for (int32_t i = 0; i < h; i++) {
			uint32_t src_o = (src_y + i) * bmp.w + src_x;
			uint32_t src_i = src_o / 8;
			src_o %= 8;
			uint32_t dest_o = (dest_y + i) * 128 + dest_x;
			uint32_t dest_i = dest_o / 8;
			dest_o %= 8;
			(*bitblit_func)(&bmp.bb[src_i], src_o, &ctx->bb[dest_i], dest_o, w);
		}
	} else {
		// TODO This can be significantly optimized
		for (int16_t row = dest_y; row < dest_y + h; row++) {
			for (int16_t col = dest_x; col < dest_x + w; col++) {
				uint8_t color_index;
				uint32_t index_i = (row * bmp.w + col) * bmp.bpp;
				bui_bitblit_set(&bmp.bb[index_i / 8], index_i % 8, &color_index, 8 - bmp.bpp, bmp.bpp);
				bui_ctx_draw_pixel(ctx, col, row, bmp.plt[color_index]);
			}
		}
	}
}

void bui_ctx_draw_bitmap_full(bui_ctx_t *ctx, bui_const_bitmap_t bmp, int16_t dest_x, int16_t dest_y) {
	bui_ctx_draw_bitmap(ctx, bmp, 0, 0, dest_x, dest_y, bmp.w, bmp.h);
}
