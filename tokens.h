/* tokens.h */
#ifndef TOKENS_H
#define TOKENS_H

/*----------------------------------*/
/* Tokens for scanner & syntax tree */
/*----------------------------------*/

typedef enum
{ 
	T_NULL=0, T_EOF, T_ARRAY, T_BEGIN, T_BY, T_CASE, T_CONST, T_IDIV,
	T_DO, T_ELSE, T_ELSIF, T_END, T_EXIT, T_FOR, T_IMPORT, T_IF, T_LOOP,
	T_MODULE, T_MOD, T_NIL, T_OF, T_OR, T_POINTER, T_PROCEDURE, T_RECORD,
	T_REPEAT, T_RETURN, T_THEN, T_TO, T_TYPE, T_UNTIL, T_VAR, T_WHILE,
	T_IN, T_IS, T_WITH, T_IDENT, T_CHAR, T_INTEGER, T_REAL, T_STRING, 
	T_AND, T_CARET, T_RBRACE, T_RBRACK, T_RPAREN, T_EQUALS, T_COLON, 
	T_COMMA, T_EQU, T_RDIV, T_NEQ, T_SUB, T_LEQ, T_LES, T_LBRACE, T_LBRACK,
	T_LPAREN, T_PERIOD, T_ADD, T_GTE, T_GTR, T_SEMI, T_NOT, T_MUL, T_BAR,
	T_DOTDOT, T_SET, T_NEG, T_CALL, T_STATESEQ, T_ROOT, T_CONV, T_MONADIC 
} TOKEN;

#endif
