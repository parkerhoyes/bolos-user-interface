/*
 * License for the BOLOS User Interface Library project, originally found here:
 * https://github.com/parkerhoyes/bolos-user-interface
 *
 * Copyright (C) 2017 Parker Hoyes <contact@parkerhoyes.com>
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

#include "bui_room.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "os.h"

#include "bui.h"
#include "bui_font.h"

_Static_assert(sizeof(void*) == 4, "sizeof(void*) must be 4");
_Static_assert(sizeof(uint8_t) == 1, "sizeof(uint8_t) must be 1");
_Static_assert(sizeof(uint16_t) == 2, "sizeof(uint16_t) must be 2");

#define BUI_ROOM_PAD(ptr) ((4 - ((uintptr_t) (ptr) & 0x3)) & 0x3)

void bui_room_ctx_init(bui_room_ctx_t *ctx, void *stack, const bui_room_t *room, const void *args, uint16_t args_size) {
	stack = (uint8_t*) stack + BUI_ROOM_PAD(stack);
	*((const bui_room_t**) stack) = (const bui_room_t*) PIC(room);
	stack = (const bui_room_t**) stack + 1;
	ctx->stack_ptr = stack;
	ctx->frame_ptr = stack;
	if (args_size != 0)
		bui_room_push(ctx, args, args_size);
	{
		bui_room_event_data_enter_t data = { .up = true };
		bui_room_event_t event = { .id = BUI_ROOM_EVENT_ENTER, .data = &data };
		bui_room_dispatch_event(ctx, &event);
	}
}

void bui_room_enter(bui_room_ctx_t *ctx, const bui_room_t *room, const void *args, uint16_t args_size) {
	{
		bui_room_event_data_exit_t data = { .up = true };
		bui_room_event_t event = { .id = BUI_ROOM_EVENT_EXIT, .data = &data };
		bui_room_dispatch_event(ctx, &event);
	}
	uint8_t pad = BUI_ROOM_PAD(ctx->stack_ptr + 3);
	uint16_t frame_size = (uint8_t*) ctx->stack_ptr - (uint8_t*) ctx->frame_ptr;
	ctx->stack_ptr = (uint8_t*) ctx->stack_ptr + pad;
	*((uint8_t*) ctx->stack_ptr) = pad;
	ctx->stack_ptr = (uint8_t*) ctx->stack_ptr + 1;
	*((uint16_t*) ctx->stack_ptr) = frame_size;
	ctx->stack_ptr = (uint16_t*) ctx->stack_ptr + 1;
	room = (const bui_room_t*) PIC(room);
	*((const bui_room_t**) ctx->stack_ptr) = room;
	ctx->stack_ptr = (const bui_room_t**) ctx->stack_ptr + 1;
	ctx->frame_ptr = ctx->stack_ptr;
	if (args_size != 0)
		bui_room_push(ctx, args, args_size);
	{
		bui_room_event_data_enter_t data = { .up = true };
		bui_room_event_t event = { .id = BUI_ROOM_EVENT_ENTER, .data = &data };
		bui_room_dispatch_event(ctx, &event);
	}
}

void bui_room_exit(bui_room_ctx_t *ctx) {
	{
		bui_room_event_data_exit_t data = { .up = false };
		bui_room_event_t event = { .id = BUI_ROOM_EVENT_EXIT, .data = &data };
		bui_room_dispatch_event(ctx, &event);
	}
	uint16_t ret_size = (uint8_t*) ctx->stack_ptr - (uint8_t*) ctx->frame_ptr;
	void *ret = ctx->frame_ptr;
	ctx->frame_ptr = (uint8_t*) ctx->frame_ptr - (sizeof(const bui_room_t*) + sizeof(uint16_t));
	uint16_t frame_size = *((uint16_t*) ctx->frame_ptr);
	ctx->frame_ptr = (uint8_t*) ctx->frame_ptr - 1;
	uint8_t pad = *((uint8_t*) ctx->frame_ptr);
	ctx->frame_ptr = (uint8_t*) ctx->frame_ptr - pad;
	ctx->stack_ptr = ctx->frame_ptr;
	ctx->frame_ptr = (uint8_t*) ctx->frame_ptr - frame_size;
	if (ret_size != 0) {
		os_memmove(ctx->stack_ptr, ret, ret_size);
		ctx->stack_ptr = (uint8_t*) ctx->stack_ptr + ret_size;
	}
	{
		bui_room_event_data_enter_t data = { .up = false };
		bui_room_event_t event = { .id = BUI_ROOM_EVENT_ENTER, .data = &data };
		bui_room_dispatch_event(ctx, &event);
	}
}

const bui_room_t* bui_room_get_current(const bui_room_ctx_t *ctx) {
	return *((const bui_room_t**) ctx->frame_ptr - 1);
}

void bui_room_dealloc_frame(bui_room_ctx_t *ctx) {
	ctx->stack_ptr = ctx->frame_ptr;
}

void bui_room_push(bui_room_ctx_t *ctx, const void *src, uint16_t size) {
	os_memcpy(ctx->stack_ptr, src, size);
	ctx->stack_ptr = (uint8_t*) ctx->stack_ptr + size;
}

void bui_room_pop(bui_room_ctx_t *ctx, void *dest, uint16_t size) {
	ctx->stack_ptr = (uint8_t*) ctx->stack_ptr - size;
	os_memcpy(dest, ctx->stack_ptr, size);
}

void bui_room_peek(const bui_room_ctx_t *ctx, void *dest, uint16_t size, uint16_t offset) {
	os_memcpy(dest, (uint8_t*) ctx->stack_ptr - offset, size);
}

void* bui_room_alloc(bui_room_ctx_t *ctx, uint16_t size) {
	uint8_t *ptr = ctx->stack_ptr;
	ctx->stack_ptr = (uint8_t*) ctx->stack_ptr + size;
	return ptr;
}

void* bui_room_dealloc(bui_room_ctx_t *ctx, uint16_t size) {
	uint8_t *ptr = ctx->stack_ptr;
	ctx->stack_ptr = (uint8_t*) ctx->stack_ptr - size;
	return ptr;
}

void bui_room_dispatch_event(bui_room_ctx_t *ctx, const bui_room_event_t *event) {
	bui_room_t *current = (bui_room_t*) bui_room_get_current(ctx);
	bui_room_event_handler_t event_handler = current->event_handler;
	if (event_handler == NULL)
		return;
	event_handler = (bui_room_event_handler_t) PIC(event_handler);
	event_handler(ctx, event);
}

void bui_room_forward_event(bui_room_ctx_t *ctx, const bui_event_t *bui_event) {
	bui_room_event_t event = { .id = BUI_ROOM_EVENT_FORWARD, .data = bui_event };
	bui_room_dispatch_event(ctx, &event);
}

static void bui_room_message_handle_event(bui_room_ctx_t *ctx, const bui_room_event_t *event) {
	switch (event->id) {
	case BUI_ROOM_EVENT_EXIT: {
		bui_room_dealloc_frame(ctx);
	} break;
	case BUI_ROOM_EVENT_DRAW: {
		const bui_room_event_data_draw_t *data = BUI_ROOM_EVENT_DATA_DRAW(event);
		bui_room_message_args_t *args = ctx->frame_ptr;
		uint8_t char_height = bui_font_get_font_info(args->font)->char_height;
		uint8_t n_lines = 1;
		for (const char *ch = args->msg; *ch != '\0'; ch++) {
			if (*ch == '\n')
				n_lines++;
		}
		int16_t y = -((int16_t) n_lines * (char_height + 1) - 1) / 2 + 16;
		bool last_line = false;
		for (const char *line = args->msg; !last_line;) {
			uint8_t line_len = 0;
			for (;; line_len++) {
				if (line[line_len] == '\0') {
					last_line = true;
					break;
				}
				if (line[line_len] == '\n') {
					last_line = false;
					break;
				}
			}
			bui_font_draw_char_buff(data->bui_ctx, line, line_len, 64, y, BUI_DIR_TOP, args->font);
			line += line_len + 1;
			y += char_height + 1;
		}
	} break;
	case BUI_ROOM_EVENT_FORWARD: {
		const bui_event_t *bui_event = BUI_ROOM_EVENT_DATA_FORWARD(event);
		switch (bui_event->id) {
		case BUI_EVENT_BUTTON_CLICKED: {
			bui_button_id_t button = BUI_EVENT_DATA_BUTTON_CLICKED(bui_event)->button;
			if (button == BUI_BUTTON_NANOS_BOTH)
				bui_room_exit(ctx);
		} break;
		// Other events are acknowledged
		default:
			break;
		}
	} break;
	// Other events are acknowledged
	default:
		break;
	}
}

const bui_room_t bui_room_message = {
	.event_handler = bui_room_message_handle_event,
};
