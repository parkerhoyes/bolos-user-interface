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

/*
 * NOTICE: This file contains work derived from source code originally subject
 * to the copyright of Ledger SAS and distributed under the terms of the Apache
 * License, Version 2.0 (which may be found here:
 * http://www.apache.org/licenses/LICENSE-2.0). The original work from which
 * this code was derived may be found here:
 * https://github.com/LedgerHQ/nanos-nonsecure-firmware/tree/master/bagl/src
 */

const uint8_t bui_bmp_icon_check_w = 8;
const uint8_t bui_bmp_icon_check_h = 6;
const uint8_t bui_bmp_icon_check_bb[] = {
	0x04, 0x0A, 0x11, 0x20, 0x40, 0x80,
};
const uint32_t bui_bmp_icon_check_plt[] = {
	0xFF000000,
	0xFFFFFFFF,
};
const uint8_t bui_bmp_icon_check_bpp = 1;

const uint8_t bui_bmp_icon_cross_w = 7;
const uint8_t bui_bmp_icon_cross_h = 7;
const uint8_t bui_bmp_icon_cross_bb[] = {
	0x82, 0x88, 0xA0, 0x82, 0x88, 0xA0, 0x80,
};
const uint32_t bui_bmp_icon_cross_plt[] = {
	0xFF000000,
	0xFFFFFFFF,
};
const uint8_t bui_bmp_icon_cross_bpp = 1;

const uint8_t bui_bmp_icon_left_w = 4;
const uint8_t bui_bmp_icon_left_h = 7;
const uint8_t bui_bmp_icon_left_bb[] = {
	0x84, 0x21, 0x24, 0x80,
};
const uint32_t bui_bmp_icon_left_plt[] = {
	0xFF000000,
	0xFFFFFFFF,
};
const uint8_t bui_bmp_icon_left_bpp = 1;

const uint8_t bui_bmp_icon_right_w = 4;
const uint8_t bui_bmp_icon_right_h = 7;
const uint8_t bui_bmp_icon_right_bb[] = {
	0x12, 0x48, 0x42, 0x10,
};
const uint32_t bui_bmp_icon_right_plt[] = {
	0xFF000000,
	0xFFFFFFFF,
};
const uint8_t bui_bmp_icon_right_bpp = 1;

const uint8_t bui_bmp_icon_up_w = 7;
const uint8_t bui_bmp_icon_up_h = 4;
const uint8_t bui_bmp_icon_up_bb[] = {
	0x82, 0x88, 0xA0, 0x80,
};
const uint32_t bui_bmp_icon_up_plt[] = {
	0xFF000000,
	0xFFFFFFFF,
};
const uint8_t bui_bmp_icon_up_bpp = 1;

const uint8_t bui_bmp_icon_down_w = 7;
const uint8_t bui_bmp_icon_down_h = 4;
const uint8_t bui_bmp_icon_down_bb[] = {
	0x10, 0x51, 0x14, 0x10,
};
const uint32_t bui_bmp_icon_down_plt[] = {
	0xFF000000,
	0xFFFFFFFF,
};
const uint8_t bui_bmp_icon_down_bpp = 1;

const uint8_t bui_bmp_icon_left_filled_w = 4;
const uint8_t bui_bmp_icon_left_filled_h = 7;
const uint8_t bui_bmp_icon_left_filled_bb[] = {
	0x8C, 0xEF, 0xEC, 0x80,
};
const uint32_t bui_bmp_icon_left_filled_plt[] = {
	0xFF000000,
	0xFFFFFFFF,
};
const uint8_t bui_bmp_icon_left_filled_bpp = 1;

const uint8_t bui_bmp_icon_right_filled_w = 4;
const uint8_t bui_bmp_icon_right_filled_h = 7;
const uint8_t bui_bmp_icon_right_filled_bb[] = {
	0x13, 0x7F, 0x73, 0x10,
};
const uint32_t bui_bmp_icon_right_filled_plt[] = {
	0xFF000000,
	0xFFFFFFFF,
};
const uint8_t bui_bmp_icon_right_filled_bpp = 1;

const uint8_t bui_bmp_icon_up_filled_w = 7;
const uint8_t bui_bmp_icon_up_filled_h = 4;
const uint8_t bui_bmp_icon_up_filled_bb[] = {
	0xFE, 0xF8, 0xE0, 0x80,
};
const uint32_t bui_bmp_icon_up_filled_plt[] = {
	0xFF000000,
	0xFFFFFFFF,
};
const uint8_t bui_bmp_icon_up_filled_bpp = 1;

const uint8_t bui_bmp_icon_down_filled_w = 7;
const uint8_t bui_bmp_icon_down_filled_h = 4;
const uint8_t bui_bmp_icon_down_filled_bb[] = {
	0x10, 0x71, 0xF7, 0xF0,
};
const uint32_t bui_bmp_icon_down_filled_plt[] = {
	0xFF000000,
	0xFFFFFFFF,
};
const uint8_t bui_bmp_icon_down_filled_bpp = 1;

const uint8_t bui_bmp_icon_plus_w = 7;
const uint8_t bui_bmp_icon_plus_h = 7;
const uint8_t bui_bmp_icon_plus_bb[] = {
	0x08, 0x04, 0xE2, 0x8F, 0x40, 0x20, 0x00,
};
const uint32_t bui_bmp_icon_plus_plt[] = {
	0xFF000000,
	0xFFFFFFFF,
};
const uint8_t bui_bmp_icon_plus_bpp = 1;

const uint8_t bui_bmp_icon_less_w = 6;
const uint8_t bui_bmp_icon_less_h = 1;
const uint8_t bui_bmp_icon_less_bb[] = {
	0xFF,
};
const uint32_t bui_bmp_icon_less_plt[] = {
	0xFF000000,
	0xFFFFFFFF,
};
const uint8_t bui_bmp_icon_less_bpp = 1;

const uint8_t bui_bmp_logo_ledger_mini_w = 16;
const uint8_t bui_bmp_logo_ledger_mini_h = 16;
const uint8_t bui_bmp_logo_ledger_mini_bb[] = {
	0x33, 0xCC, 0x73, 0xCE, 0xF3, 0xCF, 0xF3, 0xCF,
	0x00, 0x00, 0x00, 0x00, 0xFF, 0xCF, 0xFF, 0xCF,
	0xFF, 0xCF, 0xFF, 0xCF, 0xFF, 0xC0, 0xFF, 0xC0,
	0xFF, 0xCF, 0xFF, 0xCF, 0x7F, 0xCE, 0x3F, 0xCC,
};
const uint32_t bui_bmp_logo_ledger_mini_plt[] = {
	0xFF000000,
	0xFFFFFFFF,
};
const uint8_t bui_bmp_logo_ledger_mini_bpp = 1;

const uint8_t bui_bmp_badge_cross_w = 14;
const uint8_t bui_bmp_badge_cross_h = 14;
const uint8_t bui_bmp_badge_cross_bb[] = {
	0x07, 0x80, 0x7F, 0x83, 0xFF, 0x1D, 0xEE, 0x63,
	0x1B, 0xC0, 0xFF, 0x87, 0xFE, 0x1F, 0xF0, 0x3D,
	0x8C, 0x67, 0x7B, 0x8F, 0xFC, 0x1F, 0xE0, 0x1E,
	0x0F,
};
const uint32_t bui_bmp_badge_cross_plt[] = {
	0xFF000000,
	0xFFFFFFFF,
};
const uint8_t bui_bmp_badge_cross_bpp = 1;

const uint8_t bui_bmp_badge_dashboard_w = 14;
const uint8_t bui_bmp_badge_dashboard_h = 14;
const uint8_t bui_bmp_badge_dashboard_bb[] = {
	0x07, 0x80, 0x7F, 0x83, 0xFF, 0x1C, 0x0E, 0x60,
	0x1B, 0x9E, 0x7E, 0x79, 0xF9, 0xE7, 0xE7, 0x9D,
	0x80, 0x67, 0x03, 0x8F, 0xFC, 0x1F, 0xE0, 0x1E,
	0x0F,
};
const uint32_t bui_bmp_badge_dashboard_plt[] = {
	0xFF000000,
	0xFFFFFFFF,
};
const uint8_t bui_bmp_badge_dashboard_bpp = 1;

const uint8_t bui_bmp_badge_validate_w = 14;
const uint8_t bui_bmp_badge_validate_h = 14;
const uint8_t bui_bmp_badge_validate_bb[] = {
	0x07, 0x80, 0x7F, 0x83, 0xFF, 0x1F, 0x9E, 0x7C,
	0x3B, 0xE0, 0x7F, 0x18, 0xF8, 0xF7, 0xC7, 0xFD,
	0xBF, 0xE7, 0xFF, 0x8F, 0xFC, 0x1F, 0xE0, 0x1E,
	0x0F,
};
const uint32_t bui_bmp_badge_validate_plt[] = {
	0xFF000000,
	0xFFFFFFFF,
};
const uint8_t bui_bmp_badge_validate_bpp = 1;

const uint8_t bui_bmp_badge_loading_w = 14;
const uint8_t bui_bmp_badge_loading_h = 14;
const uint8_t bui_bmp_badge_loading_bb[] = {
	0x03, 0x00, 0x0C, 0x03, 0x03, 0x0C, 0x0C, 0x00,
	0x00, 0x00, 0x0C, 0x00, 0xF0, 0x03, 0x00, 0x00,
	0x00, 0x00, 0x03, 0x00, 0x0C, 0x03, 0x00, 0x0C,
	0x00,
};
const uint32_t bui_bmp_badge_loading_plt[] = {
	0xFF000000,
	0xFFFFFFFF,
};
const uint8_t bui_bmp_badge_loading_bpp = 1;

const uint8_t bui_bmp_badge_warning_w = 14;
const uint8_t bui_bmp_badge_warning_h = 14;
const uint8_t bui_bmp_badge_warning_bb[] = {
	0x07, 0x80, 0x7F, 0x83, 0xFF, 0x1F, 0x3E, 0x7C,
	0xFB, 0xFF, 0xFF, 0xCF, 0xFF, 0x3F, 0xFC, 0xFD,
	0xF3, 0xE7, 0xCF, 0x8F, 0xFC, 0x1F, 0xE0, 0x1E,
	0x0F,
};
const uint32_t bui_bmp_badge_warning_plt[] = {
	0xFF000000,
	0xFFFFFFFF,
};
const uint8_t bui_bmp_badge_warning_bpp = 1;

const uint8_t bui_bmp_badge_install_w = 14;
const uint8_t bui_bmp_badge_install_h = 14;
const uint8_t bui_bmp_badge_install_bb[] = {
	0x07, 0x80, 0x7F, 0x83, 0xFF, 0x1C, 0x0E, 0x70,
	0x3B, 0xFF, 0xFF, 0xCF, 0xFE, 0x1F, 0xF0, 0x3D,
	0xF3, 0xE7, 0xCF, 0x8F, 0x3C, 0x1F, 0xE0, 0x1E,
	0x0F,
};
const uint32_t bui_bmp_badge_install_plt[] = {
	0xFF000000,
	0xFFFFFFFF,
};
const uint8_t bui_bmp_badge_install_bpp = 1;

const uint8_t bui_bmp_badge_transaction_w = 14;
const uint8_t bui_bmp_badge_transaction_h = 14;
const uint8_t bui_bmp_badge_transaction_bb[] = {
	0x07, 0x80, 0x7F, 0x83, 0xF7, 0x1F, 0xCE, 0x60,
	0x1B, 0x80, 0x7F, 0xB3, 0xFC, 0xDF, 0xE0, 0x1D,
	0x80, 0x67, 0x3F, 0x8E, 0xFC, 0x1F, 0xE0, 0x1E,
	0x0F,
};
const uint32_t bui_bmp_badge_transaction_plt[] = {
	0xFF000000,
	0xFFFFFFFF,
};
const uint8_t bui_bmp_badge_transaction_bpp = 1;

const uint8_t bui_bmp_badge_bitcoin_w = 14;
const uint8_t bui_bmp_badge_bitcoin_h = 14;
const uint8_t bui_bmp_badge_bitcoin_bb[] = {
	0x07, 0x80, 0x7F, 0x83, 0xD7, 0x1C, 0x06, 0x63,
	0x3B, 0x9C, 0xFE, 0x33, 0xFC, 0x0F, 0xF3, 0x3D,
	0xCC, 0xE7, 0x81, 0x8F, 0x5C, 0x1F, 0xE0, 0x1E,
	0x0F,
};
const uint32_t bui_bmp_badge_bitcoin_plt[] = {
	0xFF000000,
	0xFFFFFFFF,
};
const uint8_t bui_bmp_badge_bitcoin_bpp = 1;

const uint8_t bui_bmp_badge_ethereum_w = 14;
const uint8_t bui_bmp_badge_ethereum_h = 14;
const uint8_t bui_bmp_badge_ethereum_bb[] = {
	0x07, 0x80, 0x7F, 0x83, 0xCF, 0x1E, 0x1E, 0x78,
	0x7B, 0xDE, 0xFF, 0x87, 0xFC, 0x0F, 0xF8, 0x7D,
	0xE1, 0xE7, 0xCF, 0x8F, 0x3C, 0x1F, 0xE0, 0x1E,
	0x0F,
};
const uint32_t bui_bmp_badge_ethereum_plt[] = {
	0xFF000000,
	0xFFFFFFFF,
};
const uint8_t bui_bmp_badge_ethereum_bpp = 1;

const uint8_t bui_bmp_badge_eye_w = 14;
const uint8_t bui_bmp_badge_eye_h = 14;
const uint8_t bui_bmp_badge_eye_bb[] = {
	0x07, 0x80, 0x7F, 0x83, 0xFF, 0x1F, 0xFE, 0x78,
	0x7B, 0x80, 0x7C, 0x30, 0xF0, 0xC3, 0xE0, 0x1D,
	0xE1, 0xE7, 0xFF, 0x8F, 0xFC, 0x1F, 0xE0, 0x1E,
	0x0F,
};
const uint32_t bui_bmp_badge_eye_plt[] = {
	0xFF000000,
	0xFFFFFFFF,
};
const uint8_t bui_bmp_badge_eye_bpp = 1;

const uint8_t bui_bmp_badge_people_w = 14;
const uint8_t bui_bmp_badge_people_h = 14;
const uint8_t bui_bmp_badge_people_bb[] = {
	0x07, 0x80, 0x7F, 0x83, 0xFF, 0x1C, 0x0E, 0x70,
	0x3B, 0xC0, 0xFF, 0x87, 0xFF, 0xFF, 0xFC, 0xFD,
	0xE1, 0xE7, 0xCF, 0x8F, 0xFC, 0x1F, 0xE0, 0x1E,
	0x0F,
};
const uint32_t bui_bmp_badge_people_plt[] = {
	0xFF000000,
	0xFFFFFFFF,
};
const uint8_t bui_bmp_badge_people_bpp = 1;

const uint8_t bui_bmp_badge_lock_w = 14;
const uint8_t bui_bmp_badge_lock_h = 14;
const uint8_t bui_bmp_badge_lock_bb[] = {
	0x07, 0x80, 0x7F, 0x83, 0xFF, 0x18, 0x06, 0x60,
	0x1B, 0x80, 0x7E, 0x01, 0xF8, 0x07, 0xF3, 0x3D,
	0xCC, 0xE7, 0x03, 0x8E, 0x1C, 0x1F, 0xE0, 0x1E,
	0x00,
};
const uint32_t bui_bmp_badge_lock_plt[] = {
	0xFF000000,
	0xFFFFFFFF,
};
const uint8_t bui_bmp_badge_lock_bpp = 1;

const uint8_t bui_bmp_toggle_on_w = 16;
const uint8_t bui_bmp_toggle_on_h = 10;
const uint8_t bui_bmp_toggle_on_bb[] = {
	0x1F, 0xF8, 0x20, 0x04, 0x4C, 0x02, 0x9E, 0x01,
	0xBF, 0x01, 0xBF, 0x01, 0x9E, 0x01, 0x4C, 0x02,
	0x20, 0x04, 0x1F, 0xF8,
};
const uint32_t bui_bmp_toggle_on_plt[] = {
	0xFF000000,
	0xFFFFFFFF,
};
const uint8_t bui_bmp_toggle_on_bpp = 1;

const uint8_t bui_bmp_toggle_off_w = 16;
const uint8_t bui_bmp_toggle_off_h = 10;
const uint8_t bui_bmp_toggle_off_bb[] = {
	0x1F, 0xF8, 0x20, 0x04, 0x40, 0x32, 0x80, 0x79,
	0x80, 0xFD, 0x80, 0xFD, 0x80, 0x79, 0x40, 0x32,
	0x20, 0x04, 0x1F, 0xF8,
};
const uint32_t bui_bmp_toggle_off_plt[] = {
	0xFF000000,
	0xFFFFFFFF,
};
const uint8_t bui_bmp_toggle_off_bpp = 1;

const uint8_t bui_bmp_app_settings_w = 16;
const uint8_t bui_bmp_app_settings_h = 16;
const uint8_t bui_bmp_app_settings_bb[] = {
	0x00, 0x00, 0x0F, 0xF0, 0x3F, 0xFC, 0x3E, 0x7C,
	0x7A, 0x5E, 0x70, 0x0E, 0x78, 0x1E, 0x61, 0x86,
	0x61, 0x86, 0x78, 0x1E, 0x70, 0x0E, 0x7A, 0x5E,
	0x3E, 0x7C, 0x3F, 0xFC, 0x0F, 0xF0, 0x00, 0x00,
};
const uint32_t bui_bmp_app_settings_plt[] = {
	0xFF000000,
	0xFFFFFFFF,
};
const uint8_t bui_bmp_app_settings_bpp = 1;
