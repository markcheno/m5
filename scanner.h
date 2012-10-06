/* scanner.h */
#ifndef SCANNER_H
#define SCANNER_H

#include "tokens.h"

typedef struct {
    char *file_name;
    int line_number;
    int char_number;
} Position_in_File;

typedef struct {
    	TOKEN 	class;
	int 	ival;
	char 	*sval;
	float   rval;
    Position_in_File pos;
} Token_Type;

extern Token_Type Token;
extern void start_lex(char *filename);
extern void next_token(void);
extern void skip(int token);
extern void match(int token);

#endif
