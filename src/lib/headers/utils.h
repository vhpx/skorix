// void drawChar(unsigned char ch, int x, int y, unsigned int attr, int zoom);
// void drawString(int x, int y, char *str, unsigned int attr, int zoom);
unsigned int uart_isReadByteReady();
unsigned char getUart();
void wait_msec(unsigned int n);
void set_wait_timer(int set, unsigned int msVal);