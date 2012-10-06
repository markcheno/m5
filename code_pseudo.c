/* code_pseudo.c */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "code_pseudo.h"
#include "tokens.h"
#include "ast.h"
#include "error.h"

/* Private */
static FILE *outfile; 
static int  exit_num=0;
static int  exit_label[100];

/* Prototypes */
static int  emit(const char *fmt, ... );
static int  new_label(void);
static void check_exit(void);
static void push_args(AstPtr ast);
static void push(AstPtr ast);
static void store(AstPtr ast);
static void expression(AstPtr ast);
static void statement(AstPtr ast);


static int emit(const char *fmt, ... )
{
	int i; 
	va_list ap;
  
	va_start(ap,fmt);
	i = vfprintf(outfile,fmt,ap);
	va_end(ap);
  
	return i;
}

static int new_label(void)
{
	static int label=0;

	return label++;
}

static void check_exit(void)
{
	while( --exit_num >= 0 )
	{
		emit("exit_%lu:\n",exit_label[exit_num]);
	}
	exit_num=0;
}

static void push_args(AstPtr ast)
{
	if(ast==NULL) return;
	ast_postorder_walk(ast,expression);
	if(ast->link!=NULL)
		push_args(ast->link);
}

static void push(AstPtr ast)
{
	emit("\tpush");

	if( ast->type==BOOLEAN_TYPE  ) emit(".b\t");
	if( ast->type==SHORTINT_TYPE ) emit(".s\t");
	if( ast->type==INTEGER_TYPE  ) emit(".i\t");
	if( ast->type==LONGINT_TYPE  ) emit(".l\t");
	if( ast->type==REAL_TYPE     ) emit(".r\t");
	if( is_string(ast->type)     ) emit(".l\t");

	switch(ast->token)
	{
	case T_INTEGER: emit("%d\n",ast->value.ival);   break;
	case T_REAL:    emit("%f\n",ast->value.rval);   break;
	case T_STRING:  emit("'%s'\n",ast->value.sval); break;
	case T_IDENT:   emit("%s\n",ast->object->name); break;		
	default: break;
	}
}

static void store(AstPtr ast)
{
	switch(ttab.type[ast->object->type].kind)
	{
	case SIMPLE_KIND:
	    emit("\tstore\t%s\n",ast->object->name);
		break;
		
	case ARRAY_KIND: /* TODO */
	    emit("\tstore\t%s\n",ast->object->name);
		break;
		
	case RECORD_KIND:
		break;
		
	case POINTER_KIND:
		break;

	case PROCEDURE_KIND:
		break;
	}
}

static void expression(AstPtr ast)
{
	switch(ast->token)
	{
	case T_EQU:		  	emit("\teql"); 	break;
	case T_NEQ:		  	emit("\tneq"); 	break;
	case T_LES:		  	emit("\tles"); 	break;
	case T_LEQ:		  	emit("\tleq"); 	break;
	case T_GTR:		  	emit("\tgtr"); 	break;
	case T_GTE:		  	emit("\tgte"); 	break;
	case T_ADD:		  	emit("\tadd"); 	break;
	case T_SUB:		  	emit("\tsub"); 	break;
	case T_OR:		  	emit("\tor");  	break;
	case T_MUL:		  	emit("\tmul"); 	break;
	case T_IDIV:	  	emit("\tidiv"); break;
	case T_RDIV: 	  	emit("\trdiv"); break;
	case T_MOD:		  	emit("\tmod"); 	break;
	case T_AND: 	  	emit("\tand"); 	break;
	case T_NOT:		  	emit("\tnot"); 	break;
	case T_NEG:		  	emit("\tneg"); 	break;
	case T_IDENT:	  	push(ast); return; break;
	case T_INTEGER:  	push(ast); return; break;
	case T_REAL:	 	push(ast); return; break;
	case T_STRING:  	push(ast); return; break;

	case T_CONV:	
		if(ast->type==INTEGER_TYPE)
			emit("\ttoint\n");
		else if(ast->type==LONGINT_TYPE)
			emit("\ttolong\n");
		else if(ast->type==REAL_TYPE)
			emit("\ttoreal\n");
		return;
		break;
	
	case T_CALL:
		push_args(ast->expr);
		emit("\tcall\t%s\n",ast->object->name); 
		return; 
		break;
		
	default:
		fatal_error("code_pseudo:expression: unknown token - %d\n",ast->token); 
		break;
	}

	if( ast->type==BOOLEAN_TYPE  ) emit(".b\n");
	if( ast->type==SHORTINT_TYPE ) emit(".s\n");
	if( ast->type==INTEGER_TYPE  ) emit(".i\n");
	if( ast->type==LONGINT_TYPE  ) emit(".l\n");
	if( ast->type==REAL_TYPE     ) emit(".r\n");
}

static void statement(AstPtr ast)
{
	if(ast==NULL) return;
	
	switch(ast->token)
	{
	case T_MODULE:
		emit("; module\n");
		emit("%s:\n",ast->object->name);
		ast_link_walk(ast->left,statement);
		statement(ast->right);
		break;
	
	case T_PROCEDURE:
		ast_postorder_walk(ast->left,expression);
		emit("%s:\n",ast->object->name);
		statement(ast->right);
		emit("\tret\n");
		break;
		
	case T_STATESEQ:
		ast_link_walk(ast->left,statement);
		break;

	case T_EQUALS:
		emit("; assign\n");
		ast_postorder_walk(ast->expr,expression);
		store(ast);
		break;

	case T_CALL:		
		emit("; call\n");
		push_args(ast->expr);
		emit("\tcall\t%s\n",ast->object->name); 
		break;

	case T_IF:
	{
		long end_label=new_label();
		long next_label=new_label();
		
		emit("; if\n");
		ast_postorder_walk(ast->expr,expression);
		emit("\tjz\t\tifnext_%lu\n",next_label);
		statement(ast->left->left);
		emit("\tjmp\t\tendif_%lu\n",end_label);

		ast=ast->left->link;
		while(ast!=NULL)
		{
			if(ast->token==T_ELSIF)
			{
				emit("ifnext_%lu:\n",next_label);
				next_label = new_label();
				ast_postorder_walk(ast->expr,expression);
				emit("\tjz\t\tifnext_%lu\n",next_label);
				statement(ast->left);
				emit("\tjmp\t\tendif_%lu\n",end_label);
			}
			else if(ast->token==T_ELSE)
			{
				emit("ifnext_%lu:\n",next_label);
				statement(ast->left);
				next_label = new_label();
			}
			ast=ast->link;
		}

		emit("ifnext_%lu:\n",next_label);
		emit("endif_%lu:\n",end_label);
		break; 
	}

	case T_CASE:
	{
		long end_label=new_label();
		
		emit("; case\n");
		ast_postorder_walk(ast->expr,expression);

		ast=ast->left;
		while(ast!=NULL)
		{
			long stmt_label = new_label();
			long next_label = new_label();

			if(ast->token==T_OF)
			{
				AstPtr exp=ast->expr;
				while(exp!=NULL)
				{
					emit("\tdup\n");
					/* ast_postorder_walk(exp,expression); */
					emit("\tpush\t%d\n",exp->value.ival);
					emit("\teq\n");
					emit("\tjnz\t\tcase_stmt_%lu\n",stmt_label);
					exp=exp->link;
				}
				emit("\tjmp\t\tcase_next_%lu\n",next_label);
				emit("case_stmt_%lu:\n",stmt_label);
				statement(ast->left);
				emit("\tjmp\t\tendcase_%lu\n",end_label);
			}
			else if(ast->token==T_ELSE)
			{
				statement(ast->left);	
			}
			ast=ast->link;
			emit("case_next_%lu:\n",next_label);
		}
		emit("endcase_%lu:\n",end_label);		
		emit("\tpop\n");
		break;
	}

	case T_WHILE:
	{
		long label = new_label();
		emit("while_%lu:\n",label);
		ast_postorder_walk(ast->expr,expression);
		emit("\tjz\t\tendwhile_%lu\n",label);
		statement(ast->left);
		emit("\tjmp\t\twhile_%lu\n",label);			
		emit("endwhile_%lu:\n",label);
		check_exit();
		break;
	}
	
	case T_REPEAT:
	{
		long label = new_label();
		emit("repeat_%lu:\n",label);
		statement(ast->left);
		ast_postorder_walk(ast->right,expression);
		emit("\tjz\t\trepeat_%lu\n",label);
		check_exit();
		break;
	}

	case T_FOR:
	{
		char *control_var;
		long top_label=new_label();
		long bot_label=new_label();
		
		emit("; for\n");
		
		statement(ast->left);
		control_var=ast->left->object->name;
		emit("for_%lu:\n",top_label);
		ast_postorder_walk(ast->right->expr,expression);
		emit("\tpush\t%s\n",control_var);
		emit("\teq\n");
		emit("\tjnz\t\tendfor_%lu\n",bot_label);
			
		if( ast->right->left!=NULL ) /* BY expr */
		{
			emit("\tpush\t%s\n",control_var);
			emit("\tpush\t%d\n",ast->right->left->value.ival);
			/* ast_postorder_walk(ast->right->left->expr,expression); */
			emit("\tadd\n");
			emit("\tstore\t%s\n",control_var);
		}
		else
		{
			emit("\tpush\t%s\n",control_var);
			emit("\tinc\n");				
			emit("\tstore\t%s\n",control_var);
		}
		
		statement(ast->right->link);
		emit("\tjmp\t\tfor_%lu\n",top_label);
		emit("endfor_%lu:\n",bot_label);
		check_exit();
		break;
	}

	case T_LOOP:
	{
		long label=new_label();
		emit("loop_%lu:\n",label);
		statement(ast->left);
		emit("\tjmp\t\tloop_%lu\n",label);
		check_exit();
		break;
	}

	case T_EXIT:
	{
		long label = new_label();
		exit_label[exit_num++] = label;
		emit("\tjmp\t\texit_%lu\n",label);
		break;
	}

	case T_RETURN:
		emit("; return\n");
		break;

	default:
		fatal_error("code_pseudo:statement: unknown token - %d\n",ast->token);
		break;
	}
}

void code_pseudo(void *ast_root,char *output_file)
{
	if( (outfile=fopen(output_file,"w"))==NULL )
	{
		fatal_error("code_pseudo: unable to open output file for writing\n"); 
	}
	statement(ast_root);
	fclose(outfile); 
}
