# BOLOS User Interface Library v0.7.0

[This repository](https://github.com/parkerhoyes/bolos-user-interface) contains
a user interface library for
[BOLOS](http://ledger.readthedocs.io/en/latest/bolos/index.html) applications.
This library is currently targeted at only the [Ledger Nano
S](https://github.com/LedgerHQ/ledger-nano-s), though it is possible it may be
expanded to support other devices in the future.

This library shifts the GUI rendering which is usually done on the STM32F042
microcontroller unit in the device which manages peripherals over to the secure
ST31H320 chip where applications are loaded. This gives applications much
greater control over the rendering process.

In order to do this, the meat of the library is a
[blitting](https://en.wikipedia.org/wiki/Bit_blit) API which allows the
application to be rendered onto a display buffer stored on the SE before being
sent to the MCU to be displayed. The display buffer is stored within the BUI
context, of type `bui_ctx_t`, which also stores information about what parts of
the buffer need to be sent to the MCU. In order to display the data stored
within the BUI context, `bui_ctx_display(...)` is used. Storing the display
buffer on the SE greatly simplifies the rendering process for applications with
complicated interfaces because rendering can be done without having to
communicate with the MCU.

The library's API is thoroughly documented in its header files. In order to
include this library in your project, simply include the files under the
`include/` directory and link to the source files in `src/`.

The release notes and changelog for this library is maintained [on
GitHub](https://github.com/parkerhoyes/bolos-user-interface/releases).

## Modules

Aside from the core of the library in `src/bui.c` and `include/bui.h`, there are
also four major modules of BUI that contain more specific display and user
interface related utilities.

### Font Module

The font module (which defines all symbols with the prefix `bui_font_`)
implements basic font rendering and drawing. The library includes 14 different
fonts ranging from 8 to 32 pixels in height.

### Binary Keyboard Module

The binary keyboard module (which defines all symbols with the prefix
`bui_bkb_`) implements a "keyboard" that allows users to type letters, digits,
symbols, and punctuation on the Nano S using only the two buttons on the device.
Note that this module uses the Lucida Console 8 font from the font module. I
have created a [demo of the binary
keyboard](https://github.com/parkerhoyes/nanos-app-binarykbdemo) which is a good
starting point to learn how to use it in your project. The binary keyboard is
also used in the [demo of the menu
module](https://github.com/parkerhoyes/nanos-app-menudemo).

### Menu Module

The menu module (which defines all symbols with the prefix `bui_menu_`)
implements a "scrollable" list of display components. The menu supports optional
smooth scrolling animations (it uses an exponential curve to animate the
scroll). When initializing the menu, a callback is provided which is used to
render the menu's elements. I have created a [demo of the menu
module](https://github.com/parkerhoyes/nanos-app-menudemo) which is a good
starting point to learn how to use it in your project.

### Room Module

The room module (which defines all symbols with the prefix `bui_room_`) is an
incredibly useful utility for writing application UIs that include many
different "views" that are nested (such as nested menus). The room module
includes an abstract definition of a "room", which is a specific "view" or
"screen" that the application displays. Using a room context (of type
`bui_room_ctx_t`), applications can use a stack-based approach to managing their
rooms. The room context works a lot like a typical [call
stack](https://en.wikipedia.org/wiki/Call_stack), where each room is analogous
to a subroutine. A large benefit of this approach is that the stack-based design
allows for a different application memory layout in RAM based on what rooms are
visible and when. For a very thorough example of how to use the room module,
check out my [OTP 2FA App](https://github.com/parkerhoyes/bolos-app-otp2fa).

## Examples

I have created many different applications of varying degrees of complexity to
demonstrate the use of this library.

- [Nano S Snake Game](https://github.com/parkerhoyes/nanos-app-snake) - This is
  an implementation of the popular game Snake for the Nano S that uses this
  library to do its highly customized graphics rendering and render its user
  interface.

- [Menu Module Demo](https://github.com/parkerhoyes/nanos-app-menudemo) - A
  relatively simple app that demonstrates how to use the Menu Module to create a
  traditional vertical-style app. This app also uses the Binary Keyboard Module.

- [Binary Keyboard Module
  Demo](https://github.com/parkerhoyes/nanos-app-binarykbdemo) - If all you want
  to learn is how to use the binary keyboard, this is a good place to start.

- [OTP 2FA App](https://github.com/parkerhoyes/bolos-app-otp2fa) - This app is a
  fairly large-scale use of all of the features of this library. This is a good
  app to look at for more advanced developers who would like to learn how to use
  the Room Module to create advanced user interfaces with menus and text inputs.

If you use this library in your project, please feel free to email me (or submit
a pull request) and I'll link to it in this README.

## Development Cycle

This repository will follow a Git branching model similar to that described in
[Vincent Driessen's *A successful Git branching
model*](http://nvie.com/posts/a-successful-git-branching-model/) and a
versioning scheme similar to that defined by [Semantic Versioning
2.0.0](http://semver.org/).

The release notes and changelog for this library is maintained [on
GitHub](https://github.com/parkerhoyes/bolos-user-interface/releases).

## License

This library is distributed under the terms of the very permissive [Zlib
License](https://opensource.org/licenses/Zlib). The exact text of this license
is reproduced in the `LICENSE.txt` file as well as at the top of every source
file in this repository.
