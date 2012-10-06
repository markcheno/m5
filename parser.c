/* parser.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scanner.h"
#include "symbol.h"
#include "ast.h"
#include "type.h"
#include "error.h"
#include "parser.h"

static void   import_list(void);
static void   import_spec(void);
static void   var_type_decl(void);
static void   const_decl(void);
static void   type_decl(void);
static void   var_decl(void);
static Type   type(void);
static Type   array_type(void);
static Type   record_type(void);
static void   field_list(void);
static Type   pointer_type(void);
static Type   qual_ident(void);
static AstPtr procedure_decl(void);
static Type   formal_pars(void);
static void   fp_section(void);
static SymPtr ident_def(void);
static AstPtr statement_seq(void);
static AstPtr statement(void);
static AstPtr if_statement(void);
static AstPtr case_statement(void);
static AstPtr case_stmt(void);
static AstPtr while_statement(void);
static AstPtr repeat_statement(void);
static AstPtr for_statement(void);
static AstPtr loop_statement(void);
static AstPtr exit_statement(void);
static AstPtr return_statement(void);
static AstPtr assign_statement(void);
static AstPtr expr(void);
static AstPtr simple_expr(void);
static AstPtr term(void);
static AstPtr factor(void);
static AstPtr designator(void);
static AstPtr arguments(void);
static AstPtr expr_list(void);
static AstPtr build_expr_ast(int oper,AstPtr left,AstPtr right);
static Type   const_expr(Type expected,Const *result);
static Type   const_expr1(void);
static Type   const_simple_expr(void);
static Type   const_term(void);
static Type   const_factor(void);

#define in_expr_start(t) \
	(t==T_EQU   || t==T_NEQ    || t==T_LES    || \
	 t==T_LEQ   || t==T_GTR    || t==T_GTE    || \
	 t==T_ADD   || t==T_SUB    || t==T_OR     || \
	 t==T_MUL   || t==T_RDIV   || t==T_IDIV   || \
	 t==T_MOD   || t==T_AND    || t==T_CHAR   || \
	 t==T_IDENT || t==T_REAL   || t==T_STRING || \
	 t==T_NOT   || t==T_RPAREN || t==T_INTEGER )

#define in_decl_start(t) \
	(t==T_CONST || t==T_TYPE || t==T_VAR)

void *module(char *sourcefile)
{
	SymPtr sym; 
	AstPtr ast,last=NULL; 
	
	sym_init(10000);
	start_lex(sourcefile);
	skip(T_MODULE);
	ast=ast_new(T_MODULE);

	match(T_IDENT);
	sym=sym_add(Token.sval); 
	sym->mode=MODULE_MODE;	
	ast->object=sym;
	skip(T_IDENT);	
	skip(T_SEMI);

	if(Token.class==T_IMPORT)
	{
		import_list();
	}
	
	while(in_decl_start(Token.class))
	{
		var_type_decl();
	}

	while(Token.class==T_PROCEDURE)
	{
		AstPtr proc=procedure_decl();
		if(proc!=NULL)
		{
			if(ast->left==NULL)
			{
				ast->left=proc;
				last=proc;
			} 
			else 
				last->link=proc;

			last=proc;
		}	
	}

	skip(T_BEGIN);
	ast->right=statement_seq();
	skip(T_END);
	skip(T_IDENT);
	skip(T_PERIOD);
	
	return ast;
}

static void import_list(void)
{
	skip(T_IMPORT);
	import_spec();
	while(Token.class==T_COMMA)
	{
		skip(T_COMMA);
		import_spec();
	} 
	skip(T_SEMI);
}

static void import_spec(void)
{
	skip(T_IDENT);
	if(Token.class==T_EQUALS)
	{
		skip(T_EQUALS);
		skip(T_IDENT);
	}
}

/*--------------*/
/* Declarations */
/*--------------*/

static void var_type_decl(void)
{
	switch(Token.class)
	{
	case T_CONST:
		skip(T_CONST);
		while(Token.class==T_IDENT)
		{
			const_decl();
			skip(T_SEMI);
		}
		break;
		
	case T_TYPE:
		skip(T_TYPE);
		while(Token.class==T_IDENT)
		{
			type_decl();
			skip(T_SEMI);
		}
		break;
		
	case T_VAR:
		skip(T_VAR);
		while(Token.class==T_IDENT)
		{
			var_decl();
			skip(T_SEMI);
		}
		break;
		
	default: 
		break;
	}
}

static void const_decl(void)
{
	SymPtr sym;
	
	sym=ident_def();
	skip(T_EQU);

	sym->type=const_expr(NIL_TYPE,&sym->value);
	
	/* type_print(sym->type,sym->value); */
}

static void type_decl(void)
{
	SymPtr sym;
	
	sym=ident_def();
	skip(T_EQU);
	sym->type=type();
	sym->mode=TYPE_MODE;
}

static void var_decl(void)
{
	int sp=0;
	Type typ;
	SymPtr sym,st[10];

	sym=ident_def();
	st[sp++]=sym;
	
	while(Token.class==T_COMMA)
	{
		skip(T_COMMA);
		sym=ident_def();
		st[sp++]=sym;
	} 
	
	skip(T_COLON);
	typ=type();
	
	while(sp)
	{
		sym=st[--sp];
		sym->mode=VARIABLE_MODE; 
		sym->type=typ;
	}
}

/*-------*/
/* Types */
/*-------*/

static Type type(void)
{
	Type typ=0;

	switch(Token.class)
	{
	case T_ARRAY:	typ=array_type();	break;
	case T_RECORD:	typ=record_type();	break;
	case T_POINTER:	typ=pointer_type();	break;
	case T_IDENT:	typ=qual_ident();	break;
	default:		break;
	}

	return typ;
}

static Type array_type(void)
{
	Const low,high;
	Type basetype,typ;
	
	skip(T_ARRAY);
	const_expr(INTEGER_TYPE,&low);
	
	skip(T_COMMA);
	const_expr(INTEGER_TYPE,&high);
	
	skip(T_OF);
	
	basetype=type();
	
	typ=type_add(ARRAY_KIND);
	ttab.type[typ].low=low.ival;
	ttab.type[typ].high=high.ival;
	ttab.type[typ].basetype=basetype;
	ttab.type[typ].size=(high.ival-low.ival)*ttab.type[basetype].size;
	
	return typ;
}

static Type record_type(void)
{
	Type typ;
	
	typ=type_add(RECORD_KIND);
	sym_enter_scope();
	
	skip(T_RECORD);
	field_list();

	while(Token.class==T_SEMI)
	{
		skip(T_SEMI);
		field_list();
	}
	skip(T_END);
	
	ttab.type[typ].scope=sym_exit_scope();
	
	return typ;
}

static void field_list(void)
{
	int sp=0;
	Type typ;
	SymPtr sym,st[10];

	if(Token.class!=T_IDENT) return;
	
	sym=sym_add(Token.sval);
	skip(T_IDENT);
	
	st[sp++]=sym;
	while(Token.class==T_COMMA)
	{
		skip(T_COMMA);
		match(T_IDENT);
		sym=sym_add(Token.sval); 
		skip(T_IDENT);
		st[sp++]=sym;
	}
	skip(T_COLON);
	typ=type();
	
	/* TODO: keep track of number of 
	   elements and allocation size here */
	while(sp)
	{
		sym=st[--sp];
		sym->mode=VARIABLE_MODE; 
		sym->type=typ;
	}
}

static Type pointer_type(void)
{
	Type typ;
	
	skip(T_POINTER);
	skip(T_TO);
	typ=type_add(POINTER_KIND);
	ttab.type[typ].basetype=type();
	
	return typ;
}

static Type qual_ident(void)
{
	SymPtr sym;
	
	match(T_IDENT);
	sym=sym_find(Token.sval);
	if(sym==NULL) fatal_error("identifier not found: '%s'\n",Token.sval);
	skip(T_IDENT);

	return sym->type;
}

/*------------*/
/* Procedures */
/*------------*/

static AstPtr procedure_decl(void)
{
	SymPtr sym;
	AstPtr ast;
	
	skip(T_PROCEDURE);
	sym=ident_def();
	sym->mode=PROCEDURE_MODE; 
	ast=ast_new(T_PROCEDURE);
	ast->object=sym;

	sym_enter_scope();

	sym->type=ast->type=formal_pars();

	skip(T_SEMI);

	while(in_decl_start(Token.class))
	{
		var_type_decl();
	}
	
	if(Token.class==T_BEGIN)
	{
		skip(T_BEGIN); 
		ast->right=statement_seq();
	} 

	skip(T_END); 
	skip(T_IDENT);
	skip(T_SEMI);

	ttab.type[sym->type].scope=sym_exit_scope();
	
	return ast;
}

static Type formal_pars(void)
{
	Type typ=NIL_TYPE;

	if(Token.class!=T_LPAREN)
	{
		typ=type_add(PROCEDURE_KIND);
		return typ;
	}
	skip(T_LPAREN);
	
	if(Token.class==T_VAR || Token.class==T_IDENT)
	{
		fp_section();
		while(Token.class==T_SEMI)
		{
			skip(T_SEMI);
			fp_section();
		}
	}

	skip(T_RPAREN);
	
	if(Token.class==T_COLON)
	{
		skip(T_COLON);
		typ=qual_ident();
	}
	else
		typ=type_add(PROCEDURE_KIND);
		
	return typ;
}

static void fp_section(void)
{
	int sp=0;
	Type typ;
	SymPtr sym,tmp,st[10];
	
	if(Token.class==T_VAR)
	{
		skip(T_VAR);
		/* TODO: byref=true */
	}
	
	match(T_IDENT);
	sym=sym_add(Token.sval);
	skip(T_IDENT);
	
	tmp=sym;
	st[sp++]=sym;
	while(Token.class==T_COMMA)
	{
		skip(T_COMMA);
		match(T_IDENT);
		sym=sym_add(Token.sval); 
		tmp->next=sym;
		tmp=st[sp++]=sym;
		skip(T_IDENT);
	} 

	skip(T_COLON);
	typ=type();
	
	while(sp)
	{
		sym=st[--sp];
		sym->mode=VARIABLE_MODE; 
		sym->type=typ;
	}
}

static SymPtr ident_def(void)
{
	SymPtr sym;
	
	match(T_IDENT);
	sym=sym_add(Token.sval);	
	skip(T_IDENT);
	
	return sym;
}

/*------------*/
/* Statements */
/*------------*/

static AstPtr statement_seq(void)
{
	AstPtr ast,seq,last;
	
	ast=ast_new(T_STATESEQ);
	
	seq=statement();
	ast->left=last=seq; 
	while(Token.class==T_SEMI)
	{
		skip(T_SEMI);
		seq=statement();
		last->link=seq; 
		last=seq;
	}
	
	return ast;
}

static AstPtr statement(void)
{
	AstPtr ast=NULL;
	
	switch(Token.class)
	{
	case T_IDENT:	ast=assign_statement();	break;
	case T_IF:		ast=if_statement();		break;
	case T_CASE:	ast=case_statement();	break;
	case T_WHILE:	ast=while_statement();	break;
	case T_REPEAT:	ast=repeat_statement();	break;
	case T_FOR: 	ast=for_statement();	break;
	case T_LOOP:	ast=loop_statement();	break;
	case T_EXIT:	ast=exit_statement();	break;
	case T_RETURN:	ast=return_statement();	break;
	default:		break;
	}
	
	return ast;
}

static AstPtr if_statement(void)
{
	AstPtr ast,tmp;
	
	skip(T_IF);	   
	ast=ast_new(T_IF); 
	ast->expr=expr();

	skip(T_THEN);
	ast->left=ast_new(T_THEN);
	ast->left->left=statement_seq(); 

	tmp=ast->left;
	while(Token.class==T_ELSIF)
	{
		AstPtr elsif_ast;
		
		skip(T_ELSIF);
		elsif_ast=ast_new(T_ELSIF);
		elsif_ast->expr=expr();
		skip(T_THEN);
		elsif_ast->left=statement_seq();
   		tmp->link=elsif_ast;
		tmp=elsif_ast;
	}
	   
	if(Token.class==T_ELSE)
	{
		AstPtr else_ast;
		
		skip(T_ELSE);
		else_ast=ast_new(T_ELSE);
		else_ast->left=statement_seq();
		ast->link=else_ast;
	}

	skip(T_END);
	
	return ast;
}

static AstPtr case_statement(void)
{
	AstPtr seq,last=NULL;
	AstPtr case_ast,of_ast;
	
	skip(T_CASE);
	case_ast=ast_new(T_CASE);	   
	case_ast->expr=expr();
	skip(T_OF);
	
	of_ast=case_stmt();

	if(of_ast!=NULL)
	{
		case_ast->left=of_ast;
		last=of_ast;
	}

	while(Token.class==T_BAR)
	{
		skip(T_BAR);
		of_ast=case_stmt();
		if(of_ast!=NULL)
		{
			if(case_ast->left==NULL)
			{
				case_ast->left=of_ast;
				last=of_ast;
			}
			else
			{
				last->link=of_ast;
			}
			last=of_ast;
		}
	}

	if(Token.class==T_ELSE)
	{
		AstPtr else_ast;
		
		skip(T_ELSE);
		seq=statement_seq();
		else_ast=ast_new(T_ELSE);
		else_ast->left=seq;
		last->link=else_ast;
	}

	skip(T_END);
	
	return case_ast;
}

static AstPtr case_stmt(void)
{
	AstPtr exp,last,ast=NULL;
		
	if( !in_expr_start(Token.class) )
		return NULL;

	ast=ast_new(T_OF);

	ast->expr=ast_new(T_INTEGER);
	const_expr(INTEGER_TYPE,&ast->expr->value);
	
	last=ast->expr;
	while(Token.class==T_COMMA)
	{
		skip(T_COMMA);
		exp=ast_new(T_INTEGER);
		const_expr(INTEGER_TYPE,&exp->value);
		last->link=exp;
		last=exp;
	}
		
	skip(T_COLON);
	ast->left=statement_seq();
	
	return ast;
}

static AstPtr while_statement(void)
{
	AstPtr ast;
	
	skip(T_WHILE);
	ast=ast_new(T_WHILE);
	ast->expr=expr();
	skip(T_DO);
	ast->left=statement_seq();
	skip(T_END);

	return ast;
}

static AstPtr repeat_statement(void)
{
	AstPtr ast;
	
	skip(T_REPEAT);
	ast=ast_new(T_REPEAT);
	ast->left=statement_seq();
	skip(T_UNTIL);
	ast->right=expr();

	return ast;
}

static AstPtr for_statement(void)
{
	AstPtr ast;
	
	skip(T_FOR);
	ast=ast_new(T_FOR);
	ast->left=assign_statement();

	skip(T_TO);
	ast->right=ast_new(T_TO);
	ast->right->expr=expr();

	if(Token.class==T_BY)
	{
		skip(T_BY);
		ast->right->left=ast_new(T_BY);
		const_expr(INTEGER_TYPE,&ast->right->left->value);
	}

	skip(T_DO);
	ast->right->link=statement_seq();
	skip(T_END);
	
	return ast;
}

static AstPtr loop_statement(void)
{
	AstPtr ast;
	
	skip(T_LOOP);
	ast=ast_new(T_LOOP);
	ast->left=statement_seq();
	skip(T_END);
	
	return ast;
}

static AstPtr exit_statement(void)
{
	AstPtr ast;
	
	skip(T_EXIT);
	ast=ast_new(T_EXIT);
	
	return ast;
}

static AstPtr return_statement(void)
{
	AstPtr ast;

	ast=ast_new(T_RETURN);
	skip(T_RETURN);

	if( in_expr_start(Token.class) )
	{
		ast->expr=expr();
	}

	return ast;
}

static AstPtr assign_statement(void)
{
	AstPtr ast,exp=NULL,args=NULL;
	
	ast=designator();
	
	if(Token.class==T_EQUALS)
	{
		skip(T_EQUALS);
		exp=expr();
	}
	else if(Token.class==T_LPAREN)
	{
		args=arguments();
	}
	
	if( is_procedure_mode(ast->object) )
	{
		ast->token=T_CALL;
		ast->expr=args;
	}
	else
	{
		ast->token=T_EQUALS;
		type_check_assign(ast->type,exp->type);
		if( is_numeric(ast->type) && exp->type < ast->type )
		{
			ast->expr=ast_new(T_CONV);
			ast->expr->type=ast->type;
			ast->expr->left=exp;
		}
		else
			ast->expr=exp;
	}
	
	return ast;
}

/*-------------*/
/* Expressions */
/*-------------*/

static AstPtr expr(void)
{
	int oper=T_MONADIC;
	AstPtr ast,t1=NULL,t2=NULL;

	t1=simple_expr();

	switch(Token.class)
	{
	case T_EQU: 
		next_token(); 
		t2=simple_expr(); 
		oper=T_EQU; 
		break;
		
	case T_NEQ: 
		next_token(); 
		t2=simple_expr(); 
		oper=T_NEQ; 
		break;
		
	case T_LES: 
		next_token(); 
		t2=simple_expr(); 
		oper=T_LES; 
		break;
		
	case T_LEQ: 
		next_token(); 
		t2=simple_expr(); 
		oper=T_LEQ; 
		break;
		
	case T_GTR: 
		next_token(); 
		t2=simple_expr(); 
		oper=T_GTR; 
		break;
		
	case T_GTE: 
		next_token(); 
		t2=simple_expr(); 
		oper=T_GTE; 
		break;
		
	default: break;
	}
	
	ast=build_expr_ast(oper,t1,t2);
	
	return ast;
}	

static AstPtr simple_expr(void)
{
	int oper=T_MONADIC,neg=0;
	AstPtr ast,t1=NULL,t2=NULL;

	if(Token.class==T_ADD)
		skip(T_ADD);
		
	if(Token.class==T_SUB)
	{ 
		skip(T_SUB); neg=1;
	}

	t1=term();

	if(neg) t1=build_expr_ast(T_NEG,t1,NULL);

	switch(Token.class)
	{
	case T_ADD: 
		next_token(); 
		t2=term(); 
		oper=T_ADD; 
		break;
		
	case T_SUB: 
		next_token(); 
		t2=term(); 
		oper=T_SUB; 
		break;
		
	case T_OR:  
		next_token(); 
		t2=term(); 
		oper=T_OR;  
		break;
		
	default: break;
	}	

	ast=build_expr_ast(oper,t1,t2);
	
	return ast;
}

static AstPtr term(void)
{
	int oper=T_MONADIC;
	AstPtr ast,t1=NULL,t2=NULL;
	
	t1=factor();

	switch(Token.class)
	{
	case T_MUL:   
		next_token(); 
		t2=factor(); 
		oper=T_MUL; 
		break;
		
	case T_RDIV: 
		next_token(); 
		t2=factor(); 
		oper=T_RDIV; 
		break;
		
	case T_IDIV:   
		next_token(); 
		t2=factor(); 
		oper=T_IDIV; 
		break;
		
	case T_MOD:   
		next_token(); 
		t2=factor(); 
		oper=T_MOD; 
		break;
		
	case T_AND: 
		next_token(); 
		t2=factor(); 
		oper=T_AND; 
		break;
		
	default: break;
	}
	
	ast=build_expr_ast(oper,t1,t2);
	
	return ast;
}
	
static AstPtr factor(void)
{
	AstPtr ast,t1,args=NULL;
	
	switch(Token.class)
	{
	case T_INTEGER:
		ast=ast_new(T_INTEGER);
	    ast->value.ival=Token.ival;
		ast->type=type_check_int(Token.ival);
		next_token();
		break;
	
	case T_REAL:
		ast=ast_new(T_REAL);
	    ast->value.rval=Token.rval;
	    ast->type=REAL_TYPE;
		next_token();
		break;
			
	case T_STRING:
		ast=ast_new(T_STRING);
	    ast->value.sval=Token.sval;
		ast->type=type_add_string(strlen(Token.sval));
		next_token();
		break;
	
	case T_NOT:
		skip(T_NOT);
		t1=factor();
		ast=build_expr_ast(T_NOT,t1,NULL);
		break;
	
	case T_LPAREN:
		skip(T_LPAREN);
		ast=expr();
		skip(T_RPAREN);
		break;
	
	default:
		ast=designator();
		args=arguments();
		if(is_procedure_mode(ast->object))
		{
	       ast->token=T_CALL;
		   ast->expr=args;
	    }
		break;
	}
	
	return ast;
}

static AstPtr designator(void)
{
	SymPtr sym; 
	AstPtr ast,index=NULL; 
	
	ast=ast_new(T_IDENT);

	match(T_IDENT);	
	sym=sym_find(Token.sval);
	if(sym==NULL) 
		fatal_error("identifier not found: '%s'\n",Token.sval);
	skip(T_IDENT);
	
	while(Token.class==T_PERIOD ||
	      Token.class==T_LBRACK ||
		  Token.class==T_CARET)
	{ 
		switch(Token.class)
		{
		case T_PERIOD:
			skip(T_PERIOD);
			match(T_IDENT);
			sym=sym_find_in_scope(Token.sval,ttab.type[sym->type].scope);
			if(sym==NULL) 
				fatal_error("identifier not found: '%s'\n",Token.sval);
			skip(T_IDENT);
			break;
			
		case T_LBRACK:
			skip(T_LBRACK);
			index=expr_list();
			skip(T_RBRACK);
			break;
			
		case T_CARET:
			/* TODO */
			break;
		
		default: break;
		}
	}
	
	ast->object=sym;
	ast->type=sym->type;
	ast->expr=index;
	if(is_constant_mode(sym))
	{
    	ast->value.uval = sym->value.uval;
	}
		
	return ast;
}

static AstPtr arguments(void)
{
	AstPtr ast=NULL;
	
	if(Token.class==T_LPAREN)
	{
		skip(T_LPAREN);
		ast=expr_list();
		skip(T_RPAREN);
	}
	
	return ast;
}

static AstPtr expr_list(void)
{
	AstPtr ast,exp,last;

	ast=exp=last=expr();
	
	while(Token.class==T_COMMA)
	{
		skip(T_COMMA);
		exp=expr();
		last->link=exp;
		last=exp;
	}
	
	return ast;	
}

static AstPtr build_expr_ast(int oper,AstPtr left,AstPtr right)
{
	AstPtr ast=NULL;
	Type right_type=0;

	if(oper==T_MONADIC) return left;
	
	ast=ast_new(oper);
	
	if(right!=NULL ) right_type=right->type;

	/* determine the result type */
	ast->type=type_check_expr(oper,left->type,right_type);

	/* promote left side if needed */
	if( is_numeric(ast->type) && left->type < right_type )
	{
		ast->left=ast_new(T_CONV);
		ast->left->type=ast->type;
		ast->left->left=left;
	}
	else
		ast->left=left;

	/* bail if no right side */
	if( right_type==0 )	return ast;
	
	/* promote right side if needed */
	if( is_numeric(ast->type) && right_type < left->type )
	{
		ast->right=ast_new(T_CONV);
		ast->right->type=ast->type;
		ast->right->right=right;
	}
	else
		ast->right=right;

	return ast;
}

/*----------------------*/
/* Constant Expressions */
/*----------------------*/

static int   const_sp;
static Const const_stack[50];

#define pushr(r)	(const_stack[const_sp++].rval=r)
#define pushs(s) 	(const_stack[const_sp++].sval=s)
#define popr()  	(const_stack[--const_sp].rval)
#define pops()  	(const_stack[--const_sp].sval)

static Type const_expr(Type expected,Const *result)
{
	Type typ;
	float r1;
	
	const_sp=0;

	typ=const_expr1();

	const_sp--;
	if(typ==INTEGER_TYPE || typ==BOOLEAN_TYPE)
	{
		r1=const_stack[const_sp].rval;
		const_stack[const_sp].ival=(int)r1;
	}

	if(const_sp==0)
	{
		result->uval=const_stack[const_sp].uval;
	}

	if(expected>0 && (typ!=expected) )
		fatal_error("expected INTEGER constant\n");
	
	return typ;
}

static Type const_expr1(void)
{
	int cmp;
	float r1,r2;
	char *s1,*s2;
	Type t1=0,t2=0;
	
	t1=const_simple_expr();

	switch(Token.class)
	{
	case T_EQU: 
		next_token(); 
		t2=const_simple_expr();
		t1=type_check_expr(T_EQU,t1,t2);
		if(is_string(t2))
		{
			s1=pops(); s2=pops();
			cmp=strcmp(s1,s2);
			pushr(cmp==0 ? 1.0 : 0.0);
		}
		else
		{
			r1=popr(); r2=popr();
			pushr(r2==r1 ? 1.0 : 0.0);
		}
		break;
		
	case T_NEQ:
		next_token(); 
		t2=const_simple_expr(); 
		t1=type_check_expr(T_NEQ,t1,t2);
		if(is_string(t2))
		{
			s1=pops(); s2=pops();
			cmp=strcmp(s1,s2);
			pushr(cmp!=0 ? 1.0 : 0.0);
		}
		else
		{
			r1=popr(); r2=popr();
			pushr(r2!=r1 ? 1.0 : 0.0);
		}
		break;
		
	case T_LES:
		next_token(); 
		t2=const_simple_expr(); 
		t1=type_check_expr(T_LES,t1,t2);
		if(is_string(t2))
		{
			s1=pops(); s2=pops();
			cmp=strcmp(s1,s2);
			pushr(cmp>0 ? 1.0 : 0.0);
		}
		else
		{
			r1=popr(); r2=popr();
			pushr(r2<r1 ? 1.0 : 0.0);
		}
		break;
		
	case T_LEQ:
		next_token(); 
		t2=const_simple_expr(); 
		t1=type_check_expr(T_LEQ,t1,t2);
		if(is_string(t2))
		{
			s1=pops(); s2=pops();
			cmp=strcmp(s1,s2);
			pushr(cmp>=0 ? 1.0 : 0.0);
		}
		else
		{
			r1=popr(); r2=popr();
			pushr(r2<=r1 ? 1.0 : 0.0);
		}
		break;
		
	case T_GTR:
		next_token(); 
		t2=const_simple_expr(); 
		t1=type_check_expr(T_GTR,t1,t2);
		if(is_string(t2))
		{
			s1=pops(); s2=pops();
			cmp=strcmp(s1,s2);
			pushr(cmp<0 ? 1.0 : 0.0);
		}
		else
		{
			r1=popr(); r2=popr();
			pushr(r2>r1 ? 1.0 : 0.0);
		}
		break;
		
	case T_GTE:
		next_token(); 
		t2=const_simple_expr(); 
		t1=type_check_expr(T_GTE,t1,t2);
		if(is_string(t2))
		{
			s1=pops(); s2=pops();
			cmp=strcmp(s1,s2);
			pushr(cmp<=0 ? 1.0 : 0.0);
		}
		else
		{
			r1=popr(); r2=popr();
			pushr(r2>=r1 ? 1.0 : 0.0);
		}
		break;
		
	default: break;
	}
		
	return t1;
}	

static Type const_simple_expr(void)
{
	int neg=0;
	float r1,r2;
	Type t1=0,t2=0;

	if(Token.class==T_ADD)   skip(T_ADD);
	if(Token.class==T_SUB) { skip(T_SUB); neg=1; }

	t1=const_term();

	if(neg)
	{
		r1=popr();
		pushr(r1*-1.0);
	}

	switch(Token.class)
	{
	case T_ADD:
		next_token();
		t2=const_term();
		t1=type_check_expr(T_ADD,t1,t2);
		r1=popr(); r2=popr();
		pushr(r2+r1);
		break;
		
	case T_SUB: 
		next_token(); 
		t2=const_term();
		t1=type_check_expr(T_SUB,t1,t2);
		r1=popr(); r2=popr();		
		pushr(r2-r1);
		break;
		
	case T_OR:
		next_token(); 
		t2=const_term(); 
		t1=type_check_expr(T_OR,t1,t2);
		r1=popr(); r2=popr();
		pushr(r2+r1);
		break;
		
	default: break;
	}

	return t1;
}

static Type const_term(void)
{
	float r1,r2;
	Type t1=0,t2=0;
	
	t1=const_factor();

	switch(Token.class)
	{
	case T_MUL:   
		next_token(); 
		t2=const_factor();
		t1=type_check_expr(T_MUL,t1,t2);
		r1=popr(); r2=popr();
		pushr(r2*r1);
		break;
		
	case T_RDIV:
		next_token(); 
		t2=const_factor();
		t1=type_check_expr(T_RDIV,t1,t2);
		r1=popr(); r2=popr();
		pushr(r2/r1);
		break;
		
	case T_IDIV:
		next_token();
		t2=const_factor();
		t1=type_check_expr(T_IDIV,t1,t2);
		r1=popr(); r2=popr();
		pushr((float)((int)(r2/r1)));
		break;
		
	case T_MOD:
		next_token();
		t2=const_factor(); 
		t1=type_check_expr(T_MOD,t1,t2);
		r1=popr(); r2=popr();
		pushr((int)r2%(int)r1);
		break;

	case T_AND:
		next_token(); 
		t2=const_factor();
		t1=type_check_expr(T_AND,t1,t2);
		r1=popr(); r2=popr();
		(r1>0.0 && r2>0.0) ? pushr(1.0) : pushr(0.0);
		break;
		
	default: break;
	}
	
	return t1;
}
	
static Type const_factor(void)
{
	float r1;
	Type typ;
	AstPtr ast;
	
	switch(Token.class)
	{
	case T_INTEGER:
	    pushr((float)Token.ival);
		next_token();
		typ=INTEGER_TYPE;
		break;
	
	case T_REAL:
	    pushr(Token.rval);
		next_token();
		typ=REAL_TYPE;
		break;
		
	case T_STRING:
		pushs(sym_strdup(Token.sval));
		typ=type_add_string(strlen(Token.sval));
		next_token();
		break;
		
	case T_NOT:
		skip(T_NOT);
		const_factor();
		r1=popr();
		(r1==0.0) ? pushr(1.0) : pushr(0.0);
		typ=BOOLEAN_TYPE;
		break;
	
	case T_LPAREN:
		skip(T_LPAREN);
		typ=const_expr1();
		skip(T_RPAREN);
		break;
	
	default:
		ast=designator();
		if(is_constant_mode(ast->object))
		{
			if(is_integer(ast->type))		pushr((float)ast->value.ival);
			else if(is_real(ast->type))		pushr(ast->value.rval);
			else if(is_char(ast->type))		pushs(ast->value.sval);
			else if(is_string(ast->type))	pushs(ast->value.sval);
		}
		else fatal_error("ident must be constant\n");
		typ=ast->type;
		ast_del(ast); /* not used */
		break;
	}
	return typ;
}
