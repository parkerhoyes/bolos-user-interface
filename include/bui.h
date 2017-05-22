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

typedef uint8_t bui_button_id_t;

#define BUI_BUTTON_NANOS_NONE  ((bui_button_id_t) 0x00)
#define BUI_BUTTON_NANOS_LEFT  ((bui_button_id_t) 0x01)
#define BUI_BUTTON_NANOS_RIGHT ((bui_button_id_t) 0x02)
#define BUI_BUTTON_NANOS_BOTH  ((bui_button_id_t) (BUI_BUTTON_NANOS_LEFT | BUI_BUTTON_NANOS_RIGHT))

typedef enum {
	// The button is not currently pressed
	BUI_BUTTON_STATE_RELEASED = 0x01,
	// The button is currently pressed, but hasn't been for a very long time
	BUI_BUTTON_STATE_PRESSED = 0x02,
	// The button is currently pressed, and has been for a long time (longer than would be considered a "click")
	BUI_BUTTON_STATE_HELD = 0x03,
} bui_button_state_t;

#define BUI_BUTTON_STATE_IS_PRESSED(state) ((state & 0x02) == 0x02)

typedef enum {
	// Associated data: none
	BUI_EVENT_DISPLAYED = 1,
	// Associated data: bui_event_data_time_elapsed_t
	BUI_EVENT_TIME_ELAPSED = 2,
	// Associated data: bui_event_data_button_pressed_t
	BUI_EVENT_BUTTON_PRESSED = 3,
	// Associated data: bui_event_data_button_released_t
	BUI_EVENT_BUTTON_RELEASED = 4,
	// Associated data: bui_event_data_button_clicked_t
	BUI_EVENT_BUTTON_CLICKED = 5,
	// Associated data: bui_event_data_button_held_t
	BUI_EVENT_BUTTON_HELD = 6,
} bui_event_id_t;

typedef struct {
	// The time elapsed since the last event of the same type, or since the BUI context was initialized (whichever was
	// most recent), in milliseconds; always > 0
	uint32_t elapsed;
} bui_event_data_time_elapsed_t;

typedef struct {
	// The button pressed; either BUI_BUTTON_NANOS_LEFT or BUI_BUTTON_NANOS_RIGHT
	bui_button_id_t button;
} bui_event_data_button_pressed_t;

typedef struct {
	// The button released; either BUI_BUTTON_NANOS_LEFT or BUI_BUTTON_NANOS_RIGHT
	bui_button_id_t button;
	// The state of the button before it was released; either BUI_BUTTON_STATE_PRESSED or BUI_BUTTON_STATE_HELD
	bui_button_state_t prev_state;
} bui_event_data_button_released_t;

typedef struct {
	// The button clicked; one of BUI_BUTTON_NANOS_LEFT, BUI_BUTTON_NANOS_RIGHT, or BUI_BUTTON_NANOS_BOTH
	bui_button_id_t button;
} bui_event_data_button_clicked_t;

typedef struct {
	// The button held; one of BUI_BUTTON_NANOS_LEFT or BUI_BUTTON_NANOS_RIGHT
	bui_button_id_t button;
} bui_event_data_button_held_t;

#define BUI_EVENT_DATA_TIME_ELAPSED(event) ((const bui_event_data_time_elapsed_t*) (event)->data)
#define BUI_EVENT_DATA_BUTTON_PRESSED(event) ((const bui_event_data_button_pressed_t*) (event)->data)
#define BUI_EVENT_DATA_BUTTON_RELEASED(event) ((const bui_event_data_button_released_t*) (event)->data)
#define BUI_EVENT_DATA_BUTTON_CLICKED(event) ((const bui_event_data_button_clicked_t*) (event)->data)
#define BUI_EVENT_DATA_BUTTON_HELD(event) ((const bui_event_data_button_held_t*) (event)->data)

typedef struct {
	// The ID of the event
	bui_event_id_t id;
	// A pointer to extra data associated with the event, or NULL if there is no such data
	const void *data;
} bui_event_t;

typedef struct bui_ctx_t_ bui_ctx_t;

/*
 * Handle an event that has occurred in the specified BUI context. This function may also be NULL if no action is to be
 * performed. This pointer may be a pointer to NVRAM determined at link-time, in which case it must be passed through
 * PIC(...) to translate it to a valid address at runtime.
 *
 * Args:
 *     ctx: the BUI context in which the event has occurred
 *     event: the event to be handled
 */
typedef void (*bui_event_handler_t)(bui_ctx_t *ctx, const bui_event_t *event);

// NOTE: The definition of this struct is considered internal; it may be changed between versions without warning.
struct bui_ctx_t_ {
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
	// The ticker interval, in milliseconds; always in [10, 10000]
	uint16_t ticker_interval;
	// Called whenever a new BUI event occurs (if not NULL)
	bui_event_handler_t event_handler;
	// True if the left button is currently pressed, false otherwise
	bool button_left : 1;
	// The duration, in milliseconds, for which the left button has been in its current state (pressed / released);
	// maximum of 2^10-1
	uint16_t button_left_duration : 10;
	// A representation of the duration for which the left button was in its previous state (pressed / released). 0
	// indicates it was in the range of [0, BUI_BUTTON_FAST_THRESHOLD) ms, 1 indicates it was in the range of
	// [BUI_BUTTON_FAST_THRESHOLD, BUI_BUTTON_CLICK_THRESHOLD) ms, and 2 indicates it was greater than or equal to
	// BUI_BUTTON_CLICK_THRESHOLD ms.
	uint8_t button_left_prev : 2;
	// True if the left button is released, and has already triggered a click event because it was released, or false
	// otherwise
	bool button_left_clicked : 1;
	// True if the right button is currently pressed, false otherwise
	bool button_right : 1;
	// The duration, in milliseconds, for which the right button has been in its current state (pressed / released);
	// maximum of 2^10-1
	uint16_t button_right_duration : 10;
	// A representation of the duration for which the right button was in its previous state (pressed / released). 0
	// indicates it was in the range of [0, BUI_BUTTON_FAST_THRESHOLD) ms, 1 indicates it was in the range of
	// [BUI_BUTTON_FAST_THRESHOLD, BUI_BUTTON_CLICK_THRESHOLD) ms, and 2 indicates it was greater than or equal to
	// BUI_BUTTON_CLICK_THRESHOLD ms.
	uint8_t button_right_prev : 2;
	// True if the right button is released, and has already triggered a click event because it was released, or false
	// otherwise
	bool button_right_clicked : 1;
};

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

typedef uint8_t bui_dir_t;

#define BUI_DIR_CENTER       ((bui_dir_t) 0b00000000)
#define BUI_DIR_LEFT         ((bui_dir_t) 0b00000001)
#define BUI_DIR_RIGHT        ((bui_dir_t) 0b00000010)
#define BUI_DIR_TOP          ((bui_dir_t) 0b00000100)
#define BUI_DIR_BOTTOM       ((bui_dir_t) 0b00001000)
#define BUI_DIR_LEFT_TOP     ((bui_dir_t) (BUI_DIR_LEFT | BUI_DIR_TOP))
#define BUI_DIR_LEFT_BOTTOM  ((bui_dir_t) (BUI_DIR_LEFT | BUI_DIR_BOTTOM))
#define BUI_DIR_RIGHT_TOP    ((bui_dir_t) (BUI_DIR_RIGHT | BUI_DIR_TOP))
#define BUI_DIR_RIGHT_BOTTOM ((bui_dir_t) (BUI_DIR_RIGHT | BUI_DIR_BOTTOM))

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
 * Initialize / reset a BUI context for the Ledger Nano S. The context's display buffer is initially filled with the
 * background color and the ticker interval is set to 40 ms. The MCU must be ready to receive a command when this
 * function is called.
 *
 * Args:
 *     ctx: the BUI context to be initialized / reset
 */
void bui_ctx_init(bui_ctx_t *ctx);

/*
 * Display content waiting within the BUI context's display buffer onto the device's screen by sending a display status
 * over SEPROXYHAL. BUI will attempt to only flush the display buffer when this function is called; however, it may have
 * to do so at other times for reasons including memory constraints on the SE. When the buffer is completely flushed,
 * bui_ctx_is_displayed(...) will return true and a BUI_EVENT_DISPLAYED event will be dispatched. If this function is
 * called when the display buffer is in the process of being flushed (bui_ctx_is_displayed(...) returns false), then
 * this function has no side effects. When this function is called, the MCU must be ready to receive a status (unless
 * bui_ctx_is_displayed(...) is false).
 *
 * Args:
 *     ctx: the BUI context
 * Returns:
 *     true if a display status was sent as a result of the call to this function, false if no status was sent
 */
bool bui_ctx_display(bui_ctx_t *ctx);

/*
 * Get the ticker interval in the specified BUI context.
 *
 * Args:
 *     ctx: the BUI context
 * Returns:
 *     the ticker interval, in milliseconds; in the range [10, 10000]
 */
uint16_t bui_ctx_get_ticker(bui_ctx_t *ctx);

/*
 * Set the specified BUI context's ticker interval.
 *
 * Args:
 *     ctx: the BUI context
 *     interval: the desired ticker interval, in milliseconds; must be in [10, 10000]
 */
void bui_ctx_set_ticker(bui_ctx_t *ctx, uint16_t interval);

/*
 * Set (or unset) the event handler associated with the given BUI context.
 *
 * Args:
 *     ctx: the BUI context
 *     event_handler: the event handler, or NULL to unset the BUI context's event handler
 */
void bui_ctx_set_event_handler(bui_ctx_t *ctx, bui_event_handler_t event_handler);

/*
 * Handle a SEPROXYHAL event sent to the SE by the MCU. This function may or may not send commands and / or a status in
 * return.
 *
 * Args:
 *     ctx: the BUI context to be notified of the SEPROXYHAL event
 *     allow_status: true if a status may be sent to the MCU by this function in response to the event, false otherwise;
 *                   always passing false as this parameter may prevent BUI from functioning properly, so it should be
 *                   done rarely
 * Returns:
 *     true if a status was sent to the MCU, false otherwise
 */
bool bui_ctx_seproxyhal_event(bui_ctx_t *ctx, bool allow_status);

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
 * Determine the current state of a button.
 *
 * Args:
 *     ctx: the BUI context
 *     button: the button; must be BUI_BUTTON_NANOS_LEFT or BUI_BUTTON_NANOS_RIGHT
 * Returns:
 *     one of BUI_BUTTON_STATE_RELEASED, BUI_BUTTON_STATE_PRESSED, or BUI_BUTTON_STATE_HELD
 */
bui_button_state_t bui_ctx_get_button(const bui_ctx_t *ctx, bui_button_id_t button);

/*
 * Dispatch an event in the provided BUI context.
 *
 * Args:
 *     ctx: the BUI context
 *     event: the event to be dispatched
 */
void bui_ctx_dispatch_event(bui_ctx_t *ctx, const bui_event_t *event);

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
