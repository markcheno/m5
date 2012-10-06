/* error.c */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "error.h"
#include "scanner.h"

void fatal_error(const char *fmt, ... )
{
	va_list ap;
  
	va_start(ap,fmt);
	vfprintf(stderr,fmt,ap);
	va_end(ap);
	fprintf(stderr,"at line number: %d\n",Token.pos.line_number);
	exit(1);
}
