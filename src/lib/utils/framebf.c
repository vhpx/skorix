#include "../headers/font.h"
#include "../headers/math.h"
#include "../headers/mbox.h"
#include "../headers/uart0.h"

// Use RGBA32 (32 bits for each pixel)
#define COLOR_DEPTH 32
// Pixel Order: BGR in memory order (little endian --> RGB in byte order)
#define PIXEL_ORDER 0
// Screen info
unsigned int width, height, pitch;
/* Frame buffer address
 * (declare as pointer of unsigned char to access each byte) */
unsigned char *fb;

void initialize_frame_buffer(int physicalWidth, int physicalHeight,
                             int virtualWidth, int virtualHeight) {
  mBuf[0] = 35 * 4; // Length of message in bytes
  mBuf[1] = MBOX_REQUEST;

  mBuf[2] = MBOX_TAG_SETPHYWH; // Set physical width-height
  mBuf[3] = 8;                 // Value size in bytes
  mBuf[4] = 0;                 // REQUEST CODE = 0
  mBuf[5] = physicalWidth;     // Value(width)
  mBuf[6] = physicalHeight;    // Value(height)

  mBuf[7] = MBOX_TAG_SETVIRTWH; // Set virtual width-height
  mBuf[8] = 8;
  mBuf[9] = 0;
  mBuf[10] = virtualWidth;
  mBuf[11] = virtualHeight;

  mBuf[12] = MBOX_TAG_SETVIRTOFF; // Set virtual offset
  mBuf[13] = 8;
  mBuf[14] = 0;
  mBuf[15] = 0; // x offset
  mBuf[16] = 0; // y offset

  mBuf[17] = MBOX_TAG_SETDEPTH; // Set color depth
  mBuf[18] = 4;
  mBuf[19] = 0;
  mBuf[20] = COLOR_DEPTH; // Bits per pixel

  mBuf[21] = MBOX_TAG_SETPXLORDR; // Set pixel order
  mBuf[22] = 4;
  mBuf[23] = 0;
  mBuf[24] = PIXEL_ORDER;

  mBuf[25] = MBOX_TAG_GETFB; // Get frame buffer
  mBuf[26] = 8;
  mBuf[27] = 0;
  mBuf[28] = 16; // alignment in 16 bytes
  mBuf[29] = 0;  // will return Frame Buffer size in bytes

  mBuf[30] = MBOX_TAG_GETPITCH; // Get pitch
  mBuf[31] = 4;
  mBuf[32] = 0;
  mBuf[33] = 0; // Will get pitch value here

  mBuf[34] = MBOX_TAG_LAST;

  // Call Mailbox
  if (mbox_call(ADDR(mBuf), MBOX_CH_PROP) // mailbox call is successful ?
      && mBuf[20] == COLOR_DEPTH          // got correct color depth ?
      && mBuf[24] == PIXEL_ORDER          // got correct pixel order ?
      && mBuf[28] != 0 // got a valid address for frame buffer ?
  ) {
    /* Convert GPU address to ARM address (clear higher address bits)
     * Frame Buffer is located in RAM memory, which VideoCore MMU
     * maps it to bus address space starting at 0xC0000000.
     * Software accessing RAM directly use physical addresses
     * (based at 0x00000000)
     */
    mBuf[28] &= 0x3FFFFFFF;
    // Access frame buffer as 1 byte per each address
    fb = (unsigned char *)((unsigned long)mBuf[28]);
    // uart_puts("Got allocated Frame Buffer at RAM physical address: ");
    // uart_hex(mBuf[28]);
    // uart_puts("\n");
    // uart_puts("Frame Buffer Size (bytes): ");
    // uart_dec(mBuf[29]);
    // uart_puts("\n");
    width = mBuf[5];  // Actual physical width
    height = mBuf[6]; // Actual physical height
    pitch = mBuf[33]; // Number of bytes per line
  } else {
    uart_puts("Unable to get a frame buffer with provided setting\n");
  }
}

void draw_pixel_ARGB_32(int x, int y, unsigned int attr) {
  int offs = (y * pitch) + (COLOR_DEPTH / 8 * x);
  /* //Access and assign each byte
   *(fb + offs ) = (attr >> 0 ) & 0xFF; //BLUE (get the least significant byte)
   *(fb + offs + 1) = (attr >> 8 ) & 0xFF; //GREEN
   *(fb + offs + 2) = (attr >> 16) & 0xFF; //RED
   *(fb + offs + 3) = (attr >> 24) & 0xFF; //ALPHA
   */
  // Access 32-bit together
  *((unsigned int *)(fb + offs)) = attr;
}

void draw_rect_ARGB_32(int x1, int y1, int x2, int y2, unsigned int attr,
                       int fill) {
  for (int y = y1; y <= y2; y++) {
    for (int x = x1; x <= x2; x++) {
      if ((x == x1 || x == x2) || (y == y1 || y == y2))
        draw_pixel_ARGB_32(x, y, attr);
      else if (fill)
        draw_pixel_ARGB_32(x, y, attr);
    }
  }
}

void draw_pixel(int x, int y, unsigned int attr) {
  draw_pixel_ARGB_32(x, y, attr);
}

void draw_transparent_pixel(int x, int y, unsigned int attr) {
  if (attr == 0)
    return;

  draw_pixel_ARGB_32(x, y, attr);
}

void draw_base_image(int x, int y, int width, int height,
                     const unsigned long *bitmap, int transparent) {
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      // Calculate the offset for the bitmap pixel
      int bitmapOffs = (j * width) + i;

      // Draw the pixel from the bitmap to the screen
      if (transparent) {
        draw_transparent_pixel(x + i, y + j, bitmap[bitmapOffs]);
      } else {
        draw_pixel(x + i, y + j, bitmap[bitmapOffs]);
      }
    }
  }
}

void draw_image(int x, int y, int width, int height,
                const unsigned long *bitmap) {
  draw_base_image(x, y, width, height, bitmap, 0);
}

void draw_transparent_image(int x, int y, int width, int height,
                            const unsigned long *bitmap) {
  draw_base_image(x, y, width, height, bitmap, 1);
}

void draw_rect(int x1, int y1, int x2, int y2, unsigned int attr, int fill) {
  draw_rect_ARGB_32(x1, y1, x2, y2, attr, fill);
}

void draw_line(int x1, int y1, int x2, int y2, unsigned char attr) {
  int dx = abs(x2 - x1);
  int dy = abs(y2 - y1);
  int sx = (x1 < x2) ? 1 : -1;
  int sy = (y1 < y2) ? 1 : -1;
  int err = dx - dy;

  while (1) {
    draw_pixel(x1, y1, attr);

    if (x1 == x2 && y1 == y2) {
      break;
    }

    int e2 = 2 * err;

    if (e2 > -dy) {
      err -= dy;
      x1 += sx;
    }

    if (e2 < dx) {
      err += dx;
      y1 += sy;
    }
  }
}

void draw_circle(int x0, int y0, int radius, unsigned char attr, int fill) {
  int x = radius;
  int y = 0;
  int err = 0;

  while (x >= y) {
    if (fill) {
      draw_line(x0 - y, y0 + x, x0 + y, y0 + x, attr);
      draw_line(x0 - x, y0 + y, x0 + x, y0 + y, attr);
      draw_line(x0 - x, y0 - y, x0 + x, y0 - y, attr);
      draw_line(x0 - y, y0 - x, x0 + y, y0 - x, attr);
    }
    y++;
    err += 1 + 2 * y;
    if (2 * (err - x) + 1 > 0) {
      x--;
      err += 1 - 2 * x;
    }
  }
}

void draw_char(unsigned char ch, int x, int y, unsigned int attr, int zoom) {
  unsigned char *glyph =
      (unsigned char *)&font + (ch < FONT_NUMGLYPHS ? ch : 0) * FONT_BPG;

  for (int i = 1; i <= (FONT_HEIGHT * zoom); i++) {
    for (int j = 0; j < (FONT_WIDTH * zoom); j++) {
      unsigned char mask = 1 << (j / zoom);
      if (*glyph & mask) { // only draw pixels belong to the character glyph
        draw_pixel(x + j, y + i, attr);
      }
    }
    glyph += (i % zoom) ? 0 : FONT_BPL;
  }
}

void draw_string(int x, int y, char *str, unsigned int attr, int zoom) {
  while (*str) {
    if (*str == '\r') {
      x = 0;
    } else if (*str == '\n') {
      x = 0;
      y += (FONT_HEIGHT * zoom);
    } else {
      draw_char(*str, x, y, attr, zoom);
      x += (FONT_WIDTH * zoom);
    }
    str++;
  }
}

void move_rect(int x, int y, int width, int height, int xoff, int yoff,
               unsigned char attr) {
  draw_rect(x, y, x + width, y + height, 0, 1);
  draw_rect(x + xoff, y + yoff, x + width + xoff, y + height + yoff, attr, 1);
}

void copy_rect(int srcX, int srcY, int width, int height, unsigned long *dest) {
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      // Calculate the offset for the source pixel
      int srcOffs = ((srcY + y) * pitch) + (COLOR_DEPTH / 8 * (srcX + x));

      // Calculate the offset for the destination pixel
      int destOffs = (y * width) + x;

      // Copy the pixel from the source to the destination
      dest[destOffs] = *((unsigned int *)(fb + srcOffs));
    }
  }
}

void draw_rect_from_bitmap(int x, int y, int width, int height,
                           unsigned long *bitmap) {
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      // Calculate the offset for the bitmap pixel
      int bitmapOffs = (j * width) + i;
      // Draw the pixel from the bitmap to the screen
      draw_pixel_ARGB_32(x + i, y + j, bitmap[bitmapOffs]);
    }
  }
}