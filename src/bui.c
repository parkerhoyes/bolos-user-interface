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

int8_t bui_display(bui_bitmap_128x32_t *buffer, int8_t progress) {
	unsigned int color_index[] = {0x00000000, 0x00FFFFFF};
	uint8_t section[64];
	for (uint8_t i = 0; i < 64; i++)
		section[i] = buffer->bb[511 - progress * 64 - i];
	io_seproxyhal_display_bitmap(0, progress * 4, 128, 4, color_index, 1, section);
	if (progress != 7)
		return ++progress;
	else
		return -1;
}

void bui_fill(bui_bitmap_128x32_t *buffer, bool color) {
	os_memset(buffer->bb, color ? 0xFF : 0x00, sizeof(buffer->bb));
}

void bui_invert(bui_bitmap_128x32_t *buffer) {
	uint32_t *end = (uint32_t*) &buffer->bb[512];
	for (uint32_t *ptr = (uint32_t*) &buffer->bb[0]; ptr != end; ptr++) {
		*ptr = ~(*ptr);
	}
}

void bui_fill_rect(bui_bitmap_128x32_t *buffer, int16_t x16, int16_t y16, int16_t w16, int16_t h16, bool color) {
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
	// Calculate reflected coordinates
	int32_t x1r = 128 - x - w; // index of the first column in the 2D bit array to be modified
	int32_t y1r = 32 - y - h; // index of the first row in the 2D bit array to be modified
	int32_t x2r = x1r + w; // index just beyond the last column in the 2D bit array to be modified
	int32_t y2r = y1r + h; // index just beyond the last row in the 2D bit array to be modified
	for (int32_t i = y1r; i < y2r; i++) {
		uint8_t *row = &buffer->bb[i * 16];
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

void bui_set_pixel(bui_bitmap_128x32_t *buffer, int16_t x, int16_t y, bool color) {
	if (x < 0 || x >= 128 || y < 0 || y >= 32)
		return;
	// Reflect coordinates
	x = 128 - x;
	y = 32 - y;
	// Find destination
	uint32_t dest_bit = y * 128 + x;
	uint32_t dest_byte = dest_bit / 8;
	dest_bit %= 8;
	// Blit
	if (color)
		buffer->bb[dest_byte] |= 0x80 >> dest_bit;
	else
		buffer->bb[dest_byte] &= ~(0x80 >> dest_bit);
}

void bui_draw_bitmap(bui_bitmap_128x32_t *buffer, bui_const_bitmap_t bitmap, int16_t src_x16, int16_t src_y16,
			int16_t dest_x16, int16_t dest_y16, int16_t w16, int16_t h16) {
	int32_t src_x = src_x16, src_y = src_y16, dest_x = dest_x16, dest_y = dest_y16, w = w16, h = h16;
	if (bitmap.w == 0 || bitmap.h == 0)
		return;
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
	if (dest_x >= 128 || dest_y >= 32 || src_x >= bitmap.w || src_y >= bitmap.h)
		return;
	if (dest_x + w > 128)
		w = 128 - dest_x;
	if (dest_y + h > 32)
		h = 32 - dest_y;
	if (src_x + w > bitmap.w)
		w = bitmap.w - src_x;
	if (src_y + h > bitmap.h)
		h = bitmap.h - src_y;
	// Reflect coordinates
	src_x = bitmap.w - src_x - w;
	src_y = bitmap.h - src_y - h;
	dest_x = 128 - dest_x - w; // index of the first column in the 2D bit array to be modified
	dest_y = 32 - dest_y - h; // index of the first row in the 2D bit array to be modified
	for (int32_t i = 0; i < h; i++) {
		uint32_t src_o = (src_y + i) * bitmap.w + src_x;
		uint32_t src_i = src_o / 8;
		src_o %= 8;
		uint32_t dest_o = (dest_y + i) * 128 + dest_x;
		uint32_t dest_i = dest_o / 8;
		dest_o %= 8;
		bui_bitblit_or(&bitmap.bb[src_i], src_o, &buffer->bb[dest_i], dest_o, w);
	}
}
