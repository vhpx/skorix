#include "../headers/color.h"
#include "../headers/font.h"
#include "../headers/math.h"
#include "../headers/mbox.h"
#include "../headers/string.h"
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

unsigned long long rendered_pixels = 0;

long long get_rendered_pixels() { return rendered_pixels; }

void reset_rendered_pixels() { rendered_pixels = 0; }

void print_rendered_pixels() {
  char buffer[20];
  format_num(rendered_pixels, buffer);
  pad_str(buffer, 10, ' ');
  uart_puts(COLOR.TEXT.BLUE);
  uart_puts(buffer);
  uart_puts(COLOR.RESET);
}

void print_pixel_diff(unsigned long long start, char *message) {
  char buffer[20];
  format_num(rendered_pixels - start, buffer);
  pad_str(buffer, 10, ' ');
  uart_puts(COLOR.TEXT.GREEN);
  uart_puts("+");
  uart_puts(buffer);
  uart_puts(COLOR.RESET);
  uart_puts(" | ");
  uart_puts(COLOR.TEXT.YELLOW);
  uart_puts(message);
  uart_puts(COLOR.RESET);
}

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

  // TODO: Only for debugging
  // Disable this for performance
  rendered_pixels++;
}

void draw_rect_ARGB_32(int x1, int y1, int x2, int y2, unsigned int attr,
                       int fill, int line_width) {
  for (int y = y1; y <= y2; y++) {
    for (int x = x1; x <= x2; x++) {
      if ((x >= x1 && x < x1 + line_width) ||
          (x <= x2 && x > x2 - line_width) ||
          (y >= y1 && y < y1 + line_width) || (y <= y2 && y > y2 - line_width))
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
  draw_rect_ARGB_32(x1, y1, x2, y2, attr, fill, 4);
}

void draw_line(int x1, int y1, int x2, int y2, unsigned int attr,
               int lineWidth) {
  int dx = abs(x2 - x1);
  int dy = abs(y2 - y1);
  int sx = (x1 < x2) ? 1 : -1;
  int sy = (y1 < y2) ? 1 : -1;
  int err = dx - dy;

  for (int i = 0; i < lineWidth; i++) {
    int nx1 = x1, ny1 = y1, nx2 = x2, ny2 = y2;

    if (dx > dy) {
      ny1 -= i;
      ny2 -= i;
    } else {
      nx1 -= i;
      nx2 -= i;
    }

    while (1) {
      draw_pixel(nx1, ny1, attr);

      if (nx1 == nx2 && ny1 == ny2) {
        break;
      }

      int e2 = 2 * err;

      if (e2 > -dy) {
        err -= dy;
        nx1 += sx;
      }

      if (e2 < dx) {
        err += dx;
        ny1 += sy;
      }
    }
  }
}

void draw_circle(int x0, int y0, int radius, unsigned char attr, int fill) {
  int x = radius;
  int y = 0;
  int err = 0;

  while (x >= y) {
    if (fill) {
      draw_line(x0 - y, y0 + x, x0 + y, y0 + x, attr, 1);
      draw_line(x0 - x, y0 + y, x0 + x, y0 + y, attr, 1);
      draw_line(x0 - x, y0 - y, x0 + x, y0 - y, attr, 1);
      draw_line(x0 - y, y0 - x, x0 + y, y0 - x, attr, 1);
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

void copy_rect(int srcX, int srcY, int destX, int destY, int srcWidth,
               int destWidth, int destHeight, const unsigned long *srcBitmap,
               unsigned long *dest) {
  for (int y = 0; y < destHeight; y++) {
    for (int x = 0; x < destWidth; x++) {
      // Calculate the offset for the source bitmap pixel
      int srcOffs = ((srcY + y) * srcWidth) + (srcX + x);

      // Calculate the offset for the destination bitmap pixel
      int destOffs = ((destY + y) * destWidth) + (destX + x);

      // Copy the pixel from the source bitmap to the destination
      dest[destOffs] = srcBitmap[srcOffs];
    }
  }
}

void copy_rect_alpha(int srcX, int srcY, int destX, int destY, int srcWidth,
                     int destWidth, int destHeight,
                     const unsigned long *srcBitmap, unsigned long *dest,
                     unsigned int attr) {
  // Clamp attr to valid range (0-255)
  if (attr > 255) {
    attr = 255;
  }

  for (int j = 0; j < destHeight; j++) {
    for (int i = 0; i < destWidth; i++) {
      int srcOffs = ((srcY + j) * srcWidth) + (srcX + i);
      unsigned long srcPixel = srcBitmap[srcOffs];

      // Extract RGB components from the source pixel
      unsigned char srcRed = (srcPixel >> 16) & 0xFF;
      unsigned char srcGreen = (srcPixel >> 8) & 0xFF;
      unsigned char srcBlue = srcPixel & 0xFF;

      // Apply the alpha blending to the source pixel
      srcRed = (srcRed * attr) / 255;
      srcGreen = (srcGreen * attr) / 255;
      srcBlue = (srcBlue * attr) / 255;

      // Combine the new RGB components with full opacity
      unsigned int newPixel =
          (255 << 24) | (srcRed << 16) | (srcGreen << 8) | srcBlue;

      // Copy the blended pixel to the destination
      int destOffs = ((destY + j) * destWidth) + (destX + i);
      dest[destOffs] = newPixel;
    }
  }
}

void draw_rect_from_bitmap(int x, int y, int width, int height,
                           const unsigned long *bitmap) {
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      // Calculate the offset for the bitmap pixel
      int bitmapOffs = (j * width) + i;
      // Draw the pixel from the bitmap to the screen
      draw_pixel(x + i, y + j, bitmap[bitmapOffs]);
    }
  }
}

void draw_rect_from_bitmap_alpha(int x, int y, int width, int height,
                                 const unsigned long *bitmap,
                                 unsigned char opacity) {
  // Clamp opacity to valid range (0-255)
  if (opacity > 255) {
    opacity = 255;
  }

  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      int bitmapOffs = (j * width) + i;
      unsigned long pixel = bitmap[bitmapOffs];

      // Extract RGB components from the bitmap pixel
      unsigned char red = (pixel >> 16) & 0xFF;
      unsigned char green = (pixel >> 8) & 0xFF;
      unsigned char blue = pixel & 0xFF;

      // Get the existing background pixel from the framebuffer
      int offs = (y + j) * pitch + (COLOR_DEPTH / 8) * (x + i);
      unsigned long bgPixel = *((unsigned int *)(fb + offs));

      // Extract RGB components from the background pixel
      unsigned char bgRed = (bgPixel >> 16) & 0xFF;
      unsigned char bgGreen = (bgPixel >> 8) & 0xFF;
      unsigned char bgBlue = bgPixel & 0xFF;

      // Blend the colors based on the fixed opacity
      unsigned char newRed = (opacity * red + (255 - opacity) * bgRed) / 255;
      unsigned char newGreen =
          (opacity * green + (255 - opacity) * bgGreen) / 255;
      unsigned char newBlue = (opacity * blue + (255 - opacity) * bgBlue) / 255;

      // Combine the new RGB components with full opacity
      unsigned int newPixel =
          (255 << 24) | (newRed << 16) | (newGreen << 8) | newBlue;

      // Draw the blended pixel
      draw_pixel(x + i, y + j, newPixel);
    }
  }
}
