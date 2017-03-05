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

#include "bui.h"

#include <stdbool.h>
#include <stdint.h>

#include "os.h"
#include "os_io_seproxyhal.h"

#include "bui_font.h"

#include "bui_bitmaps.inc"

/*
 * Copy a sequence of bits from one location to another. The source and destination sequences may not be overlapping. No
 * bytes that do not contain bits in either of the source or destination sequences are accessed.
 *
 * Args:
 *     src: the pointer to the byte containing the first bit in the source sequence
 *     src_o: the index of the first bit in the source sequence in its byte (0 for most significant bit, 7 for least);
 *            must be <= 7
 *     dest: the pointer to the byte containing the first bit in the destination sequence
 *     dest_o: the index of the first bit in the destination sequence in its byte (0 for most significant bit, 7 for
 *             least); must be <= 7
 *     n: the number of bits to copy; also the number of bits in each of the source and destination sequences
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
 * Perform a bitwise OR with a sequence of bits from one location to another. The source and destination sequences may
 * not be overlapping. No bytes that do not contain bits in either of the source or destination sequences are accessed.
 *
 * Args:
 *     src: the pointer to the byte containing the first bit in the source sequence
 *     src_o: the index of the first bit in the source sequence in its byte (0 for most significant bit, 7 for least);
 *            must be <= 7
 *     dest: the pointer to the byte containing the first bit in the destination sequence
 *     dest_o: the index of the first bit in the destination sequence in its byte (0 for most significant bit, 7 for
 *             least); must be <= 7
 *     n: the number of bits to OR; also the number of bits in each of the source and destination sequences
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

void bui_bm_fill(bui_bitmap_t bm, bool color) {
	uint16_t w = bm.w;
	uint16_t h = bm.h;
	os_memset(bm.bb, color ? 0xFF : 0x00, w * h / 8);
	uint8_t extra = w * h % 8;
	if (extra == 0)
		return;
	if (color)
		bm.bb[w * h / 8] |= ~((uint8_t) 0xFF >> extra);
	else
		bm.bb[w * h / 8] &= (uint8_t) 0xFF >> extra;
}

void bui_bm_invert(bui_bitmap_t bm) {
	uint16_t w = bm.w;
	uint16_t h = bm.h;
	uint8_t *end = (uint8_t*) &bm.bb[w * h / 8];
	for (uint8_t *ptr = bm.bb; ptr != end; ptr++)
		*ptr = ~(*ptr);
	uint8_t extra = w * h % 8;
	if (extra == 0)
		return;
	uint8_t mask = ~((uint8_t) 0xFF >> extra);
	*end ^= mask;
}

void bui_bm_draw_pixel(bui_bitmap_t bm, int16_t x, int16_t y, bool color) {
	if (x < 0 || x >= bm.w || y < 0 || y >= bm.h)
		return;
	// Reflect coordinates
	x = bm.w - x;
	y = bm.h - y;
	// Find destination
	uint32_t dest_bit = y * bm.w + x;
	uint32_t dest_byte = dest_bit / 8;
	dest_bit %= 8;
	// Set the target bit
	if (color)
		bm.bb[dest_byte] |= 0x80 >> dest_bit;
	else
		bm.bb[dest_byte] &= ~(0x80 >> dest_bit);
}

void bui_ctx_init(bui_ctx_t *ctx) {
	os_memset(ctx->bb, 0, sizeof(ctx->bb));
	ctx->dirty_x = 0;
	ctx->dirty_y = 0;
	ctx->dirty_w = 128;
	ctx->dirty_h = 32;
}

bool bui_ctx_display(bui_ctx_t *ctx) {
	if (ctx->dirty_w == 0 || ctx->dirty_h == 0)
		return false;
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
	unsigned int color_index[] = {0x00000000, 0x00FFFFFF};
	io_seproxyhal_display_bitmap(ctx->dirty_x, ctx->dirty_y, sub_w, sub_h, color_index, 1, sub);
	// Exclude subrectangle from the dirty rectangle
	if (sub_w != ctx->dirty_w) {
		ctx->dirty_x += sub_w;
		ctx->dirty_w -= sub_w;
	} else {
		ctx->dirty_y += sub_h;
		ctx->dirty_h -= sub_h;
	}
	return true;
}

bool bui_ctx_is_displayed(const bui_ctx_t *ctx) {
	return ctx->dirty_w == 0 || ctx->dirty_h == 0;
}

void bui_ctx_fill(bui_ctx_t *ctx, bool color) {
	ctx->dirty_x = 0;
	ctx->dirty_y = 0;
	ctx->dirty_w = 128;
	ctx->dirty_h = 32;
	os_memset(ctx->bb, color ? 0xFF : 0x00, sizeof(ctx->bb));
}

void bui_ctx_fill_rect(bui_ctx_t *ctx, int16_t x16, int16_t y16, int16_t w16, int16_t h16, bool color) {
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
	// Extend the dirty rectangle
	bui_ctx_dirty(ctx, x, y, w, h);
	// Calculate reflected coordinates
	int32_t x1r = 128 - x - w; // index of the first column in the 2D bit array to be modified
	int32_t y1r = 32 - y - h; // index of the first row in the 2D bit array to be modified
	int32_t x2r = x1r + w; // index just beyond the last column in the 2D bit array to be modified
	int32_t y2r = y1r + h; // index just beyond the last row in the 2D bit array to be modified
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
				if (color) {
					*row++ |= (uint8_t) (mask >> 24);
					*row++ |= (uint8_t) (mask >> 16);
					*row++ |= (uint8_t) (mask >> 8);
					*row++ |= (uint8_t) mask;
				} else {
					*row++ &= ~((uint8_t) (mask >> 24));
					*row++ &= ~((uint8_t) (mask >> 16));
					*row++ &= ~((uint8_t) (mask >> 8));
					*row++ &= ~((uint8_t) mask);
				}
			} else {
				row += 4;
			}
		}
	}
}

void bui_ctx_draw_pixel(bui_ctx_t *ctx, int16_t x, int16_t y, bool color) {
	if (x < 0 || x >= 128 || y < 0 || y >= 32)
		return;
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
	if (color)
		ctx->bb[dest_byte] |= 0x80 >> dest_bit;
	else
		ctx->bb[dest_byte] &= ~(0x80 >> dest_bit);
}

void bui_ctx_draw_bitmap(bui_ctx_t *ctx, bui_const_bitmap_t bm, int16_t src_x16, int16_t src_y16, int16_t dest_x16,
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
	if (dest_x >= 128 || dest_y >= 32 || src_x >= bm.w || src_y >= bm.h)
		return;
	if (dest_x + w > 128)
		w = 128 - dest_x;
	if (dest_y + h > 32)
		h = 32 - dest_y;
	if (src_x + w > bm.w)
		w = bm.w - src_x;
	if (src_y + h > bm.h)
		h = bm.h - src_y;
	// Extend the dirty rectangle
	bui_ctx_dirty(ctx, dest_x, dest_y, w, h);
	// Reflect coordinates
	src_x = bm.w - src_x - w;
	src_y = bm.h - src_y - h;
	dest_x = 128 - dest_x - w; // index of the first column in the 2D bit array to be modified
	dest_y = 32 - dest_y - h; // index of the first row in the 2D bit array to be modified
	for (int32_t i = 0; i < h; i++) {
		uint32_t src_o = (src_y + i) * bm.w + src_x;
		uint32_t src_i = src_o / 8;
		src_o %= 8;
		uint32_t dest_o = (dest_y + i) * 128 + dest_x;
		uint32_t dest_i = dest_o / 8;
		dest_o %= 8;
		bui_bitblit_set(&bm.bb[src_i], src_o, &ctx->bb[dest_i], dest_o, w);
	}
}

void bui_ctx_draw_bitmap_full(bui_ctx_t *ctx, bui_const_bitmap_t bm, int16_t dest_x, int16_t dest_y) {
	bui_ctx_draw_bitmap(ctx, bm, 0, 0, dest_x, dest_y, bm.w, bm.h);
}

void bui_ctx_draw_mbitmap(bui_ctx_t *ctx, bui_const_bitmap_t bm, int16_t src_x16, int16_t src_y16, int16_t dest_x16,
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
	if (dest_x >= 128 || dest_y >= 32 || src_x >= bm.w || src_y >= bm.h)
		return;
	if (dest_x + w > 128)
		w = 128 - dest_x;
	if (dest_y + h > 32)
		h = 32 - dest_y;
	if (src_x + w > bm.w)
		w = bm.w - src_x;
	if (src_y + h > bm.h)
		h = bm.h - src_y;
	// Extend the dirty rectangle
	bui_ctx_dirty(ctx, dest_x, dest_y, w, h);
	// Reflect coordinates
	src_x = bm.w - src_x - w;
	src_y = bm.h - src_y - h;
	dest_x = 128 - dest_x - w; // index of the first column in the 2D bit array to be modified
	dest_y = 32 - dest_y - h; // index of the first row in the 2D bit array to be modified
	for (int32_t i = 0; i < h; i++) {
		uint32_t src_o = (src_y + i) * bm.w + src_x;
		uint32_t src_i = src_o / 8;
		src_o %= 8;
		uint32_t dest_o = (dest_y + i) * 128 + dest_x;
		uint32_t dest_i = dest_o / 8;
		dest_o %= 8;
		bui_bitblit_or(&bm.bb[src_i], src_o, &ctx->bb[dest_i], dest_o, w);
	}
}

void bui_ctx_draw_mbitmap_full(bui_ctx_t *ctx, bui_const_bitmap_t bm, int16_t dest_x, int16_t dest_y) {
	bui_ctx_draw_mbitmap(ctx, bm, 0, 0, dest_x, dest_y, bm.w, bm.h);
}
