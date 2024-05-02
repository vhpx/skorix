#include "../headers/string.h"
#include "../headers/constants.h"

// Function to compare two strings
int strcmp(const char *str1, const char *str2) {
  // if length of both strings are not equal, return 1
  if (strlen(str1) != strlen(str2)) {
    return 1;
  }

  // Loop until the end of either string
  while (*str1 && *str2) {
    // If the characters are not equal, return 1
    if (*str1 != *str2) {
      return 1;
    }
    // Move to the next character
    str1++;
    str2++;
  }
  // If the end of both strings has been reached, return 0
  return 0;
}

void skip_spaces(char **str) {
  // Loop until the end of the string
  while (**str) {
    // If a space is found, move to the next character
    if (**str == ' ') {
      (*str)++;
    } else {
      // If a non-space character is found, return
      return;
    }
  }
}

int strlen(const char *str) {
  // Initialize the length to 0
  int len = 0;
  // Loop until the end of the string
  while (*str != '\0') {
    // Increment the length
    len++;
    // Move to the next character
    str++;
  }
  // Return the length
  return len;
}

int strlen_no_color(const char *str) {
  // Initialize the length to 0
  int len = 0;
  // Loop until the end of the string
  while (*str != '\0') {
    // If the character is a color code, skip it
    if (*str == '\x1B') {
      while (*str != 'm') {
        str++;
      }
    } else {
      // Increment the length
      len++;
    }
    // Move to the next character
    str++;
  }
  // Return the length
  return len;
}

void custom_strcpy(char *dest, const char *src) {
  // Loop until the end of the source string
  while (*src) {
    // Copy the character from source to destination
    *dest = *src;
    // Move to the next character
    dest++;
    src++;
  }
  // Add a null character at the end of the destination string
  *dest = '\0';
}

int str2int(char *str) {
  // Initialize the result to 0
  int result = 0;
  // Loop until the end of the string
  while (*str) {
    // Convert the character to integer
    int digit = *str - '0';
    // Multiply the result by 10 and add the digit
    result = result * 10 + digit;
    // Move to the next character
    str++;
  }
  // Return the final result
  return result;
}

void int2str(int num, char *str) {
  // If the number is 0, return "0"
  if (num == 0) {
    str[0] = '0';
    str[1] = '\0';
    return;
  }

  // Initialize the index to 0
  int index = 0;
  // Initialize the result to 0
  int result = num;

  // Loop until the result is greater than 0
  while (result > 0) {
    // Get the last digit of the result
    int digit = result % 10;
    // Convert the digit to character
    str[index] = digit + '0';
    // Divide the result by 10
    result = result / 10;
    // Move to the next index
    index++;
  }
  // Add a null character at the end of the string
  str[index] = '\0';
  // Reverse the string
  reverse(str);
}

void reverse(char *str) {
  // Initialize the length of the string
  int len = strlen(str);
  // Loop until the middle of the string
  for (int i = 0; i < len / 2; i++) {
    // Swap the characters from start and end
    char temp = str[i];
    str[i] = str[len - i - 1];
    str[len - i - 1] = temp;
  }
}

void append_str(char *str, char *append) {
  // Loop until the end of the string
  while (*str) {
    // Move to the next character
    str++;
  }
  // Loop until the end of the append string
  while (*append) {
    // Copy the character from append to str
    *str = *append;
    // Move to the next character
    str++;
    append++;
  }
  // Add a null character at the end of the string
  *str = '\0';
}

void trim_leading_zeros(char *str) {
  // Initialize the index to 0
  int index = 0;
  // Loop until the end of the string
  while (str[index] == '0') {
    // Move to the next character
    index++;
  }
  // Shift the remaining characters to the left
  for (int i = 0; i < strlen(str) - index; i++) {
    str[i] = str[i + index];
  }
  // Add a null character at the end of the string
  str[strlen(str) - index] = '\0';
}

// Function to trim leading and trailing spaces from a string
void trim(char *str) {
  // Initialize the start index to 0
  int start = 0;
  // Initialize the end index to the length of the string
  int end = strlen(str) - 1;

  // Loop until the start index is less than the end index
  while (str[start] == ' ') {
    // Move to the next character
    start++;
  }
  // Loop until the end index is greater than the start index
  while (str[end] == ' ') {
    // Move to the previous character
    end--;
  }
  // Shift the remaining characters to the left
  for (int i = 0; i <= end - start; i++) {
    str[i] = str[i + start];
  }
  // Add a null character at the end of the string
  str[end - start + 1] = '\0';
}

void str_auto_newline(char *str, int max_length, char *starts_with,
                      char *ends_with, int required_length) {
  // Initialize the length of the string
  int len = strlen(str);
  // Initialize the index to 0
  int index = 0;
  // Loop until the end of the string
  while (index < len) {
    // If the max length is reached
    if ((index + 1) % max_length == 0 && index < len - 1) {
      // Shift the remaining characters to the right
      int ends_with_len = strlen(ends_with);
      for (int i = len; i > index; i--) {
        str[i + ends_with_len] = str[i];
      }
      // Append ends_with to the end of the line
      for (int i = 0; i < ends_with_len; i++) {
        str[index++] = ends_with[i];
      }
      // Append a newline character to the end of the line
      str[index++] = '\n';
      len += ends_with_len + 1; // Update the length of the string

      // Shift the remaining characters to the right
      int starts_with_len = strlen(starts_with);
      for (int i = len; i >= index; i--) {
        str[i + starts_with_len] = str[i];
      }
      // Append starts_with to the start of the next line
      for (int i = 0; i < starts_with_len; i++) {
        str[index++] = starts_with[i];
      }
      len += starts_with_len; // Update the length of the string
    } else {
      index++;
    }
  }
  // Pad with blank spaces until the required length is reached
  while (len < required_length) {
    str[index++] = ' ';
    len++;
  }
  // Append ends_with to the end of the string
  int ends_with_len = strlen(ends_with);
  for (int i = 0; i < ends_with_len; i++) {
    str[index++] = ends_with[i];
  }
  len += ends_with_len;
  // Null terminate the string
  str[index] = '\0';
}

int has_prefix(char *str, char *prefix) {
  // Loop until the end of the prefix
  while (*prefix) {
    // If the characters are not equal, return 0
    if (*str != *prefix) {
      return 0;
    }
    // Move to the next character
    str++;
    prefix++;
  }
  // If the end of the prefix has been reached, return 1
  return 1;
}

void strcpy(char *dest, const char *src) {
  // Loop until the end of the source string
  while (*src) {
    // Copy the character from source to destination
    *dest = *src;
    // Move to the next character
    dest++;
    src++;
  }
  // Add a null character at the end of the destination string
  *dest = '\0';
}

void strncpy(char *dest, const char *src, int n) {
  // Loop until the end of the source string or n characters
  while (*src && n > 0) {
    // Copy the character from source to destination
    *dest = *src;
    // Move to the next character
    dest++;
    src++;
    // Decrement the count
    n--;
  }
  // Add a null character at the end of the destination string
  *dest = '\0';
}

void strncat(char *dest, const char *src, int n) {
  // Loop until the end of the destination string
  while (*dest) {
    // Move to the next character
    dest++;
  }
  // Loop until the end of the source string or n characters
  while (*src && n > 0) {
    // Copy the character from source to destination
    *dest = *src;
    // Move to the next character
    dest++;
    src++;
    // Decrement the count
    n--;
  }
  // Add a null character at the end of the destination string
  *dest = '\0';
}

void clrstr(char *str) {
  for (int i = 0; i < MAX_CMD_SIZE; i++) {
    // Add a null character at each index
    str[i] = '\0';
  }
}