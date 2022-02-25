#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

//Constants
const int STD_STR_LEN = 200;
const char NEW_LINE = '\n';
const char NULL_CHAR = '\0';
int NUM_CHANNELS = 3;
int P3 = 301;
int P6 = 302;
char COMMENT = '#';


void writeP3(char *outputName, int *colors, int width, int height, int maxValue){
    FILE *outputPtr = fopen(outputName, "wb");

    //Setup header for P3 file
    fprintf(outputPtr, "P3\n");
    fprintf(outputPtr, "%d %d\n", width, height);
    fprintf(outputPtr, "%d\n", maxValue);

    //Write out all color values to file
    for(int index = 0; index < width*height*3;index++){
      fprintf(outputPtr, "%d\n", colors[index]);
    }
    fclose(outputPtr);
  }
