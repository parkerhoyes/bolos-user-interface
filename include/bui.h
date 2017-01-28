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

#define BUI_VER_MAJOR 0
#define BUI_VER_MINOR 4
#define BUI_VER_PATCH 0

typedef struct bui_bitmap_128x32_t {
	// A 128x32 bitmap. Every 128 bits is a row ordered from bottom to top, each row containing 128 pixels ordered from
	// right to left on the screen. The foreground color is represented by 1 bits, and the background color by 0 bits.
	unsigned char bitmap[512];
} bui_bitmap_128x32_t;

/*
 * Bitmaps in this library are represented as sequences of bits, each representing the color of a single pixel (a 1 bit
 * represents the foreground color, and a 0 bit represents the background color). Bitmaps are stored big-endian (first
 * bytes in the sequence appear in lower memory addresses), rows preferred, from bottom to top and from right to left.
 */

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

#define BUI_DIR_IS_LEFT(dir) (((int) (dir) & (int) BUI_DIR_LEFT) != 0)
#define BUI_DIR_IS_RIGHT(dir) (((int) (dir) & (int) BUI_DIR_RIGHT) != 0)
#define BUI_DIR_IS_TOP(dir) (((int) (dir) & (int) BUI_DIR_TOP) != 0)
#define BUI_DIR_IS_BOTTOM(dir) (((int) (dir) & (int) BUI_DIR_BOTTOM) != 0)
#define BUI_DIR_IS_HTL_CENTER(dir) (((int) (dir) & ~((int) BUI_DIR_TOP | (int) BUI_DIR_BOTTOM)) == 0)
#define BUI_DIR_IS_VTL_CENTER(dir) (((int) (dir) & ~((int) BUI_DIR_LEFT | (int) BUI_DIR_RIGHT)) == 0)

#define BUI_DECLARE_BITMAP(name) \
		extern const unsigned char bui_bitmap_ ## name ## _w; \
		extern const unsigned char bui_bitmap_ ## name ## _h; \
		extern const unsigned char bui_bitmap_ ## name ## _bitmap[];

BUI_DECLARE_BITMAP(check);
BUI_DECLARE_BITMAP(cross);
BUI_DECLARE_BITMAP(left);
BUI_DECLARE_BITMAP(right);
BUI_DECLARE_BITMAP(up);
BUI_DECLARE_BITMAP(down);
BUI_DECLARE_BITMAP(left_filled);
BUI_DECLARE_BITMAP(right_filled);
BUI_DECLARE_BITMAP(up_filled);
BUI_DECLARE_BITMAP(down_filled);
BUI_DECLARE_BITMAP(ledger_mini);
BUI_DECLARE_BITMAP(badge_cross);
BUI_DECLARE_BITMAP(badge_dashboard);

/*
 * Send some data contained within the provided display buffer to the MCU to be displayed.
 *
 * Args:
 *     buffer: the display buffer to be displayed
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
 *     color: the color with which to fill the display buffer; true is the foreground color and false is the background
 *            color
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
 *     x: the x-coordinate of top-left corner of the destination rectangle; must be >= -32,768 and <= 32,767
 *     y: the y-coordinate of top-left corner of the destination rectangle; must be >= -32,768 and <= 32,767
 *     w: the width of the rectangle; must be >= 0 and <= 32,767
 *     h: the height of the rectangle; must be >= 0 and <= 32,767
 *     color: the color with which to fill the rectangle; true is the foreground color and false is the background color
 */
void bui_fill_rect(bui_bitmap_128x32_t *buffer, int x, int y, int w, int h, bool color);

/*
 * Set the color of a single pixel on the provided display buffer. If the coordinates of the pixel are out of bounds of
 * the display, this function has no side effects.
 *
 * Args:
 *     buffer: the display buffer whose pixel is to be set
 *     x: the x-coordinate of the pixel to be set
 *     y: the y-coordinate of the pixel to be set
 *     color: the color to which the pixel is to be set; true is the foreground color and false is the background color
 */
void bui_set_pixel(bui_bitmap_128x32_t *buffer, int x, int y, bool color);

/*
 * Draw a bitmap onto the provided display buffer given a source rectangle on the bitmap's coordinate plane and a
 * destination rectangle on the buffer's coordinate plane. Any part of the destination rectangle out of bounds of the
 * buffer will not be drawn. The source rectangle must be entirely within the source bitmap. If the width or height is
 * 0, this function has no side effects.
 *
 * Args:
 *     buffer: the display buffer onto which to draw the bitmap
 *     bitmap: the pointer to the bitmap to be drawn; for each pixel, true is the foreground color and false is the
 *             background color
 *     bitmap_w: the number of pixels in a single row in the bitmap; must be >= 0 and <= 32,767
 *     src_x: the x-coordinate of the top-left corner of the source rectangle within the source bitmap's coordinate
 *            plane; must be >= 0 and <= 32,767
 *     src_y: the y-coordinate of the top-left corner of the source rectangle within the source bitmap's coordinate
 *            plane; must be >= 0 and <= 32,767
 *     dest_x: the x-coordinate of the top-left corner of the destination rectangle on or outside of the buffer's
 *             coordinate plane; must be >= -32,768 and <= 32,767
 *     dest_y: the y-coordinate of the top-left corner of the destination rectangle on or outside of the buffer's
 *             coordinate plane; must be >= -32,768 and <= 32,767
 *     w: the width of the source and destination rectangles; must be >= 0 and <= 32,767
 *     h: the height of the source and destination rectangles; must be >= 0 and <= 32,767
 */
void bui_draw_bitmap(bui_bitmap_128x32_t *buffer, const unsigned char *bitmap, int bitmap_w, int src_x, int src_y,
		int dest_x, int dest_y, int w, int h);

#endif
