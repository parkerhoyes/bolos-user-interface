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
#define BUI_VER_MINOR 6
#define BUI_VER_PATCH 0

// NOTE: The definition of this struct is considered internal; it may be changed between versions without warning.
typedef struct {
	// The data for the display buffer bitmap (128x32). This is a 2-dimensional bit array (or "bit block") representing
	// the contents of the bitmap. The array is encoded as a sequence of bits, starting at the most significant bit,
	// which is 128 * 32 bits in length, with big-endian byte order. Every 128 bits in the sequence is a row, with 32
	// rows in total. The values of cells in this array (a bit at a specific row and column) correspond to the colors of
	// the pixels at their respective location, except the order of rows and columns are both reversed.
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
	// sequence of bits, starting at the most significant bit, which is w * h bits in length, with big-endian byte
	// order. Every w bits in the sequence is a row, with h rows in total. The values of cells in this array (a bit at a
	// specific row and column) correspond to the colors of the pixels at their respective location, except the order of
	// rows and columns are both reversed.
	uint8_t *bb;
} bui_bitmap_t;

typedef struct {
	// The bitmap width in pixels; this must be > 0
	int16_t w;
	// The bitmap height in pixels; this must be > 0
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

BUI_DECLARE_BITMAP(icon_check);
#define BUI_BITMAP_ICON_CHECK ((bui_const_bitmap_t) { .w = bui_bitmap_icon_check_w, .h = bui_bitmap_icon_check_h, \
		.bb = bui_bitmap_icon_check_bitmap })
BUI_DECLARE_BITMAP(icon_cross);
#define BUI_BITMAP_ICON_CROSS ((bui_const_bitmap_t) { .w = bui_bitmap_icon_cross_w, .h = bui_bitmap_icon_cross_h, \
		.bb = bui_bitmap_icon_cross_bitmap })
BUI_DECLARE_BITMAP(icon_left);
#define BUI_BITMAP_ICON_LEFT ((bui_const_bitmap_t) { .w = bui_bitmap_icon_left_w, .h = bui_bitmap_icon_left_h, \
		.bb = bui_bitmap_icon_left_bitmap })
BUI_DECLARE_BITMAP(icon_right);
#define BUI_BITMAP_ICON_RIGHT ((bui_const_bitmap_t) { .w = bui_bitmap_icon_right_w, .h = bui_bitmap_icon_right_h, \
		.bb = bui_bitmap_icon_right_bitmap })
BUI_DECLARE_BITMAP(icon_up);
#define BUI_BITMAP_ICON_UP ((bui_const_bitmap_t) { .w = bui_bitmap_icon_up_w, .h = bui_bitmap_icon_up_h, \
		.bb = bui_bitmap_icon_up_bitmap })
BUI_DECLARE_BITMAP(icon_down);
#define BUI_BITMAP_ICON_DOWN ((bui_const_bitmap_t) { .w = bui_bitmap_icon_down_w, .h = bui_bitmap_icon_down_h, \
		.bb = bui_bitmap_icon_down_bitmap })
BUI_DECLARE_BITMAP(icon_left_filled);
#define BUI_BITMAP_ICON_LEFT_FILLED ((bui_const_bitmap_t) { .w = bui_bitmap_icon_left_filled_w, \
		.h = bui_bitmap_icon_left_filled_h, .bb = bui_bitmap_icon_left_filled_bitmap })
BUI_DECLARE_BITMAP(icon_right_filled);
#define BUI_BITMAP_ICON_RIGHT_FILLED ((bui_const_bitmap_t) { .w = bui_bitmap_icon_right_filled_w, \
		.h = bui_bitmap_icon_right_filled_h, .bb = bui_bitmap_icon_right_filled_bitmap })
BUI_DECLARE_BITMAP(icon_up_filled);
#define BUI_BITMAP_ICON_UP_FILLED ((bui_const_bitmap_t) { .w = bui_bitmap_icon_up_filled_w, \
		.h = bui_bitmap_icon_up_filled_h, .bb = bui_bitmap_icon_up_filled_bitmap })
BUI_DECLARE_BITMAP(icon_down_filled);
#define BUI_BITMAP_ICON_DOWN_FILLED ((bui_const_bitmap_t) { .w = bui_bitmap_icon_down_filled_w, \
		.h = bui_bitmap_icon_down_filled_h, .bb = bui_bitmap_icon_down_filled_bitmap })
BUI_DECLARE_BITMAP(icon_plus);
#define BUI_BITMAP_ICON_PLUS ((bui_const_bitmap_t) { .w = bui_bitmap_icon_plus_w, .h = bui_bitmap_icon_plus_h, \
		.bb = bui_bitmap_icon_plus_bitmap })
BUI_DECLARE_BITMAP(icon_less);
#define BUI_BITMAP_ICON_LESS ((bui_const_bitmap_t) { .w = bui_bitmap_icon_less_w, .h = bui_bitmap_icon_less_h, \
		.bb = bui_bitmap_icon_less_bitmap })
BUI_DECLARE_BITMAP(logo_ledger_mini);
#define BUI_BITMAP_LOGO_LEDGER_MINI ((bui_const_bitmap_t) { .w = bui_bitmap_logo_ledger_mini_w, \
		.h = bui_bitmap_logo_ledger_mini_h, .bb = bui_bitmap_logo_ledger_mini_bitmap })
BUI_DECLARE_BITMAP(badge_cross);
#define BUI_BITMAP_BADGE_CROSS ((bui_const_bitmap_t) { .w = bui_bitmap_badge_cross_w, .h = bui_bitmap_badge_cross_h, \
		.bb = bui_bitmap_badge_cross_bitmap })
BUI_DECLARE_BITMAP(badge_dashboard);
#define BUI_BITMAP_BADGE_DASHBOARD ((bui_const_bitmap_t) { .w = bui_bitmap_badge_dashboard_w, \
		.h = bui_bitmap_badge_dashboard_h, .bb = bui_bitmap_badge_dashboard_bitmap })
BUI_DECLARE_BITMAP(badge_validate);
#define BUI_BITMAP_BADGE_VALIDATE ((bui_const_bitmap_t) { .w = bui_bitmap_badge_validate_w, \
		.h = bui_bitmap_badge_validate_h, .bb = bui_bitmap_badge_validate_bitmap })
BUI_DECLARE_BITMAP(badge_loading);
#define BUI_BITMAP_BADGE_LOADING ((bui_const_bitmap_t) { .w = bui_bitmap_badge_loading_w, \
		.h = bui_bitmap_badge_loading_h, .bb = bui_bitmap_badge_loading_bitmap })
BUI_DECLARE_BITMAP(badge_warning);
#define BUI_BITMAP_BADGE_WARNING ((bui_const_bitmap_t) { .w = bui_bitmap_badge_warning_w, \
		.h = bui_bitmap_badge_warning_h, .bb = bui_bitmap_badge_warning_bitmap })
BUI_DECLARE_BITMAP(badge_install);
#define BUI_BITMAP_BADGE_INSTALL ((bui_const_bitmap_t) { .w = bui_bitmap_badge_install_w, \
		.h = bui_bitmap_badge_install_h, .bb = bui_bitmap_badge_install_bitmap })
BUI_DECLARE_BITMAP(badge_transaction);
#define BUI_BITMAP_BADGE_TRANSACTION ((bui_const_bitmap_t) { .w = bui_bitmap_badge_transaction_w, \
		.h = bui_bitmap_badge_transaction_h, .bb = bui_bitmap_badge_transaction_bitmap })
BUI_DECLARE_BITMAP(badge_bitcoin);
#define BUI_BITMAP_BADGE_BITCOIN ((bui_const_bitmap_t) { .w = bui_bitmap_badge_bitcoin_w, \
		.h = bui_bitmap_badge_bitcoin_h, .bb = bui_bitmap_badge_bitcoin_bitmap })
BUI_DECLARE_BITMAP(badge_ethereum);
#define BUI_BITMAP_BADGE_ETHEREUM ((bui_const_bitmap_t) { .w = bui_bitmap_badge_ethereum_w, \
		.h = bui_bitmap_badge_ethereum_h, .bb = bui_bitmap_badge_ethereum_bitmap })
BUI_DECLARE_BITMAP(badge_eye);
#define BUI_BITMAP_BADGE_EYE ((bui_const_bitmap_t) { .w = bui_bitmap_badge_eye_w, .h = bui_bitmap_badge_eye_h, \
		.bb = bui_bitmap_badge_eye_bitmap })
BUI_DECLARE_BITMAP(badge_people);
#define BUI_BITMAP_BADGE_PEOPLE ((bui_const_bitmap_t) { .w = bui_bitmap_badge_people_w, \
		.h = bui_bitmap_badge_people_h, .bb = bui_bitmap_badge_people_bitmap })
BUI_DECLARE_BITMAP(badge_lock);
#define BUI_BITMAP_BADGE_LOCK ((bui_const_bitmap_t) { .w = bui_bitmap_badge_lock_w, .h = bui_bitmap_badge_lock_h, \
		.bb = bui_bitmap_badge_lock_bitmap })
BUI_DECLARE_BITMAP(toggle_on);
#define BUI_BITMAP_TOGGLE_ON ((bui_const_bitmap_t) { .w = bui_bitmap_toggle_on_w, .h = bui_bitmap_toggle_on_h, \
		.bb = bui_bitmap_toggle_on_bitmap })
BUI_DECLARE_BITMAP(toggle_off);
#define BUI_BITMAP_TOGGLE_OFF ((bui_const_bitmap_t) { .w = bui_bitmap_toggle_off_w, .h = bui_bitmap_toggle_off_h, \
		.bb = bui_bitmap_toggle_off_bitmap })
BUI_DECLARE_BITMAP(app_settings);
#define BUI_BITMAP_APP_SETTINGS ((bui_const_bitmap_t) { .w = bui_bitmap_app_settings_w, \
		.h = bui_bitmap_app_settings_h, .bb = bui_bitmap_app_settings_bitmap })

#undef BUI_DECLARE_BITMAP

/*
 * Fill the provided bitmap with the specified color.
 *
 * Args:
 *     bm: the bitmap to be filled
 *     color: the color with which to fill the bitmap; true fills with 1 bits and false fills with 0 bits
 */
void bui_bm_fill(bui_bitmap_t bm, bool color);

/*
 * Invert every pixel in the provided bitmap.
 *
 * Args:
 *     bitmap: the bitmap to be inverted
 */
void bui_bm_invert(bui_bitmap_t bm);

/*
 * Set the color of a single pixel in the provided bitmap. If the coordinates of the pixel are out of bounds of the
 * bitmap, the bitmap is not accessed.
 *
 * Args:
 *     bm: the bitmap onto which the pixel is to be drawn
 *     x: the x-coordinate of the pixel to be drawn
 *     y: the y-coordinate of the pixel to be drawn
 *     color: the color to which the pixel is to be set; true corresponds to a 1 bit and false to a 0 bit
 */
void bui_bm_draw_pixel(bui_bitmap_t bm, int16_t x, int16_t y, bool color);

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
 * Fill the provided BUI context's display with the specified color.
 *
 * Args:
 *     ctx: the BUI context whose display is to be filled
 *     color: the color with which to fill the display; true fills with the foreground color and false fills with the
 *            background color
 */
void bui_ctx_fill(bui_ctx_t *ctx, bool color);

/*
 * Fill a rectangle in the provided BUI context's display. Any part of the rectangle out of bounds of the display will
 * not be filled. If the specified width or height is 0, nothing is drawn.
 *
 * Args:
 *     ctx: the BUI context onto whose display the rectangle is to be drawn
 *     x: the x-coordinate of top-left corner of the destination rectangle
 *     y: the y-coordinate of top-left corner of the destination rectangle
 *     w: the width of the rectangle; must be >= 0
 *     h: the height of the rectangle; must be >= 0
 *     color: the color with which to fill the rectangle; true fills with the foreground color and false fills with the
 *            background color
 */
void bui_ctx_fill_rect(bui_ctx_t *ctx, int16_t x, int16_t y, int16_t w, int16_t h, bool color);

/*
 * Set the color of a single pixel in the provided BUI context's display. If the coordinates of the pixel are out of
 * bounds of the display, nothing is drawn.
 *
 * Args:
 *     ctx: the BUI context onto whose display the pixel is to be drawn
 *     x: the x-coordinate of the pixel to be drawn
 *     y: the y-coordinate of the pixel to be drawn
 *     color: the color to which the pixel is to be set; true corresponds to the foreground color and false to the
 *            background color
 */
void bui_ctx_draw_pixel(bui_ctx_t *ctx, int16_t x, int16_t y, bool color);

/*
 * Draw a bitmap onto the provided BUI context's display given a source rectangle on the bitmap's coordinate plane and a
 * destination rectangle on the display's coordinate plane. Any part of the destination rectangle out of bounds of the
 * display will not be drawn. The source rectangle must be entirely within the source bitmap. If the width or height is
 * 0, nothing is drawn. Pixels with a color index of 0 in the source bitmap are drawn as the background color, and
 * pixels with a color index of 1 are drawn as the foreground color.
 *
 * Args:
 *     ctx: the BUI context onto whose display the bitmap is to be drawn
 *     bm: the bitmap to be drawn onto ctx's display
 *     src_x: the x-coordinate of the top-left corner of the source rectangle on or outside of bitmap's coordinate plane
 *     src_y: the y-coordinate of the top-left corner of the source rectangle on or outside of bitmap's coordinate plane
 *     dest_x: the x-coordinate of the top-left corner of the destination rectangle on or outside of ctx's display's
 *             coordinate plane
 *     dest_y: the y-coordinate of the top-left corner of the destination rectangle on or outside of ctx's display's
 *             coordinate plane
 *     w: the width of the source and destination rectangles; must be >= 0
 *     h: the height of the source and destination rectangles; must be >= 0
 */
void bui_ctx_draw_bitmap(bui_ctx_t *ctx, bui_const_bitmap_t bm, int16_t src_x, int16_t src_y, int16_t dest_x,
		int16_t dest_y, int16_t w, int16_t h);

/*
 * Draw an entire bitmap onto the provided BUI context's display given a destination rectangle on the display's
 * coordinate plane. Any part of the destination rectangle out of bounds of the display will not be drawn. Pixels with a
 * color index of 0 in the source bitmap are drawn as the background color, and pixels with a color index of 1 are drawn
 * as the foreground color.
 *
 * Args:
 *     ctx: the BUI context onto whose display the bitmap is to be drawn
 *     bm: the bitmap to be drawn onto ctx's display
 *     dest_x: the x-coordinate of the top-left corner of the destination rectangle on or outside of ctx's display's
 *             coordinate plane
 *     dest_y: the y-coordinate of the top-left corner of the destination rectangle on or outside of ctx's display's
 *             coordinate plane
 */
void bui_ctx_draw_bitmap_full(bui_ctx_t *ctx, bui_const_bitmap_t bm, int16_t dest_x, int16_t dest_y);

/*
 * Draw a masked bitmap onto the provided BUI context's display given a source rectangle on the bitmap's coordinate
 * plane and a destination rectangle on the display's coordinate plane. Any part of the destination rectangle out of
 * bounds of the display will not be drawn. The source rectangle must be entirely within the source bitmap. If the width
 * or height is 0, nothing is drawn. Pixels with a color index of 0 in the source bitmap are interpreted as transparent,
 * and pixels with a color index of 1 are drawn as the foreground color.
 *
 * Args:
 *     ctx: the BUI context onto whose display the bitmap is to be drawn
 *     bm: the bitmap to be drawn onto ctx's display
 *     src_x: the x-coordinate of the top-left corner of the source rectangle on or outside of bitmap's coordinate plane
 *     src_y: the y-coordinate of the top-left corner of the source rectangle on or outside of bitmap's coordinate plane
 *     dest_x: the x-coordinate of the top-left corner of the destination rectangle on or outside of ctx's display's
 *             coordinate plane
 *     dest_y: the y-coordinate of the top-left corner of the destination rectangle on or outside of ctx's display's
 *             coordinate plane
 *     w: the width of the source and destination rectangles; must be >= 0
 *     h: the height of the source and destination rectangles; must be >= 0
 */
void bui_ctx_draw_mbitmap(bui_ctx_t *ctx, bui_const_bitmap_t bm, int16_t src_x, int16_t src_y, int16_t dest_x,
		int16_t dest_y, int16_t w, int16_t h);

/*
 * Draw an entire masked bitmap onto the provided BUI context's display given a destination rectangle on the display's
 * coordinate plane. Any part of the destination rectangle out of bounds of the display will not be drawn. Pixels with a
 * color index of 0 in the source bitmap are interpreted as transparent, and pixels with a color index of 1 are drawn as
 * the foreground color.
 *
 * Args:
 *     ctx: the BUI context onto whose display the bitmap is to be drawn
 *     bm: the bitmap to be drawn onto ctx's display
 *     dest_x: the x-coordinate of the top-left corner of the destination rectangle on or outside of ctx's display's
 *             coordinate plane
 *     dest_y: the y-coordinate of the top-left corner of the destination rectangle on or outside of ctx's display's
 *             coordinate plane
 */
void bui_ctx_draw_mbitmap_full(bui_ctx_t *ctx, bui_const_bitmap_t bm, int16_t dest_x, int16_t dest_y);

#endif
