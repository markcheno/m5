/* main.c */

#include <stdio.h>
#include "ast.h"
#include "code_pseudo.h"
#include "parser.h"

int main(int argc,char *args[])
{
	AstPtr ast_root;
	char *source_file=NULL;
	
	if(argc>0) source_file=args[1];
	ast_root=module(source_file);
	code_pseudo(ast_root,"m5.lis");

	return 0;
}
