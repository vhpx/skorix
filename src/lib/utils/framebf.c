// ----------------------------------- framebf.c -------------------------------------
#include "../headers/mbox.h"
#include "../headers/uart0.h"
#include "../../img/img.h"
//Use RGBA32 (32 bits for each pixel)
#define COLOR_DEPTH 32
//Pixel Order: BGR in memory order (little endian --> RGB in byte order)
#define PIXEL_ORDER 0
//Screen info
unsigned int width, height, pitch;
/* Frame buffer address
* (declare as pointer of unsigned char to access each byte) */
unsigned char *fb;

void framebf_init(int physicalWidth, int physicalHeight, int virtualWidth, int virtualHeight) {
    mBuf[0] = 35*4; // Length of message in bytes
    mBuf[1] = MBOX_REQUEST;
    mBuf[2] = MBOX_TAG_SETPHYWH; //Set physical width-height
    mBuf[3] = 8; // Value size in bytes
    mBuf[4] = 0; // REQUEST CODE = 0
    mBuf[5] = physicalWidth; // Value(width)
    mBuf[6] = physicalHeight; // Value(height)
    mBuf[7] = MBOX_TAG_SETVIRTWH; //Set virtual width-height
    mBuf[8] = 8;
    mBuf[9] = 0;
    mBuf[10] = virtualWidth;
    mBuf[11] = virtualHeight;
    mBuf[12] = MBOX_TAG_SETVIRTOFF; //Set virtual offset
    mBuf[13] = 8;
    mBuf[14] = 0;
    mBuf[15] = 0; // x offset
    mBuf[16] = 0; // y offset
    mBuf[17] = MBOX_TAG_SETDEPTH; //Set color depth
    mBuf[18] = 4;
    mBuf[19] = 0;
    mBuf[20] = COLOR_DEPTH; //Bits per pixel
    mBuf[21] = MBOX_TAG_SETPXLORDR; //Set pixel order
    mBuf[22] = 4;
    mBuf[23] = 0;
    mBuf[24] = PIXEL_ORDER;
    mBuf[25] = MBOX_TAG_GETFB; //Get frame buffer
    mBuf[26] = 8;
    mBuf[27] = 0;
    mBuf[28] = 16; //alignment in 16 bytes
    mBuf[29] = 0; //will return Frame Buffer size in bytes
    mBuf[30] = MBOX_TAG_GETPITCH; //Get pitch
    mBuf[31] = 4;
    mBuf[32] = 0;
    mBuf[33] = 0; //Will get pitch value here
    mBuf[34] = MBOX_TAG_LAST;
    // Call Mailbox
    if (mbox_call(ADDR(mBuf), MBOX_CH_PROP) //mailbox call is successful ?
        && mBuf[20] == COLOR_DEPTH //got correct color depth ?
        && mBuf[24] == PIXEL_ORDER //got correct pixel order ?
        && mBuf[28] != 0 //got a valid address for frame buffer ?
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
        width = mBuf[5]; // Actual physical width
        height = mBuf[6]; // Actual physical height
        pitch = mBuf[33]; // Number of bytes per line
    } else {
        uart_puts("Unable to get a frame buffer with provided setting\n");
    }
}
void drawPixelARGB32(int x, int y, unsigned int attr) {
    int offs = (y * pitch) + (COLOR_DEPTH/8 * x);
    /* //Access and assign each byte
    *(fb + offs ) = (attr >> 0 ) & 0xFF; //BLUE (get the least significant byte)
    *(fb + offs + 1) = (attr >> 8 ) & 0xFF; //GREEN
    *(fb + offs + 2) = (attr >> 16) & 0xFF; //RED
    *(fb + offs + 3) = (attr >> 24) & 0xFF; //ALPHA
    */
    //Access 32-bit together
    *((unsigned int*)(fb + offs)) = attr;
}

void drawRectARGB32(int x1, int y1, int x2, int y2, unsigned int attr, int fill) {
    for (int y = y1; y <= y2; y++ ) {
        for (int x = x1; x <= x2; x++) {
            if ((x == x1 || x == x2) || (y == y1 || y == y2))
                drawPixelARGB32(x, y, attr);
                    else if (fill)
                drawPixelARGB32(x, y, attr);
        }
    }
}
