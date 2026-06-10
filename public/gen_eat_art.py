#!/usr/bin/env python3
"""eat.png → braille ASCII art generator"""

from PIL import Image

IMG_PATH = "/home/whsoo/kmu/github/tamagotchi_rpg/public/eat.png"

# Target size: cols braille chars wide, rows braille chars tall
# each braille char = 2px wide × 4px tall
COLS = 52   # display columns (≈ terminal width for the art)
ROWS = 26   # braille rows

# Braille dot layout (bit positions in Unicode braille codepoint offset):
# dot1=0x01 dot2=0x02 dot3=0x04 dot7=0x40
# dot4=0x08 dot5=0x10 dot6=0x20 dot8=0x80
# Column 0 (left): dots 1,2,3,7  → rows 0,1,2,3
# Column 1 (right): dots 4,5,6,8 → rows 0,1,2,3
DOT_MAP = [
    [0x01, 0x08],
    [0x02, 0x10],
    [0x04, 0x20],
    [0x40, 0x80],
]

def img_to_braille(path, cols, rows, threshold=128, invert=False):
    img = Image.open(path).convert("RGBA")

    # White background composite (remove transparency)
    bg = Image.new("RGBA", img.size, (255, 255, 255, 255))
    bg.paste(img, mask=img.split()[3])
    img = bg.convert("L")

    px_w = cols * 2
    px_h = rows * 4
    img = img.resize((px_w, px_h), Image.LANCZOS)

    pixels = img.load()
    lines = []
    for row in range(rows):
        line = ""
        for col in range(cols):
            code = 0
            for dr in range(4):
                for dc in range(2):
                    px = pixels[col * 2 + dc, row * 4 + dr]
                    dark = px < threshold
                    if invert:
                        dark = not dark
                    if dark:
                        code |= DOT_MAP[dr][dc]
            line += chr(0x2800 + code)
        lines.append(line)
    return lines

lines = img_to_braille(IMG_PATH, COLS, ROWS, threshold=200, invert=False)

print(f"// {COLS} cols × {ROWS} rows braille art — eat.png")
print(f"// Indent: adjust leading spaces in Renderer.cpp")
print()
for line in lines:
    # Print as C++ string literal
    print(f'"{line}\\n"')

print()
print("// --- raw text preview ---")
for line in lines:
    print(line)
