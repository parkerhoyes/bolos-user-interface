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

#ifndef BUI_ROOM_H_
#define BUI_ROOM_H_

#include <stdbool.h>
#include <stdint.h>

#include "bui.h"

typedef struct bui_room_t bui_room_t;

/*
 * The BUI Room Module uses a stack-based architecture to implement a room-based GUI design. A "room" is a specific mode
 * that the display can be in, and it defines how the GUI is drawn onto the display and what to do with user input.

 * An application typically has one room context; the room context stores a pointer to the active room as well as
 * dynamically allocated memory for the current room to use, as well as memory to store the state of other rooms that
 * are inactive. The room context does this with a stack which is used in much the same way that a typical call stack is
 * used, with the exception that parameters passed to a room are stored above the frame pointer in its locals space,
 * instead of underneath the frame pointer. For more information about the use of call stacks, see
 * https://en.wikipedia.org/wiki/Call_stack.
 *
 * The stack is intended to be used as follows:
 *
 * - When entering a room, the previous room's exit callback is first called. Next, the size of the initial stack frame
 *   (not including the size itself or the room address) is pushed onto the stack, increasing the stack pointer. Next,
 *   the pointer to the new room is pushed onto the stack (it must be a valid pointer at runtime, so if it points to
 *   NVRAM it must've been passed through PIC(...)); the frame pointer is then pointed to the memory just above this
 *   address. Additionally, any parameters to be passed to the room are pushed onto the stack, increasing the stack
 *   pointer to point to the memory just after the parameters to the room. If no parameters are passed to the room, the
 *   stack pointer and the frame pointer are the same. Finally, the room's enter callback is called. There is no
 *   mechanism in place for rooms to "return" values on the stack.
 * - When exiting a room, the current room's exit callback is called first. Next, the stack pointer is restored to its
 *   value in the previous stack frame and the frame pointer is restored to its previous value using the frame size,
 *   which was stored just above the previous stack frame. Finally, the new room's enter callback is called.
 * - When pushing or popping local data for a room, the stack pointer is changed, not the frame pointer. As such, the
 *   amount of memory allocated for a room's local use is the stack pointer minus the frame pointer.
 */
typedef struct {
	uint8_t *stack_ptr;
	uint8_t *frame_ptr;
} bui_room_ctx_t;

/*
 * Indicate that the room has been "entered" into, meaning it is now the room displayed on the screen. This function
 * will be called before any other callback for the specified room. This pointer may be a pointer to NVRAM determined at
 * link-time, in which case it must be passed through PIC(...) to translate it to a valid address at runtime.
 *
 * Args:
 *     ctx: the room context that contains the room
 *     room: the room; this must be a valid pointer at runtime, meaning if it points to NVRAM it must be passed through
 *           PIC(...) if necessary
 *     up: true if the room is being entered after having its stack frame be created anew, false if the room is being
 *         entered after a room higher on the stack was exited
 */
typedef void (*bui_room_enter_callback_t)(bui_room_ctx_t *ctx, bui_room_t *room, bool up);

/*
 * Indicate that a room has been "exited" from, meaning that it is no longer the room displayed on the screen. After
 * this callback is called, no other callback will be called for the specified room until bui_room_enter_callback_t is
 * called. This pointer may be a pointer to NVRAM determined at link-time, in which case it must be passed through
 * PIC(...) to translate it to a valid address at runtime. All data remaining in the current stack frame after this
 * function returns is the data intended to be returned to the room with the stack frame directly below this one.
 *
 * If the room for which this callback is called is the base room, this function does not return.
 *
 * Args:
 *     ctx: the room context that contains the room
 *     room: the room; this must be a valid pointer at runtime, meaning if it points to NVRAM it must be passed through
 *           PIC(...) if necessary
 *     up: true if the room is being exited before the stack frame for a new room is pushed onto the stack, false if the
 *         room is being exited before having its stack frame popped off the stack
 */
typedef void (*bui_room_exit_callback_t)(bui_room_ctx_t *ctx, bui_room_t *room, bool up);

/*
 * Indicate to the room that a certain amount of time has passed. This callback may be used for animation purposes. This
 * pointer may be a pointer to NVRAM determined at link-time, in which case it must be passed through PIC(...) to
 * translate it to a valid address at runtime.
 *
 * Args:
 *     ctx: the room context that contains the room
 *     room: the room; this must be a valid pointer at runtime, meaning if it points to NVRAM it must be passed through
 *           PIC(...) if necessary
 *     elapsed: the amount of time elapsed since this callback was last called or since the room was entered into,
 *              whichever came last; this may not be 0
 * Returns:
 *     true if the appearance of the room may have changed between now and the last time this callback was called or the
 *     room was entered into, whichever came last, or false otherwise
 */
typedef bool (*bui_room_tick_callback_t)(bui_room_ctx_t *ctx, bui_room_t *room, uint32_t elapsed);

/*
 * Indicate to the room that a button press occurred. Both left and right may be true (if both were pressed at the same
 * time), but both may not be false. This pointer may be a pointer to NVRAM determined at link-time, in which case it
 * must be passed through PIC(...) to translate it to a valid address at runtime.
 *
 * Args:
 *     ctx: the room context that contains the room
 *     room: the room; this must be a valid pointer at runtime, meaning if it points to NVRAM it must be passed through
 *           PIC(...) if necessary
 *     left: true if the left button was pressed, false otherwise
 *     right: true if the right button was pressed, false otherwise
 */
typedef void (*bui_room_button_callback_t)(bui_room_ctx_t *ctx, bui_room_t *room, bool left, bool right);

/*
 * Draw the room onto the specified display buffer. This pointer may be a pointer to NVRAM determined at link-time, in
 * which case it must be passed through PIC(...) to translate it to a valid address at runtime.
 *
 * Args:
 *     ctx: the room context that contains the room
 *     room: the room; this must be a valid pointer at runtime, meaning if it points to NVRAM it must be passed through
 *           PIC(...) if necessary
 *     buffer: the display buffer
 */
typedef void (*bui_room_draw_callback_t)(bui_room_ctx_t *ctx, const bui_room_t *room, bui_bitmap_128x32_t *buffer);

struct bui_room_t {
	// See type documentation for bui_room_enter_callback_t for more information about how this callback is used. This
	// may also be NULL, to perform no action.
	bui_room_enter_callback_t enter;
	// See type documentation for bui_room_exit_callback_t for more information about how this callback is used. This
	// may also be NULL, to perform no action.
	bui_room_exit_callback_t exit;
	// See type documentation for bui_room_tick_callback_t for more information about how this callback is used. This
	// may also be NULL, to perform no action and assume a return value of false.
	bui_room_tick_callback_t tick;
	// See type documentation for bui_room_button_callback_t for more information about how this callback is used. This
	// may also be NULL, to perform no action.
	bui_room_button_callback_t button;
	// See type documentation for bui_room_draw_callback_t for more information about how this callback is used. This
	// may also be NULL, to perform no action.
	bui_room_draw_callback_t draw;
};

/*
 * Initialize a room context with the specified preallocated stack and base room. The room's enter callback is called.
 * The new stack frame (which is initially empty) is guaranteed to start at a memory address aligned to a 4-byte
 * boundary (ctx->stack_ptr % 4 is 0); this is achieved by adding padding before the stack frame, if necessary.
 *
 * Args:
 *     ctx: the room context
 *     stack: the pointer to the start of the stack space to be used by the room context
 *     room: the base room for which to create a stack frame at the bottom of the stack; this is passed through PIC(...)
 *           before being pushed onto the stack
 *     args: the pointer to the arguments for the base room to be copied onto the stack; this pointer is not
 *           dereferenced if args_size is 0
 *     args_size: the size of the arguments to be copied onto the stack, in bytes, or zero to copy no arguments
 */
void bui_room_ctx_init(bui_room_ctx_t *ctx, uint8_t *stack, const bui_room_t *room, const void *args,
		uint16_t args_size);

/*
 * Enter the specified room by creating a new stack frame that contains the provided arguments. The current room's exit
 * callback is called and the new room's enter callback is called. The provided pointer to the new room is passed
 * through PIC(...) before being pushed onto the stack. The new stack frame (which is initially empty) is guaranteed to
 * start at a memory address aligned to a 4-byte boundary (ctx->stack_ptr % 4 is 0); this is achieved by adding padding
 * between this stack frame and the previous one, if necessary.
 *
 * Args:
 *     ctx: the room context
 *     room: the room to enter; this is passed through PIC(...) before being pushed onto the stack
 *     args: the pointer to the arguments to be copied onto the stack; this pointer is not dereferenced if args_size is
 *           0
 *     args_size: the size of the arguments to be copied onto the stack, in bytes, or zero to copy no arguments
 */
void bui_room_enter(bui_room_ctx_t *ctx, const bui_room_t *room, const void *args, uint16_t args_size);

/*
 * Exit the current room, popping its stack frame off of the stack. The current room's exit callback is called and the
 * new room's enter callback is called. All data left in the current stack frame is returned to the room with the stack
 * frame directly below the current one (it is pushed onto that room's stack frame).
 *
 * Args:
 *     ctx: the room context
 */
void bui_room_exit(bui_room_ctx_t *ctx);

/*
 * Get the room pointer for the current stack frame.
 *
 * Args:
 *     ctx: the room context
 * Returns:
 *     the room pointer for the current stack frame, which should've already been passed through PIC(...), if necessary
 */
const bui_room_t* bui_room_get_current(const bui_room_ctx_t *ctx);

/*
 * Set the stack pointer to the frame pointer, effectively deallocating all data in the current stack frame.
 *
 * Args:
 *    ctx: the room context
 */
void bui_room_dealloc_frame(bui_room_ctx_t *ctx);

/*
 * Push data onto the top of the stack in the specified room context.
 *
 * Args:
 *     ctx: the room context
 *     src: the pointer to the data to be pushed onto the stack
 *     size: the size of the data in bytes; this may not be 0
 */
void bui_room_push(bui_room_ctx_t *ctx, const void *src, uint16_t size);

/*
 * Pop data from the top of the stack in the specified room context.
 *
 * Args:
 *     ctx: the room context
 *     dest: the pointer to the location into which the data at the top of the stack is to be copied
 *     size: the size of the data in bytes; this may not be 0
 */
void bui_room_pop(bui_room_ctx_t *ctx, void *dest, uint16_t size);

/*
 * Read data from the stack in the specified room context without popping it.
 *
 * Args:
 *     ctx: the room context
 *     dest: the pointer to the location into which the data is to be copied
 *     size: the size of the data in bytes; this may not be 0
 *     offset: the offset between the top of the stack and the first byte of the data that is to be retrieved, in bytes
 *             (an offset of 1 and a size of 1 reads 1 byte off the stack)
 */
void bui_room_peek(const bui_room_ctx_t *ctx, void *dest, uint16_t size, uint16_t offset);

/*
 * Increase the stack pointer for the specified room context by the specified amount and then return the original stack
 * pointer.
 *
 * Args:
 *     ctx: the room context
 *     size: the amount by which to increase the stack pointer, in bytes
 * Returns:
 *     the original stack pointer, before being increased
 */
void* bui_room_alloc(bui_room_ctx_t *ctx, uint16_t size);

/*
 * Decrease the stack pointer for the specified room context by the specified amount and then return the original stack
 * pointer.
 *
 * Args:
 *     ctx: the room context
 *     size: the amount by which to decrease the stack pointer, in bytes
 * Returns:
 *     the original stack pointer, before being decreased
 */
void* bui_room_dealloc(bui_room_ctx_t *ctx, uint16_t size);

/*
 * Retrieve the room pointer for the current stack frame in the specified room context, then call the room's "enter"
 * callback, unless it is NULL.
 *
 * Args:
 *     ctx: the room context
 *     up: passed to the callback
 */
void bui_room_current_enter(bui_room_ctx_t *ctx, bool up);

/*
 * Retrieve the room pointer for the current stack frame in the specified room context, then call the room's "exit"
 * callback, unless it is NULL.
 *
 * Args:
 *     ctx: the room context
 *     up: passed to the callback
 */
void bui_room_current_exit(bui_room_ctx_t *ctx, bool up);

/*
 * Retrieve the room pointer for the current stack frame in the specified room context, then call the room's "tick"
 * callback, unless it is NULL, in which case false is returned.
 *
 * Args:
 *     ctx: the room context
 *     elapsed: passed to the callback
 * Returns:
 *     the value that the callback returns, or false if the callback is NULL
 */
bool bui_room_current_tick(bui_room_ctx_t *ctx, uint32_t elapsed);

/*
 * Retrieve the room pointer for the current stack frame in the specified room context, then call the room's "button"
 * callback, unless it is NULL.
 *
 * Args:
 *     ctx: the room context
 *     left: passed to the callback
 *     right: passed to the callback
 */
void bui_room_current_button(bui_room_ctx_t *ctx, bool left, bool right);

/*
 * Retrieve the room pointer for the current stack frame in the specified room context, then call the room's "draw"
 * callback, unless it is NULL.
 *
 * Args:
 *     ctx: the room context
 *     buffer: passed to the callback
 */
void bui_room_current_draw(bui_room_ctx_t *ctx, bui_bitmap_128x32_t *buffer);

#endif
