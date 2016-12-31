/*
 * License for the BOLOS User Interface project, originally found here:
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

#include "bui_font.h"

/*
 * Bitmaps in this library are represented as sequences of bits, each representing the color of a single pixel (a 1 bit
 * represents the foreground color, and a 0 bit represents the background color). Bitmaps are stored big-endian (first
 * bytes in the sequence appear in lower memory addresses), rows preferred, from bottom to top and from right to left.
 */

/*
 * Initialize various data structures used by the UI manager. This should always be called once before any other
 * function in this API is called. This function fills the top and bottom display buffers with the background color.
 */
void bui_init();

/*
 * Send additional data contained within the top display buffer to the MCU if available, otherwise do nothing.
 */
void bui_display();

/*
 * Indicate to the UI manager that the MCU has processed the last display status sent. This functions should only and
 * always be called after SEPROXYHAL_TAG_DISPLAY_PROCESSED_EVENT is triggered.
 */
void bui_display_processed();

/*
 * Transfer the bottom display buffer onto the top display buffer. The bottom buffer is the buffer that may be drawn
 * onto by the application, and the top buffer is the one in the process of being displayed on the screen. Calling this
 * function before the top buffer is fully displayed is legal.
 */
void bui_flush();

/*
 * Determine if the UI manager is still in the process of sending the top display buffer to the MCU to be displayed.
 *
 * Returns:
 *     false if the UI manager is in the process of sending the top display buffer to the MCU to be displayed, true
 *     otherwise
 */
bool bui_display_done();

/*
 * Fill the bottom display buffer with the specified color.
 *
 * Args:
 *     color: the color with which to fill the bottom display buffer; true is the foreground color and false is the
 *            background color
 */
void bui_fill(bool color);

/*
 * Invert every pixel in the bottom display buffer.
 */
void bui_invert();

/*
 * Fill a rectangle on the bottom display buffer. Any part of the rectangle out of bounds of the display will not be
 * drawn. If the width or height is 0, this function has no side effects.
 *
 * Args:
 *     x: the x-coordinate of top-left corner of the destination rectangle; must be >= -32,768 and <= 32,767
 *     y: the y-coordinate of top-left corner of the destination rectangle; must be >= -32,768 and <= 32,767
 *     w: the width of the rectangle; must be >= 0 and <= 32,767
 *     h: the height of the rectangle; must be >= 0 and <= 32,767
 *     color: the color with which to fill the rectangle; true is the foreground color and false is the background color
 */
void bui_fill_rect(int x, int y, int w, int h, bool color);

/*
 * Draw a bitmap onto the bottom display buffer given a source rectangle on the bitmap's coordinate plane and a
 * destination rectangle on the display's coordinate plane. Any part of the destination rectangle out of bounds of the
 * display will not be drawn. The source rectangle must be entirely within the source bitmap. If the width or height is
 * 0, this function has no side effects.
 *
 * Args:
 *     bitmap: the pointer to the bitmap to be drawn; for each pixel, true is the foreground color and false is the
 *             background color
 *     bitmap_w: the number of pixels in a single row in the bitmap; must be >= 0 and <= 32,767
 *     src_x: the x-coordinate of the top-left corner of the source rectangle within the source bitmap's coordinate
 *            plane; must be >= 0 and <= 32,767
 *     src_y: the y-coordinate of the top-left corner of the source rectangle within the source bitmap's coordinate
 *            plane; must be >= 0 and <= 32,767
 *     dest_x: the x-coordinate of the top-left corner of the destination rectangle on or outside of the display's
 *             coordinate plane; must be >= -32,768 and <= 32,767
 *     dest_y: the y-coordinate of the top-left corner of the destination rectangle on or outside of the display's
 *             coordinate plane; must be >= -32,768 and <= 32,767
 *     w: the width of the source and destination rectangles; must be >= 0 and <= 32,767
 *     h: the height of the source and destination rectangles; must be >= 0 and <= 32,767
 */
void bui_draw_bitmap(const unsigned char *bitmap, int bitmap_w, int src_x, int src_y, int dest_x, int dest_y, int w,
		int h);

/*
 * Draw a character in the specified font onto the bottom display buffer. Any part of the character out of bounds of the
 * display will not be drawn.
 *
 * Args:
 *     ch: the character code of the character to be drawn
 *     x: the x-coordinate of the top-left corner of the destination; must be >= -32,768 and <= 32,767
 *     y: the y-coordinate of the top-left corner of the destination; must be >= -32,768 and <= 32,767
 *     font_id: the ID of the font to be used to render the character
 */
void bui_draw_char(unsigned char ch, int x, int y, bui_font_id_e font_id);

/*
 * Draw a string in the specified font onto the bottom display buffer. Any part of the string out of bounds of the
 * buffer will not be drawn (the string will not wrap).
 *
 * Args:
 *     str: the null-terminated string to be drawn; may not be NULL
 *     x: the x-coordinate of the top-left corner of the destination; must be >= -32,768 and <= 32,767
 *     y: the y-coordinate of the top-left corner of the destination; must be >= -32,768 and <= 32,767
 *     font_id: the ID of the font to be used to render the string
 */
void bui_draw_string(const unsigned char *str, int x, int y, bui_font_id_e font_id);

#endif
