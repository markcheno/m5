/* ast.c */

#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

AstPtr ast_new(TOKEN token)
{
	AstPtr ast=calloc(1,sizeof(AstNode));
	
	ast->token=token;

	return ast;
}

void ast_del(AstPtr node)
{
	free(node);
}

void ast_link_walk(AstPtr ast,void (process)(AstPtr node) )
{
	if( ast==NULL ) return;
	process(ast);
	if( ast->link!=NULL ) 
		ast_link_walk(ast->link,process);
}

void ast_postorder_walk(AstPtr ast,void (process)(AstPtr node) )
{
	if( ast==NULL ) return;
	ast_postorder_walk(ast->left,process);
	ast_postorder_walk(ast->right,process);
	process(ast);
}

void ast_preorder_walk(AstPtr ast,void (process)(AstPtr node) )
{
	if( ast==NULL ) return;
	process(ast);
	ast_postorder_walk(ast->left,process);
	ast_postorder_walk(ast->right,process);
}
