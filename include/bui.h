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

#ifndef BUI_H_
#define BUI_H_

#include <stdbool.h>
#include <stdint.h>

_Static_assert(sizeof(uint8_t) == 1, "sizeof(uint8_t) must be 1");

#define BUI_VER_MAJOR 0
#define BUI_VER_MINOR 7
#define BUI_VER_PATCH 0

// Colors are encoded in ARGB 8888 (alpha-red-green-blue, 8 bits per channel).
#define BUI_CLR_BLACK        0xFF000000
#define BUI_CLR_WHITE        0xFFFFFFFF
#define BUI_CLR_TRANSPARENT  0x00000000

// NOTE: The definition of this struct is considered internal; it may be changed between versions without warning.
typedef struct {
	// The data for the display buffer bitmap (128x32). This is a 2-dimensional bit array (or "bit block") representing
	// the contents of the bitmap. The array is encoded as a sequence of bits, starting at the most significant bit,
	// which is 128 * 32 bits in length, with big-endian byte order. Every 128 bits in the sequence is a row, with 32
	// rows in total. The values of cells in this array (a bit at a specific row and column) correspond to the color
	// index of the pixels at their respective location, except the order of rows and columns are both reversed. The
	// palette of this bitmap is {0xFF000000, 0xFF0000FF}.
	uint8_t bb[512];
	// The x-coordinate of the dirty rectangle of this context; always in [0, 127] if dirty_w and dirty_h != 0
	uint8_t dirty_x;
	// The y-coordinate of the dirty rectangle of this context; always in [0, 31] if dirty_w and dirty_h != 0
	uint8_t dirty_y;
	// The width of the dirty rectangle of this context; always in [0, 128]
	uint8_t dirty_w;
	// The height of the dirty rectangle of this context; always in [0, 32]
	uint8_t dirty_h;
} bui_ctx_t;

typedef struct {
	// The bitmap width in pixels; this must be > 0
	int16_t w;
	// The bitmap height in pixels; this must be > 0
	int16_t h;
	// A 2-dimensional bit array (or "bit block") representing the contents of the bitmap. The array is encoded as a
	// sequence of bits, starting at the most significant bit, which is bpp * w * h bits in length, with big-endian byte
	// order. Every bpp * w bits in the sequence is a row, with h rows in total. The values of cells in this array
	// (sequences of bpp bits in a row) correspond to the color index of the pixels at their respective location, except
	// the order of rows and columns are both reversed. If bpp is 0, this shall not be accessed.
	uint8_t *bb;
	// The palette of this bitmap. The element of this array at index i is the color corresponding to the color index i.
	// Each element is encoded as ARGB 8888. The length of this array is 2^bpp. If bpp is 0, the entire bitmap has the
	// color plt[0].
	const uint32_t *plt;
	// The number of bits used to represent a color index in the bitmap; this must be <= 4
	uint8_t bpp;
} bui_bitmap_t;

typedef struct {
	// The bitmap width in pixels; this must be > 0
	int16_t w;
	// The bitmap height in pixels; this must be > 0
	int16_t h;
	// A 2-dimensional bit array (or "bit block") representing the contents of the bitmap. The array is encoded as a
	// sequence of bits, starting at the most significant bit, which is bpp * w * h bits in length, with big-endian byte
	// order. Every bpp * w bits in the sequence is a row, with h rows in total. The values of cells in this array
	// (sequences of bpp bits in a row) correspond to the color index of the pixels at their respective location, except
	// the order of rows and columns are both reversed. If bpp is 0, this shall not be accessed.
	const uint8_t *bb;
	// The palette of this bitmap. The element of this array at index i is the color corresponding to the color index i.
	// Each element is encoded as ARGB 8888. The length of this array is 2^bpp. If bpp is 0, the entire bitmap has the
	// color plt[0].
	const uint32_t *plt;
	// The number of bits used to represent a color index in the bitmap; this must be <= 4
	uint8_t bpp;
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
 * Evaluate to true if dir is on the left edge, false otherwise.
 */
#define BUI_DIR_IS_LEFT(dir) ((dir & BUI_DIR_LEFT) != 0)

/*
 * Evaluate to true if dir is on the right edge, false otherwise.
 */
#define BUI_DIR_IS_RIGHT(dir) ((dir & BUI_DIR_RIGHT) != 0)

/*
 * Evaluate to true if dir is on the top edge, false otherwise.
 */
#define BUI_DIR_IS_TOP(dir) ((dir & BUI_DIR_TOP) != 0)

/*
 * Evaluate to true if dir is on the bottom edge, false otherwise.
 */
#define BUI_DIR_IS_BOTTOM(dir) ((dir & BUI_DIR_BOTTOM) != 0)

/*
 * Evaluate to true if dir is horizontally centered, false otherwise.
 */
#define BUI_DIR_IS_HTL_CENTER(dir) ((dir & (BUI_DIR_LEFT | BUI_DIR_RIGHT)) == 0)

/*
 * Evaluate to true if dir is vertically centered, false otherwise.
 */
#define BUI_DIR_IS_VTL_CENTER(dir) ((dir & (BUI_DIR_TOP | BUI_DIR_BOTTOM)) == 0)

#define BUI_DECLARE_BITMAP(name) \
		extern const uint8_t bui_bmp_ ## name ## _w; \
		extern const uint8_t bui_bmp_ ## name ## _h; \
		extern const uint8_t bui_bmp_ ## name ## _bb[]; \
		extern const uint32_t bui_bmp_ ## name ## _plt[]; \
		extern const uint8_t bui_bmp_ ## name ## _bpp;

BUI_DECLARE_BITMAP(icon_check);
#define BUI_BMP_ICON_CHECK \
		((const bui_const_bitmap_t) { \
			.w = bui_bmp_icon_check_w, \
			.h = bui_bmp_icon_check_h, \
			.bb = bui_bmp_icon_check_bb, \
			.plt = bui_bmp_icon_check_plt, \
			.bpp = bui_bmp_icon_check_bpp, \
		})
BUI_DECLARE_BITMAP(icon_cross);
#define BUI_BMP_ICON_CROSS \
		((const bui_const_bitmap_t) { \
			.w = bui_bmp_icon_cross_w, \
			.h = bui_bmp_icon_cross_h, \
			.bb = bui_bmp_icon_cross_bb, \
			.plt = bui_bmp_icon_cross_plt, \
			.bpp = bui_bmp_icon_cross_bpp, \
		})
BUI_DECLARE_BITMAP(icon_left);
#define BUI_BMP_ICON_LEFT \
		((const bui_const_bitmap_t) { \
			.w = bui_bmp_icon_left_w, \
			.h = bui_bmp_icon_left_h, \
			.bb = bui_bmp_icon_left_bb, \
			.plt = bui_bmp_icon_left_plt, \
			.bpp = bui_bmp_icon_left_bpp, \
		})
BUI_DECLARE_BITMAP(icon_right);
#define BUI_BMP_ICON_RIGHT \
		((const bui_const_bitmap_t) { \
			.w = bui_bmp_icon_right_w, \
			.h = bui_bmp_icon_right_h, \
			.bb = bui_bmp_icon_right_bb, \
			.plt = bui_bmp_icon_right_plt, \
			.bpp = bui_bmp_icon_right_bpp, \
		})
BUI_DECLARE_BITMAP(icon_up);
#define BUI_BMP_ICON_UP \
		((const bui_const_bitmap_t) { \
			.w = bui_bmp_icon_up_w, \
			.h = bui_bmp_icon_up_h, \
			.bb = bui_bmp_icon_up_bb, \
			.plt = bui_bmp_icon_up_plt, \
			.bpp = bui_bmp_icon_up_bpp, \
		})
BUI_DECLARE_BITMAP(icon_down);
#define BUI_BMP_ICON_DOWN \
		((const bui_const_bitmap_t) { \
			.w = bui_bmp_icon_down_w, \
			.h = bui_bmp_icon_down_h, \
			.bb = bui_bmp_icon_down_bb, \
			.plt = bui_bmp_icon_down_plt, \
			.bpp = bui_bmp_icon_down_bpp, \
		})
BUI_DECLARE_BITMAP(icon_left_filled);
#define BUI_BMP_ICON_LEFT_FILLED \
		((const bui_const_bitmap_t) { \
			.w = bui_bmp_icon_left_filled_w, \
			.h = bui_bmp_icon_left_filled_h, \
			.bb = bui_bmp_icon_left_filled_bb, \
			.plt = bui_bmp_icon_left_filled_plt, \
			.bpp = bui_bmp_icon_left_filled_bpp, \
		})
BUI_DECLARE_BITMAP(icon_right_filled);
#define BUI_BMP_ICON_RIGHT_FILLED \
		((const bui_const_bitmap_t) { \
			.w = bui_bmp_icon_right_filled_w, \
			.h = bui_bmp_icon_right_filled_h, \
			.bb = bui_bmp_icon_right_filled_bb, \
			.plt = bui_bmp_icon_right_filled_plt, \
			.bpp = bui_bmp_icon_right_filled_bpp, \
		})
BUI_DECLARE_BITMAP(icon_up_filled);
#define BUI_BMP_ICON_UP_FILLED \
		((const bui_const_bitmap_t) { \
			.w = bui_bmp_icon_up_filled_w, \
			.h = bui_bmp_icon_up_filled_h, \
			.bb = bui_bmp_icon_up_filled_bb, \
			.plt = bui_bmp_icon_up_filled_plt, \
			.bpp = bui_bmp_icon_up_filled_bpp, \
		})
BUI_DECLARE_BITMAP(icon_down_filled);
#define BUI_BMP_ICON_DOWN_FILLED \
		((const bui_const_bitmap_t) { \
			.w = bui_bmp_icon_down_filled_w, \
			.h = bui_bmp_icon_down_filled_h, \
			.bb = bui_bmp_icon_down_filled_bb, \
			.plt = bui_bmp_icon_down_filled_plt, \
			.bpp = bui_bmp_icon_down_filled_bpp, \
		})
BUI_DECLARE_BITMAP(icon_plus);
#define BUI_BMP_ICON_PLUS \
		((const bui_const_bitmap_t) { \
			.w = bui_bmp_icon_plus_w, \
			.h = bui_bmp_icon_plus_h, \
			.bb = bui_bmp_icon_plus_bb, \
			.plt = bui_bmp_icon_plus_plt, \
			.bpp = bui_bmp_icon_plus_bpp, \
		})
BUI_DECLARE_BITMAP(icon_less);
#define BUI_BMP_ICON_LESS \
		((const bui_const_bitmap_t) { \
			.w = bui_bmp_icon_less_w, \
			.h = bui_bmp_icon_less_h, \
			.bb = bui_bmp_icon_less_bb, \
			.plt = bui_bmp_icon_less_plt, \
			.bpp = bui_bmp_icon_less_bpp, \
		})
BUI_DECLARE_BITMAP(logo_ledger_mini);
#define BUI_BMP_LOGO_LEDGER_MINI \
		((const bui_const_bitmap_t) { \
			.w = bui_bmp_logo_ledger_mini_w, \
			.h = bui_bmp_logo_ledger_mini_h, \
			.bb = bui_bmp_logo_ledger_mini_bb, \
			.plt = bui_bmp_logo_ledger_mini_plt, \
			.bpp = bui_bmp_logo_ledger_mini_bpp, \
		})
BUI_DECLARE_BITMAP(badge_cross);
#define BUI_BMP_BADGE_CROSS \
		((const bui_const_bitmap_t) { \
			.w = bui_bmp_badge_cross_w, \
			.h = bui_bmp_badge_cross_h, \
			.bb = bui_bmp_badge_cross_bb, \
			.plt = bui_bmp_badge_cross_plt, \
			.bpp = bui_bmp_badge_cross_bpp, \
		})
BUI_DECLARE_BITMAP(badge_dashboard);
#define BUI_BMP_BADGE_DASHBOARD \
		((const bui_const_bitmap_t) { \
			.w = bui_bmp_badge_dashboard_w, \
			.h = bui_bmp_badge_dashboard_h, \
			.bb = bui_bmp_badge_dashboard_bb, \
			.plt = bui_bmp_badge_dashboard_plt, \
			.bpp = bui_bmp_badge_dashboard_bpp, \
		})
BUI_DECLARE_BITMAP(badge_validate);
#define BUI_BMP_BADGE_VALIDATE \
		((const bui_const_bitmap_t) { \
			.w = bui_bmp_badge_validate_w, \
			.h = bui_bmp_badge_validate_h, \
			.bb = bui_bmp_badge_validate_bb, \
			.plt = bui_bmp_badge_validate_plt, \
			.bpp = bui_bmp_badge_validate_bpp, \
		})
BUI_DECLARE_BITMAP(badge_loading);
#define BUI_BMP_BADGE_LOADING \
		((const bui_const_bitmap_t) { \
			.w = bui_bmp_badge_loading_w, \
			.h = bui_bmp_badge_loading_h, \
			.bb = bui_bmp_badge_loading_bb, \
			.plt = bui_bmp_badge_loading_plt, \
			.bpp = bui_bmp_badge_loading_bpp, \
		})
BUI_DECLARE_BITMAP(badge_warning);
#define BUI_BMP_BADGE_WARNING \
		((const bui_const_bitmap_t) { \
			.w = bui_bmp_badge_warning_w, \
			.h = bui_bmp_badge_warning_h, \
			.bb = bui_bmp_badge_warning_bb, \
			.plt = bui_bmp_badge_warning_plt, \
			.bpp = bui_bmp_badge_warning_bpp, \
		})
BUI_DECLARE_BITMAP(badge_install);
#define BUI_BMP_BADGE_INSTALL \
		((const bui_const_bitmap_t) { \
			.w = bui_bmp_badge_install_w, \
			.h = bui_bmp_badge_install_h, \
			.bb = bui_bmp_badge_install_bb, \
			.plt = bui_bmp_badge_install_plt, \
			.bpp = bui_bmp_badge_install_bpp, \
		})
BUI_DECLARE_BITMAP(badge_transaction);
#define BUI_BMP_BADGE_TRANSACTION \
		((const bui_const_bitmap_t) { \
			.w = bui_bmp_badge_transaction_w, \
			.h = bui_bmp_badge_transaction_h, \
			.bb = bui_bmp_badge_transaction_bb, \
			.plt = bui_bmp_badge_transaction_plt, \
			.bpp = bui_bmp_badge_transaction_bpp, \
		})
BUI_DECLARE_BITMAP(badge_bitcoin);
#define BUI_BMP_BADGE_BITCOIN \
		((const bui_const_bitmap_t) { \
			.w = bui_bmp_badge_bitcoin_w, \
			.h = bui_bmp_badge_bitcoin_h, \
			.bb = bui_bmp_badge_bitcoin_bb, \
			.plt = bui_bmp_badge_bitcoin_plt, \
			.bpp = bui_bmp_badge_bitcoin_bpp, \
		})
BUI_DECLARE_BITMAP(badge_ethereum);
#define BUI_BMP_BADGE_ETHEREUM \
		((const bui_const_bitmap_t) { \
			.w = bui_bmp_badge_ethereum_w, \
			.h = bui_bmp_badge_ethereum_h, \
			.bb = bui_bmp_badge_ethereum_bb, \
			.plt = bui_bmp_badge_ethereum_plt, \
			.bpp = bui_bmp_badge_ethereum_bpp, \
		})
BUI_DECLARE_BITMAP(badge_eye);
#define BUI_BMP_BADGE_EYE \
		((const bui_const_bitmap_t) { \
			.w = bui_bmp_badge_eye_w, \
			.h = bui_bmp_badge_eye_h, \
			.bb = bui_bmp_badge_eye_bb, \
			.plt = bui_bmp_badge_eye_plt, \
			.bpp = bui_bmp_badge_eye_bpp, \
		})
BUI_DECLARE_BITMAP(badge_people);
#define BUI_BMP_BADGE_PEOPLE \
		((const bui_const_bitmap_t) { \
			.w = bui_bmp_badge_people_w, \
			.h = bui_bmp_badge_people_h, \
			.bb = bui_bmp_badge_people_bb, \
			.plt = bui_bmp_badge_people_plt, \
			.bpp = bui_bmp_badge_people_bpp, \
		})
BUI_DECLARE_BITMAP(badge_lock);
#define BUI_BMP_BADGE_LOCK \
		((const bui_const_bitmap_t) { \
			.w = bui_bmp_badge_lock_w, \
			.h = bui_bmp_badge_lock_h, \
			.bb = bui_bmp_badge_lock_bb, \
			.plt = bui_bmp_badge_lock_plt, \
			.bpp = bui_bmp_badge_lock_bpp, \
		})
BUI_DECLARE_BITMAP(toggle_on);
#define BUI_BMP_TOGGLE_ON \
		((const bui_const_bitmap_t) { \
			.w = bui_bmp_toggle_on_w, \
			.h = bui_bmp_toggle_on_h, \
			.bb = bui_bmp_toggle_on_bb, \
			.plt = bui_bmp_toggle_on_plt, \
			.bpp = bui_bmp_toggle_on_bpp, \
		})
BUI_DECLARE_BITMAP(toggle_off);
#define BUI_BMP_TOGGLE_OFF \
		((const bui_const_bitmap_t) { \
			.w = bui_bmp_toggle_off_w, \
			.h = bui_bmp_toggle_off_h, \
			.bb = bui_bmp_toggle_off_bb, \
			.plt = bui_bmp_toggle_off_plt, \
			.bpp = bui_bmp_toggle_off_bpp, \
		})
BUI_DECLARE_BITMAP(app_settings);
#define BUI_BMP_APP_SETTINGS \
		((const bui_const_bitmap_t) { \
			.w = bui_bmp_app_settings_w, \
			.h = bui_bmp_app_settings_h, \
			.bb = bui_bmp_app_settings_bb, \
			.plt = bui_bmp_app_settings_plt, \
			.bpp = bui_bmp_app_settings_bpp, \
		})

#undef BUI_DECLARE_BITMAP

/*
 * Return the index of the fist occurrence of the specified color in the specified palette, or -1 if the color is not
 * found.
 *
 * Args:
 *     palette: the pointer to the palette which is an array of colors encoded as ARGB 8888
 *     size: the number of elements in palette; must be <= 256
 *     color: the color to find
 * Returns:
 *     the index of the first occurrence of color in palette, or -1 if the color is not found
 */
int16_t bui_palette_find(const uint32_t *palette, uint16_t size, uint32_t color);

/*
 * Return the index of a color in the specified palette that best matches another color. The alpha channel is ignored.
 *
 * Args:
 *     palette: an array containing colors within which to find a match with color, each element is encoded as RGB 888
 *     size: the number of elements in palette; must be >= 1 and <= 256
 *     color: the color with which to compare all colors in the provided palette, encoded as RGB 888
 * Returns:
 *     the index of the color in palette which most closely matches color
 */
uint8_t bui_palette_find_best(const uint32_t *palette, uint16_t size, uint32_t color);


/*
 * Return the lowest unused color index in the provided bitmap. If there are no more unused color indexes (eg. bmp.bpp
 * is 4 and there are 16 unique color indexes used) then -1 is returned.
 *
 * Args:
 *     bmp: the bitmap
 * Returns:
 *     the lowest unused color index in bitmap, or -1 if there are no more unused color indexes available
 */
int16_t bui_bmp_lowest_unused_index(const bui_const_bitmap_t bmp);

/*
 * Fill the provided bitmap with the specified color. If the resulting colors are not in the bitmap's palette, the
 * nearest colors in the palette are used.
 *
 * Args:
 *     bmp: the bitmap all of whose pixels are to be set to color
 *     color: the color with which to fill the bitmap, encoded as ARGB 8888
 */
void bui_bmp_fill(bui_bitmap_t bmp, uint32_t color);

/*
 * Draw a single pixel onto the provided bitmap. If the coordinates of the pixel are out of bounds of the bitmap,
 * nothing is drawn. If the resulting color is not in the bitmap's palette, the nearest color in the palette is used.
 *
 * Args:
 *     bmp: the bitmap onto which the pixel is to be drawn
 *     x: the x-coordinate of the pixel to be drawn
 *     y: the y-coordinate of the pixel to be drawn
 *     color: the color with which to draw the pixel, encoded as ARGB 8888
 */
void bui_bmp_draw_pixel(bui_bitmap_t bmp, int16_t x, int16_t y, uint32_t color);

/*
 * Initialize / reset a BUI context. The context's display buffer is initially filled with the background color.
 *
 * Args:
 *     ctx: the BUI context to be initialized / reset
 */
void bui_ctx_init(bui_ctx_t *ctx);

/*
 * Send some data contained within the provided BUI context's display buffer to the MCU to be displayed, if there is
 * additional data to be displayed. The data is sent using a Display Status, and as such the MCU must be ready to
 * receive a Status when calling this function. If bui_ctx_is_displayed(ctx) is true, this function always returns
 * false and doesn't send anything to the MCU.
 *
 * Args:
 *     ctx: the BUI context
 * Returns:
 *     true if a Display Status was sent to the MCU, false if nothing was sent
 */
bool bui_ctx_display(bui_ctx_t *ctx);

/*
 * Determine whether or not the provided BUI context has been fully displayed.
 *
 * Args:
 *     ctx: the BUI context
 * Returns:
 *     true if ctx is fully displayed, false otherwise
 */
bool bui_ctx_is_displayed(const bui_ctx_t *ctx);

/*
 * Fill the provided BUI context's display with the specified color. If the resulting colors are not in the display's
 * palette, the nearest colors in the palette are used.
 *
 * Args:
 *     ctx: the BUI context all of whose display's pixels are to be set to color
 *     color: the color with which to fill the display, encoded as ARGB 8888
 */
void bui_ctx_fill(bui_ctx_t *ctx, uint32_t color);

/*
 * Fill a rectangle in the provided BUI context's display with the specified color. Any part of the rectangle out of
 * bounds of the display will not be drawn to. If the specified width or height is 0, nothing is drawn. If the resulting
 * colors are not in the context's palette, the nearest colors in the palette are used.
 *
 * Args:
 *     ctx: the BUI context onto whose display the rectangle is to be drawn
 *     x: the x-coordinate of top-left corner of the destination rectangle
 *     y: the y-coordinate of top-left corner of the destination rectangle
 *     w: the width of the rectangle; must be >= 0
 *     h: the height of the rectangle; must be >= 0
 *     color: the color with which to fill the rectangle, encoded as ARGB 8888
 */
void bui_ctx_fill_rect(bui_ctx_t *ctx, int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color);

/*
 * Draw a single pixel onto the provided BUI context's display. If the coordinates of the pixel are out of bounds of the
 * display, nothing is drawn. If the resulting color is not in the context's palette, the nearest color in the palette
 * is used.
 *
 * Args:
 *     ctx: the BUI context onto whose display the pixel is to be drawn
 *     x: the x-coordinate of the pixel to be drawn
 *     y: the y-coordinate of the pixel to be drawn
 *     color: the color with which to draw the pixel, encoded as ARGB 8888
 */
void bui_ctx_draw_pixel(bui_ctx_t *ctx, int16_t x, int16_t y, uint32_t color);

/*
 * Draw a bitmap onto the provided BUI context's display given a source rectangle on the bitmap's coordinate plane and a
 * destination rectangle on the display's coordinate plane. Any part of the destination rectangle out of bounds of the
 * display will not be drawn. The source rectangle must be entirely within the source bitmap. If the width or height is
 * 0, nothing is drawn. If the resulting colors are not in the context's palette, the nearest colors in the palette are
 * used.
 *
 * Args:
 *     ctx: the BUI context onto whose display the bitmap is to be drawn
 *     bmp: the bitmap to be drawn onto ctx's display
 *     src_x: the x-coordinate of the top-left corner of the source rectangle on or outside of bitmap's coordinate plane
 *     src_y: the y-coordinate of the top-left corner of the source rectangle on or outside of bitmap's coordinate plane
 *     dest_x: the x-coordinate of the top-left corner of the destination rectangle on or outside of ctx's display's
 *             coordinate plane
 *     dest_y: the y-coordinate of the top-left corner of the destination rectangle on or outside of ctx's display's
 *             coordinate plane
 *     w: the width of the source and destination rectangles; must be >= 0
 *     h: the height of the source and destination rectangles; must be >= 0
 */
void bui_ctx_draw_bitmap(bui_ctx_t *ctx, bui_const_bitmap_t bmp, int16_t src_x, int16_t src_y, int16_t dest_x,
		int16_t dest_y, int16_t w, int16_t h);

/*
 * Draw an entire bitmap onto the provided BUI context's display given a destination rectangle on the display's
 * coordinate plane. Any part of the destination rectangle out of bounds of the display will not be drawn. If the
 * resulting colors are not in the context's palette, the nearest colors in the palette are used.
 *
 * Args:
 *     ctx: the BUI context onto whose display the bitmap is to be drawn
 *     bmp: the bitmap to be drawn onto ctx's display
 *     dest_x: the x-coordinate of the top-left corner of the destination rectangle on or outside of ctx's display's
 *             coordinate plane
 *     dest_y: the y-coordinate of the top-left corner of the destination rectangle on or outside of ctx's display's
 *             coordinate plane
 */
void bui_ctx_draw_bitmap_full(bui_ctx_t *ctx, bui_const_bitmap_t bmp, int16_t dest_x, int16_t dest_y);

#endif
