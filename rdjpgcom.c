#include <stdio.h>

static FILE *infile; /*input jpeg file*/

#define NEXTBYTE() getc(infile)
