# License for the BOLOS User Interface Library project, originally found here:
# https://github.com/parkerhoyes/bolos-user-interface
#
# Copyright (C) 2017 Parker Hoyes <contact@parkerhoyes.com>
#
# This software is provided "as-is", without any express or implied warranty. In
# no event will the authors be held liable for any damages arising from the use
# of this software.
#
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it freely,
# subject to the following restrictions:
#
# 1. The origin of this software must not be misrepresented; you must not claim
#    that you wrote the original software. If you use this software in a
#    product, an acknowledgment in the product documentation would be
#    appreciated but is not required.
# 2. Altered source versions must be plainly marked as such, and must not be
#    misrepresented as being the original software.
# 3. This notice may not be removed or altered from any source distribution.

from PIL import Image
import math
import os
import re
import sys

__all__ = [
    'img_to_bui_bitmap',
    'format_data',
]

def hexbyte(b):
    return hex(0x100 + b)[-2:]

def hexword(w):
    return hex(0x100000000 + w)[-8:]

def binspliced(i, n):
    return bin(i + (1 << n))[-n:]

def bits_to_bytes(bits):
    byts = []
    for i in range(0, len(bits), 8):
        byts.append(hexbyte(int(bits[i:i+8], 2)))
    return byts

def img_to_bui_bitmap(img):
    w, h = img.size
    colors = {}
    palette = img.getpalette()

    # Compute max color index
    max_index = 0
    for y in range(h):
        for x in range(w):
            color_index = img.getpixel((x, y))
            if color_index > max_index:
                max_index = color_index
    if max_index > 255:
        raise RuntimeError("Color index too large")

    # Encode palette into a list of strings of the form AARRGGBB
    new_palette = []
    for i in range(max_index + 1):
        r = palette[i * 3]
        g = palette[i * 3 + 1]
        b = palette[i * 3 + 2]
        new_palette.append(hexword((0xFF << 24) + (r << 16) + (g << 8) + b))
    palette = new_palette
    del new_palette

    # Reorder palette by color value and remove unused colors
    old_palette = palette
    palette = sorted(palette, key=lambda c: int(c, 16))
    unused_indexes = list(range(len(palette)))
    for y in range(h):
        for x in range(w):
            color_index = img.getpixel((x, y))
            if color_index in unused_indexes:
                unused_indexes.remove(color_index)
    unused_indexes.sort()
    unused_indexes.reverse()
    for i in unused_indexes:
        del palette[i]
    del unused_indexes
    palette_remapping = {}
    for i in range(len(old_palette)):
        if old_palette[i] in palette:
            palette_remapping[i] = palette.index(old_palette[i])
    del old_palette

    # Create raster
    raster = []
    for y in range(h):
        raster.append([])
        for x in range(w):
            raster[y].append(palette_remapping[img.getpixel((x, y))])
        raster[y].reverse()
    raster.reverse()

    # Encode raster as a byte sequence
    bpp = int(math.ceil(math.log(len(palette), 2)))
    if bpp > 0:
        bits = ''
        for row in raster:
            for col in row:
                bits += binspliced(col, bpp)
        b = bits_to_bytes(bits)
    else:
        b = []

    # Done
    return w, h, b, palette, bpp

def format_data(prefix, w, h, b, p, bpp):
    s = ''
    s += 'const uint8_t ' + prefix + '_w = ' + str(w) + ';\n'
    s += 'const uint8_t ' + prefix + '_h = ' + str(h) + ';\n'
    s += 'const uint8_t ' + prefix + '_bb[] = {'
    if len(b) > 0:
        for i in range(len(b)):
            if i % 8 == 0:
                s += '\n    '
            else:
                s += ' '
            s += '0x' + b[i].upper() + ','
        s += '\n};\n'
    else:
        s += '};\n'
    s += 'const uint32_t ' + prefix + '_plt[] = {'
    for c in p:
        s += '\n    0x' + c.upper() + ','
    s += '\n};\n'
    s += 'const uint8_t ' + prefix + '_bpp = ' + str(bpp) + ';\n'
    return s

def usage():
    sys.stderr.write("Usage: python " + sys.argv[0] + " <filename> [prefix]\n")

def main():
    if len(sys.argv) == 0:
        sys.stderr.write("Invalid arguments\n")
        sys.exit(1)
    if len(sys.argv) not in [2, 3]:
        usage()
        sys.exit(1)
    try:
        img = Image.open(sys.argv[1])
        img.load()
    except FileNotFoundError:
        sys.stderr.write("Error: File '" + sys.argv[1] + "' not found\n")
        usage()
        sys.exit(1)
    w, h, b, p, bpp = img_to_bui_bitmap(img)
    prefix = ('app_bmp_' + os.path.splitext(os.path.basename(sys.argv[1]))[0]) if len(sys.argv) != 3 else sys.argv[2]
    prefix = prefix.replace('-', '_')
    prefix = re.sub('[^a-zA-Z0-9_]', '', prefix)
    if len(prefix) == 0:
        prefix = 'app_bmp'
    sys.stdout.write(format_data(prefix, w, h, b, p, bpp))

if __name__ == '__main__':
    main()
