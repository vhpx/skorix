// ----------------------------------- framebf.h -------------------------------------
void framebf_init(int physicalWidth, int physicalHeight, int virtualWidth, int virtualHeight);
void drawPixelARGB32(int x, int y, unsigned int attr);
void drawRectARGB32(int x1, int y1, int x2, int y2, unsigned int attr, int fill);