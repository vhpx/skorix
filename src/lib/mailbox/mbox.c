// -----------------------------------mbox.c
// -------------------------------------
#include "../headers/mbox.h"
#include "../headers/uart0.h"

/* Mailbox Data Buffer (each element is 32-bit)*/
/*
 * The keyword attribute allows you to specify special attributes
 *
 * The aligned(N) attribute aligns the current data item on an address
 * which is a multiple of N, by inserting padding bytes before the data item
 *
 * __attribute__((aligned(16)) : allocate the variable on a 16-byte boundary.
 *
 *
 * We must ensure that our our buffer is located at a 16 byte aligned address,
 * so only the high 28 bits contain the address
 * (last 4 bits is ZERO due to 16 byte alignment)
 *
 */
volatile unsigned int __attribute__((aligned(16))) mBuf[36];
/**
 * Read from the mailbox
 */
unsigned int mailbox_read(unsigned char channel) {
  // Receiving message is buffer_addr & channel number
  unsigned int res;
  // Make sure that the message is from the right channel
  do {
    // Make sure there is mail to receive
    do {
      asm volatile("nop");
    } while (MBOX0_STATUS & MBOX_EMPTY);
    // Get the message
    res = MBOX0_READ;
  } while ((res & 0xF) != channel);
  return res;
}
/**
 * Write to the mailbox
 */
void mailbox_send(unsigned int msg, unsigned char channel) {
  // Sending message is buffer_addr & channel number
  //  Make sure you can send mail
  do {
    asm volatile("nop");
  } while (MBOX1_STATUS & MBOX_FULL);
  // send the message
  MBOX1_WRITE = msg;
}
/**
 * Make a mailbox call. Returns 0 on failure, non-zero on success
 */
int mbox_call(unsigned int buffer_addr, unsigned char channel) {
  // Prepare Data (address of Message Buffer)
  unsigned int msg = (buffer_addr & ~0xF) | (channel & 0xF);
  mailbox_send(msg, channel);
  /* now wait for the response */
  /* is it a response to our message (same address)? */
  if (msg == mailbox_read(channel)) {
    /* is it a valid successful response (Response Code) ? */
    if (mBuf[1] == MBOX_RESPONSE)
      return (mBuf[1] == MBOX_RESPONSE);
  }
  return 0;
}