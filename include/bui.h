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

#ifndef BUI_H_
#define BUI_H_

#include <stdbool.h>
#include <stdint.h>

_Static_assert(sizeof(uint8_t) == 1, "sizeof(uint8_t) must be 1");

#define BUI_VER_MAJOR 0
#define BUI_VER_MINOR 5
#define BUI_VER_PATCH 0

typedef struct {
	// A 2-dimensional bit array (or "bit block") representing the contents of the bitmap. The array is encoded as a
	// sequence of bits, starting at the most significant bit, which is 128 * 32 bits in length, with big-endian byte
	// order. Every 128 bits in the sequence is a row, with 32 rows in total. The values of cells in this array (a bit
	// at a specific row and column) correspond to the colors of the pixels at their respective location, except the
	// order of rows and columns are both reversed.
	uint8_t bb[512];
} bui_bitmap_128x32_t;

typedef struct {
	int16_t w;
	int16_t h;
	// A 2-dimensional bit array (or "bit block") representing the contents of the bitmap. The array is encoded as a
	// sequence of bits, starting at the most significant bit, which is w * h bits in length, with big-endian byte
	// order. Every w bits in the sequence is a row, with h rows in total. The values of cells in this array (a bit at a
	// specific row and column) correspond to the colors of the pixels at their respective location, except the order of
	// rows and columns are both reversed.
	uint8_t *bb;
} bui_bitmap_t;

typedef struct {
	int16_t w;
	int16_t h;
	// A 2-dimensional bit array (or "bit block") representing the contents of the bitmap. The array is encoded as a
	// sequence of bits, starting at the most significant bit, which is w * h bits in length, with big-endian byte
	// order. Every w bits in the sequence is a row, with h rows in total. The values of cells in this array (a bit at a
	// specific row and column) correspond to the colors of the pixels at their respective location, except the order of
	// rows and columns are both reversed.
	const uint8_t *bb;
} bui_const_bitmap_t;

typedef enum {
	BUI_DIR_LEFT         = 0b00000001,
	BUI_DIR_RIGHT        = 0b00000010,
	BUI_DIR_TOP          = 0b00000100,
	BUI_DIR_BOTTOM       = 0b00001000,
	BUI_DIR_CENTER       = 0b00010000,
	BUI_DIR_LEFT_TOP     = (int) BUI_DIR_LEFT | (int) BUI_DIR_TOP,
	BUI_DIR_LEFT_BOTTOM  = (int) BUI_DIR_LEFT | (int) BUI_DIR_BOTTOM,
	BUI_DIR_RIGHT_TOP    = (int) BUI_DIR_RIGHT | (int) BUI_DIR_TOP,
	BUI_DIR_RIGHT_BOTTOM = (int) BUI_DIR_RIGHT | (int) BUI_DIR_BOTTOM,
} bui_dir_e;

/*
 * Returns true if dir is on the left edge, false otherwise.
 */
static inline bool bui_dir_is_left(bui_dir_e dir) {
	return (dir & BUI_DIR_LEFT) != 0;
}

/*
 * Returns true if dir is on the right edge, false otherwise.
 */
static inline bool bui_dir_is_right(bui_dir_e dir) {
	return (dir & BUI_DIR_RIGHT) != 0;
}

/*
 * Returns true if dir is on the top edge, false otherwise.
 */
static inline bool bui_dir_is_top(bui_dir_e dir) {
	return (dir & BUI_DIR_TOP) != 0;
}

/*
 * Returns true if dir is on the bottom edge, false otherwise.
 */
static inline bool bui_dir_is_bottom(bui_dir_e dir) {
	return (dir & BUI_DIR_BOTTOM) != 0;
}

/*
 * Returns true if dir is horizontally centered, false otherwise.
 */
static inline bool bui_dir_is_htl_center(bui_dir_e dir) {
	return (dir & (BUI_DIR_LEFT | BUI_DIR_RIGHT)) == 0;
}

/*
 * Returns true if dir is vertically centered, false otherwise.
 */
static inline bool bui_dir_is_vtl_center(bui_dir_e dir) {
	return (dir & (BUI_DIR_TOP | BUI_DIR_BOTTOM)) == 0;
}

#define BUI_DECLARE_BITMAP(name) \
		extern const uint8_t bui_bitmap_ ## name ## _w; \
		extern const uint8_t bui_bitmap_ ## name ## _h; \
		extern const uint8_t bui_bitmap_ ## name ## _bitmap[];

BUI_DECLARE_BITMAP(check);
#define BUI_BITMAP_CHECK ((bui_const_bitmap_t) { .w = bui_bitmap_check_w, .h = bui_bitmap_check_h, .bb = bui_bitmap_check_bitmap })
BUI_DECLARE_BITMAP(cross);
#define BUI_BITMAP_CROSS ((bui_const_bitmap_t) { .w = bui_bitmap_cross_w, .h = bui_bitmap_cross_h, .bb = bui_bitmap_cross_bitmap })
BUI_DECLARE_BITMAP(left);
#define BUI_BITMAP_LEFT ((bui_const_bitmap_t) { .w = bui_bitmap_left_w, .h = bui_bitmap_left_h, .bb = bui_bitmap_left_bitmap })
BUI_DECLARE_BITMAP(right);
#define BUI_BITMAP_RIGHT ((bui_const_bitmap_t) { .w = bui_bitmap_right_w, .h = bui_bitmap_right_h, .bb = bui_bitmap_right_bitmap })
BUI_DECLARE_BITMAP(up);
#define BUI_BITMAP_UP ((bui_const_bitmap_t) { .w = bui_bitmap_up_w, .h = bui_bitmap_up_h, .bb = bui_bitmap_up_bitmap })
BUI_DECLARE_BITMAP(down);
#define BUI_BITMAP_DOWN ((bui_const_bitmap_t) { .w = bui_bitmap_down_w, .h = bui_bitmap_down_h, .bb = bui_bitmap_down_bitmap })
BUI_DECLARE_BITMAP(left_filled);
#define BUI_BITMAP_LEFT_FILLED ((bui_const_bitmap_t) { .w = bui_bitmap_left_filled_w, .h = bui_bitmap_left_filled_h, .bb = bui_bitmap_left_filled_bitmap })
BUI_DECLARE_BITMAP(right_filled);
#define BUI_BITMAP_RIGHT_FILLED ((bui_const_bitmap_t) { .w = bui_bitmap_right_filled_w, .h = bui_bitmap_right_filled_h, .bb = bui_bitmap_right_filled_bitmap })
BUI_DECLARE_BITMAP(up_filled);
#define BUI_BITMAP_UP_FILLED ((bui_const_bitmap_t) { .w = bui_bitmap_up_filled_w, .h = bui_bitmap_up_filled_h, .bb = bui_bitmap_up_filled_bitmap })
BUI_DECLARE_BITMAP(down_filled);
#define BUI_BITMAP_DOWN_FILLED ((bui_const_bitmap_t) { .w = bui_bitmap_down_filled_w, .h = bui_bitmap_down_filled_h, .bb = bui_bitmap_down_filled_bitmap })
BUI_DECLARE_BITMAP(ledger_mini);
#define BUI_BITMAP_LEDGER_MINI ((bui_const_bitmap_t) { .w = bui_bitmap_ledger_mini_w, .h = bui_bitmap_ledger_mini_h, .bb = bui_bitmap_ledger_mini_bitmap })
BUI_DECLARE_BITMAP(badge_cross);
#define BUI_BITMAP_BADGE_CROSS ((bui_const_bitmap_t) { .w = bui_bitmap_badge_cross_w, .h = bui_bitmap_badge_cross_h, .bb = bui_bitmap_badge_cross_bitmap })
BUI_DECLARE_BITMAP(badge_dashboard);
#define BUI_BITMAP_BADGE_DASHBOARD ((bui_const_bitmap_t) { .w = bui_bitmap_badge_dashboard_w, .h = bui_bitmap_badge_dashboard_h, .bb = bui_bitmap_badge_dashboard_bitmap })

/*
 * Send some data contained within the provided display buffer to the MCU to be displayed.
 *
 * Args:
 *     buffer: the display buffer to be displayed; pixels with a color index of 1 are displayed as the foreground color,
 *             and pixels with a color index of 0 are displayed as the background color
 *     progress: the progress made displaying this buffer; this should be 0 to start displaying the buffer anew, or a
 *               value previously returned by bui_display(...), but not -1
 * Returns:
 *     the progress made displaying the buffer, or -1 if the buffer has been completely displayed
 */
int8_t bui_display(bui_bitmap_128x32_t *buffer, int8_t progress);

/*
 * Fill the provided display buffer with the specified color.
 *
 * Args:
 *     buffer: the display buffer to be filled
 *     color: the color with which to fill the display buffer; true fills with 1 bits and false fills with 0 bits
 */
void bui_fill(bui_bitmap_128x32_t *buffer, bool color);

/*
 * Invert every pixel in the provided display buffer.
 *
 * Args:
 *     buffer: the display buffer to be inverted
 */
void bui_invert(bui_bitmap_128x32_t *buffer);

/*
 * Fill a rectangle on the provided display buffer. Any part of the rectangle out of bounds of the display will not be
 * drawn. If the width or height is 0, this function has no side effects.
 *
 * Args:
 *     buffer: the display buffer onto which to draw the rectangle
 *     x: the x-coordinate of top-left corner of the destination rectangle
 *     y: the y-coordinate of top-left corner of the destination rectangle
 *     w: the width of the rectangle; must be >= 0
 *     h: the height of the rectangle; must be >= 0
 *     color: the color with which to fill the rectangle; true fills with 1 bits and false fills with 0 bits
 */
void bui_fill_rect(bui_bitmap_128x32_t *buffer, int16_t x, int16_t y, int16_t w, int16_t h, bool color);

/*
 * Set the color of a single pixel on the provided display buffer. If the coordinates of the pixel are out of bounds of
 * the display, this function has no side effects.
 *
 * Args:
 *     buffer: the display buffer whose pixel is to be set
 *     x: the x-coordinate of the pixel to be set
 *     y: the y-coordinate of the pixel to be set
 *     color: the color to which the pixel is to be set; true fills with 1 bits and false fills with 0 bits
 */
void bui_set_pixel(bui_bitmap_128x32_t *buffer, int16_t x, int16_t y, bool color);

/*
 * Draw a bitmap onto the provided display buffer given a source rectangle on the bitmap's coordinate plane and a
 * destination rectangle on the buffer's coordinate plane. Any part of the destination rectangle out of bounds of the
 * buffer will not be drawn. The source rectangle must be entirely within the source bitmap. If the width or height is
 * 0, this function does not modify the bitmap. Pixels with a color index of 0 in the source bitmap are interpreted as
 * transparent, and pixels with a color index of 1 are written to the destination buffer.
 *
 * Args:
 *     buffer: the display buffer onto which to draw the bitmap
 *     bitmap: the bitmap to be drawn onto buffer
 *     src_x: the x-coordinate of the top-left corner of the source rectangle on or outside of bitmap's coordinate plane
 *     src_y: the y-coordinate of the top-left corner of the source rectangle on or outside of bitmap's coordinate plane
 *     dest_x: the x-coordinate of the top-left corner of the destination rectangle on or outside of buffer's coordinate
 *             plane
 *     dest_y: the y-coordinate of the top-left corner of the destination rectangle on or outside of buffer's coordinate
 *             plane
 *     w: the width of the source and destination rectangles; must be >= 0
 *     h: the height of the source and destination rectangles; must be >= 0
 */
void bui_draw_bitmap(bui_bitmap_128x32_t *buffer, bui_const_bitmap_t bitmap, int16_t src_x, int16_t src_y,
		int16_t dest_x, int16_t dest_y, int16_t w, int16_t h);

#endif
