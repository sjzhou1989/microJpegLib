#include <stdio.h>

#define JFREAD(file, buff, sizeofbuff) \
	fread((buff), 1, (sizeofbuff), (file))
#define JFFLUSH(file) fflush(file)
#define JFERROR(file) ferror(file)
