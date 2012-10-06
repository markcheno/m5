/* ast.h */
#ifndef AST_H
#define AST_H

#include "tokens.h"
#include "symbol.h"
#include "type.h"

typedef struct AstNode AstNode;
typedef AstNode* AstPtr;

struct AstNode
{
	/* minimum definition */
	TOKEN	token;
	AstPtr 	link;
	AstPtr	left;
	AstPtr 	right;

	/* info */
	AstPtr 	expr;
	SymPtr	object;
	Type	type;
	Const   value;
};

/*------------------*/
/* public interface */
/*------------------*/

AstPtr ast_new(TOKEN token);
void   ast_del(AstPtr node);
void   ast_link_walk(AstPtr ast,void (process)(AstPtr node));
void   ast_postorder_walk(AstPtr ast,void (process)(AstPtr node));
void   ast_preorder_walk(AstPtr ast,void (process)(AstPtr node));

#endif
