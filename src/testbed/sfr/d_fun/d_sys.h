#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sys/stat.h"


// file
FILE *openfs();
FILE *openfc();


// endian
int getEndian();

int intSwap();
float floatSwap();


// string
int file_data_line();

int str2farr();
int str2iarr();

int strrepa();


// others
float deg2rad();
int mkdir_p();




