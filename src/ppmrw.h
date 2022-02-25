#include <stdio.h>
#include <errno.h>

extern const int STD_STR_LEN;
extern const char NEW_LINE;
extern const char NULL_CHAR;

void fail(char *s, FILE *openPtr);
void writeP3(char *outputName, int *colors, int width, int height, int maxValue);
