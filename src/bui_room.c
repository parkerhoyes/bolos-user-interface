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

_Static_assert(sizeof(void*) == 4, "sizeof(void*) must be 4");
_Static_assert(sizeof(uint8_t) == 1, "sizeof(uint8_t) must be 1");
_Static_assert(sizeof(uint16_t) == 2, "sizeof(uint16_t) must be 2");

#define BUI_ROOM_PAD(ptr) ((4 - ((uintptr_t) (ptr) & 0x3)) & 0x3)

void bui_room_ctx_init(bui_room_ctx_t *ctx, uint8_t *stack, const bui_room_t *room, const void *args,
		uint16_t args_size) {
	stack += BUI_ROOM_PAD(stack);
	*((const bui_room_t**) stack) = (const bui_room_t*) PIC(room);
	stack += sizeof(const bui_room_t*);
	ctx->stack_ptr = stack;
	ctx->frame_ptr = stack;
	if (args_size != 0)
		bui_room_push(ctx, args, args_size);
	bui_room_current_enter(ctx, true);
}

void bui_room_enter(bui_room_ctx_t *ctx, const bui_room_t *room, const void *args, uint16_t args_size) {
	bui_room_current_exit(ctx, true);
	uint8_t pad = BUI_ROOM_PAD(ctx->stack_ptr + 3);
	uint16_t frame_size = (uint16_t) (ctx->stack_ptr - ctx->frame_ptr);
	ctx->stack_ptr += pad;
	*ctx->stack_ptr++ = pad;
	*((uint16_t*) ctx->stack_ptr) = frame_size;
	ctx->stack_ptr += 2;
	room = (const bui_room_t*) PIC(room);
	*((const bui_room_t**) ctx->stack_ptr) = room;
	ctx->stack_ptr += 4;
	ctx->frame_ptr = ctx->stack_ptr;
	if (args_size != 0)
		bui_room_push(ctx, args, args_size);
	bui_room_current_enter(ctx, true);
}

void bui_room_exit(bui_room_ctx_t *ctx) {
	bui_room_current_exit(ctx, false);
	uint16_t ret_size = ctx->stack_ptr - ctx->frame_ptr;
	void *ret = ctx->frame_ptr;
	ctx->frame_ptr -= sizeof(const bui_room_t*) + sizeof(uint16_t);
	uint16_t frame_size = *((uint16_t*) ctx->frame_ptr);
	ctx->frame_ptr -= 1;
	uint8_t pad = *ctx->frame_ptr;
	ctx->frame_ptr -= pad;
	ctx->stack_ptr = ctx->frame_ptr;
	ctx->frame_ptr -= frame_size;
	if (ret_size != 0) {
		os_memmove(ctx->stack_ptr, ret, ret_size);
		ctx->stack_ptr += ret_size;
	}
	bui_room_current_enter(ctx, false);
}

const bui_room_t* bui_room_get_current(const bui_room_ctx_t *ctx) {
	return *((const bui_room_t**) ctx->frame_ptr - 1);
}

void bui_room_dealloc_frame(bui_room_ctx_t *ctx) {
	ctx->stack_ptr = ctx->frame_ptr;
}

void bui_room_push(bui_room_ctx_t *ctx, const void *src, uint16_t size) {
	os_memcpy(ctx->stack_ptr, src, size);
	ctx->stack_ptr += size;
}

void bui_room_pop(bui_room_ctx_t *ctx, void *dest, uint16_t size) {
	ctx->stack_ptr -= size;
	os_memcpy(dest, ctx->stack_ptr, size);
}

void bui_room_peek(const bui_room_ctx_t *ctx, void *dest, uint16_t size, uint16_t offset) {
	os_memcpy(dest, ctx->stack_ptr - offset, size);
}

void* bui_room_alloc(bui_room_ctx_t *ctx, uint16_t size) {
	uint8_t *ptr = ctx->stack_ptr;
	ctx->stack_ptr += size;
	return ptr;
}

void* bui_room_dealloc(bui_room_ctx_t *ctx, uint16_t size) {
	uint8_t *ptr = ctx->stack_ptr;
	ctx->stack_ptr -= size;
	return ptr;
}

void bui_room_current_enter(bui_room_ctx_t *ctx, bool up) {
	bui_room_t *current = (bui_room_t*) bui_room_get_current(ctx);
	bui_room_enter_callback_t callback = current->enter;
	if (callback == NULL)
		return;
	callback = (bui_room_enter_callback_t) PIC(callback);
	callback(ctx, current, up);
}

void bui_room_current_exit(bui_room_ctx_t *ctx, bool up) {
	bui_room_t *current = (bui_room_t*) bui_room_get_current(ctx);
	bui_room_exit_callback_t callback = current->exit;
	if (callback == NULL)
		return;
	callback = (bui_room_exit_callback_t) PIC(callback);
	callback(ctx, current, up);
}

bool bui_room_current_tick(bui_room_ctx_t *ctx, uint32_t elapsed) {
	bui_room_t *current = (bui_room_t*) bui_room_get_current(ctx);
	bui_room_tick_callback_t callback = current->tick;
	if (callback == NULL)
		return false;
	callback = (bui_room_tick_callback_t) PIC(callback);
	return callback(ctx, current, elapsed);
}

void bui_room_current_button(bui_room_ctx_t *ctx, bool left, bool right) {
	bui_room_t *current = (bui_room_t*) bui_room_get_current(ctx);
	bui_room_button_callback_t callback = current->button;
	if (callback == NULL)
		return;
	callback = (bui_room_button_callback_t) PIC(callback);
	callback(ctx, current, left, right);
}

void bui_room_current_draw(bui_room_ctx_t *ctx, bui_bitmap_128x32_t *buffer) {
	bui_room_t *current = (bui_room_t*) bui_room_get_current(ctx);
	bui_room_draw_callback_t callback = current->draw;
	if (callback == NULL)
		return;
	callback = (bui_room_draw_callback_t) PIC(callback);
	callback(ctx, current, buffer);
}
