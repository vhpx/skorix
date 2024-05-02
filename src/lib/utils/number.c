#include "../headers/number.h"
#include "../headers/string.h"

void parse_mac_address(unsigned int num1, unsigned int num2,
                       char *mac_address) {
  unsigned char bytes[6] = {num1 & 0xFF,         (num1 >> 8) & 0xFF,
                            (num1 >> 16) & 0xFF, (num1 >> 24) & 0xFF,
                            num2 & 0xFF,         (num2 >> 8) & 0xFF};

  for (int i = 0; i < 6; i++) {
    mac_address[i * 3] = (bytes[i] >> 4) < 10 ? (bytes[i] >> 4) + '0'
                                              : (bytes[i] >> 4) - 10 + 'A';
    mac_address[i * 3 + 1] = (bytes[i] & 0x0F) < 10
                                 ? (bytes[i] & 0x0F) + '0'
                                 : (bytes[i] & 0x0F) - 10 + 'A';
    mac_address[i * 3 + 2] = i < 5 ? ':' : '\0';
  }
}

void hex_to_str(unsigned int num, char *output) {
  for (int i = 0; i < 8; i++) {
    unsigned char nibble = (num >> (4 * (7 - i))) & 0x0F;
    output[i] = nibble < 10 ? nibble + '0' : nibble - 10 + 'A';
  }
  output[8] = '\0';
}

void parse_board_revision(unsigned int rev, unsigned int _omitted,
                          char *output) {
  hex_to_str(rev, output);
  trim_leading_zeros(output);
}
