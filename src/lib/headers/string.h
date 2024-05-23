int strcmp(const char *str1, const char *str2);
void strcpy(char *dest, const char *src);
void strncpy(char *dest, const char *src, int n);
void skip_spaces(char **str);
int strlen(const char *str);
void trim_leading_zeros(char *str);
void trim(char *str);
int strlen_no_color(const char *str);
int str2int(char *str);
void int2str(int num, char *str);
void ll2str(long long num, char *str);
void reverse(char *str);
void append_str(char *str, char *new_str);
void str_auto_newline(char *str, int max_length, char *starts_with,
                      char *ends_with, int required_length);
int has_prefix(char *str, char *prefix);
void strncat(char *dest, const char *src, int n);
void clrstr(char *str);
