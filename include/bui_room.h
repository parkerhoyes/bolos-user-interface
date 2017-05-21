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
	// It is recommended that these pointers be aligned to 4-byte boundaries to save space
	void *stack_ptr;
	void *frame_ptr;
} bui_room_ctx_t;

typedef enum {
	// Associated data: bui_room_event_enter_t. This event indicates that the current room has been "entered" into,
	// meaning it is now the room displayed on the screen. This event will be dispatched to the room before any other
	// event.
	BUI_ROOM_EVENT_ENTER = 1,
	// Associated data: bui_room_event_exit_t. This event indicates that the current room has been "exited" from,
	// meaning that it is no longer the room displayed on the screen. After this event is dispatched, no other events
	// will be dispatched to the room until the room is re-entered (and BUI_ROOM_EVENT_ENTER is dispatched). All data
	// remaining in the current stack frame after this event is handled by the current room is the data intended to be
	// returned to the room with the stack frame directly below the current room. If this event is dispatched to the
	// base room, the room's event handler does not return.
	BUI_ROOM_EVENT_EXIT = 2,
	// Associated data: bui_room_event_draw_t. This event is dispatched to request the current room to draw itself onto
	// the BUI context referenced in this event's data.
	BUI_ROOM_EVENT_DRAW = 3,
	// Associated data: bui_event_t. Events forwarded to rooms using bui_room_forward_event(...) are forwarded by
	// dispatching this event to the room.
	BUI_ROOM_EVENT_FORWARD = 4,
} bui_room_event_id_t;

typedef struct {
	// true if the current room is being entered after having its stack frame created anew, or false if the room is
	// being entered after a room higher on the stack was exited
	bool up;
} bui_room_event_data_enter_t;

typedef struct {
	// true if the current room is being exited before the stack frame for a new room is pushed onto the stack, or false
	// if the room is being exited before having its stack frame popped off the stack
	bool up;
} bui_room_event_data_exit_t;

typedef struct {
	// The BUI context onto which the current room is to be drawn.
	bui_ctx_t *bui_ctx;
} bui_room_event_data_draw_t;

typedef struct {
	bui_room_event_id_t id;
	const void *data;
} bui_room_event_t;

#define BUI_ROOM_EVENT_DATA_ENTER(event) ((const bui_room_event_data_enter_t*) (event)->data)
#define BUI_ROOM_EVENT_DATA_EXIT(event) ((const bui_room_event_data_exit_t*) (event)->data)
#define BUI_ROOM_EVENT_DATA_DRAW(event) ((const bui_room_event_data_draw_t*) (event)->data)
#define BUI_ROOM_EVENT_DATA_FORWARD(event) ((const bui_event_t*) (event)->data)

/*
 * Handle an event that has occurred in the specified room context. This function may also be NULL if no action is to be
 * performed. This pointer may be a pointer to NVRAM determined at link-time, in which case it must be passed through
 * PIC(...) to translate it to a valid address at runtime.
 *
 * Args:
 *     ctx: the room context in which the event has occurred
 *     event: the event to be handled
 */
typedef void (*bui_room_event_handler_t)(bui_room_ctx_t *ctx, const bui_room_event_t *event);

typedef struct {
	// The event handler used to dispatch an event to this room.
	bui_room_event_handler_t event_handler;
} bui_room_t;

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
void bui_room_ctx_init(bui_room_ctx_t *ctx, void *stack, const bui_room_t *room, const void *args, uint16_t args_size);

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
 * Dispatch the provided event to the current room in the provided room context.
 *
 * Args:
 *     ctx: the room context
 *     event: the room event to be dispatched
 */
void bui_room_dispatch_event(bui_room_ctx_t *ctx, const bui_room_event_t *event);

/*
 * Forward a BUI event (of type bui_event_t, NOT bui_room_event_t) to the current room in the specified room context by
 * dispatching a room event with ID BUI_ROOM_EVENT_FORWARD.
 *
 * Args:
 *     ctx: the room context
 *     bui_event: the BUI event to be forwarded
 */
void bui_room_forward_event(bui_room_ctx_t *ctx, const bui_event_t *bui_event);

#endif
