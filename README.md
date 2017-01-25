# BOLOS User Interface Library v0.2.1

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
sent to the MCU to be displayed. When the application is ready to display the
frame, `bui_display(...)` is used to convert the buffer into the format used by
the MCU and send it. Storing the display buffer on the SE greatly simplifies the
rendering process for applications with complicated interfaces because rendering
can be done without having to communicate with the MCU.

The library's API is thoroughly documented in its header files. In order to
include this library in your project, simply include the files under the
`include/` directory and link to the source files in `src/`.

## Submodules

Aside from the core of the library in `src/bui.c` and `include/bui.h`, there are
also two major submodules of BUI that contain more specific display and user
interface related utilities.

### Font Submodule

The font submodule (which defines all symbols with the prefix `bui_font_`)
implements basic font rendering. The function `bui_font_draw_string(...)` allows
an application to draw a string onto a preallocated display buffer (of type
`bui_bitmap_128x32_t`). The library includes 14 different fonts ranging from 8
to 32 pixels in height. By default, bitmaps for all fonts are included. It is
highly recommended that applications declare defines when compiling BUI to
select only the necessary fonts to include for their application. To specify
that you wish to select fonts to include rather than include all of them, define
`BUI_FONT_CHOOSE`. Then, defines of the form `BUI_FONT_INCLUDE_<font name>`
specify the fonts that are to be included.

### Binary Keyboard Submodule

The binary keyboard submodule (which defines all symbols with the prefix
`bui_bkb_`) implements a "keyboard" that allows users to type letters, digits,
symbols, and punctuation on the Nano S using only the two buttons on the device.
I have created a [demo of the binary
keyboard](https://github.com/parkerhoyes/nanos-app-binarykbdemo) which is a good
starting point to learn how to use it in your project. Note that this module
requires the Lucida Console 8 font from the font submodule.

## Examples

For an example of how this library can be used, see
https://github.com/parkerhoyes/nanos-app-snake. This is an implementation of the
popular game Snake for the Nano S that uses this library to render its user
interface.

A demonstration of the binary keyboard implemented by this library is available
here: https://github.com/parkerhoyes/nanos-app-binarykbdemo.

If you use this library in your project, please feel free to email me (or submit
a pull request) and I'll link to it in this README.

## Development Cycle

This repository will follow a Git branching model similar to that described in
[Vincent Driessen's *A successful Git branching
model*](http://nvie.com/posts/a-successful-git-branching-model/) and a
versioning scheme similar to that defined by [Semantic Versioning
2.0.0](http://semver.org/).

## License

This library is distributed under the terms of the very permissive [Zlib
License](https://opensource.org/licenses/Zlib). The exact text of this license
is reproduced in the `LICENSE.txt` file as well as at the top of every source
file in this repository.
