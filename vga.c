#include "vga.h"
#include "string.h"

uint16_t *terminal_buffer = (uint16_t*) 0xB8000;
size_t terminal_row = 0;
size_t terminal_col = 0;
uint8_t terminal_color;

uint8_t make_color(enum VgaColor fg, enum VgaColor bg) {
  return fg | bg << 4;
}

uint16_t make_vgaentry(char c, uint8_t color) {
  uint16_t c16 = c;
  uint16_t color16 = color;
  return c16 | color16 << 8;
}

void terminal_setcolor(uint8_t color) {
  terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
  const size_t index = y * VGA_WIDTH + x;
  terminal_buffer[index] = make_vgaentry(c, color);
}

void terminal_putchar(char c) {
  terminal_putentryat(c, terminal_color, terminal_col, terminal_row);
  if (++terminal_col == VGA_WIDTH) {
    terminal_col = 0;
    if (++terminal_row == VGA_HEIGHT) {
      terminal_row = 0;
    }
  }
}

void terminal_writestring(const char* data) {
  size_t datalen = strlen(data);
  for (size_t i = 0; i < datalen; i++)
    terminal_putchar(data[i]);
}

void terminal_init(void) {
  terminal_color = make_color(COLOR_LIGHT_GREEN, COLOR_BLACK);
  for (size_t y = 0; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      const size_t index = y * VGA_WIDTH + x;
      terminal_buffer[index] = make_vgaentry(' ', terminal_color);
    }
  }
}
