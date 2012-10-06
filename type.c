/* type.c */

#include <stdlib.h>
#include <stdio.h>
#include "scanner.h"
#include "symbol.h"
#include "type.h"
#include "error.h"

/*-----------*/
/* Constants */
/*-----------*/

enum
{
	TINIT = 1,		/* initial size of table   */
	TGROW = 2,		/* amount to grow table by */
	MAXSTRING = 256 /* max size of string constants */
};

/*---------*/
/* Globals */
/*---------*/

TypeTable ttab;

Type NIL_TYPE;
Type CHAR_TYPE;
Type BOOLEAN_TYPE;
Type SHORTINT_TYPE;
Type INTEGER_TYPE;
Type LONGINT_TYPE;
Type REAL_TYPE;

/*---------------------------------------------*/
/* Create a new type and return an index to it */
/*---------------------------------------------*/

Type type_add(KIND kind)
{
	TypeStruct *tp;

	if( ttab.type == NULL ) /* first time */
	{
		ttab.type = (TypeStruct*)malloc(TINIT*sizeof(TypeStruct));
		if( ttab.type == NULL )
			fatal_error("type_add: out of memory\n");
		ttab.max = TINIT;
		ttab.nval = 0;
	}
	else if( ttab.nval >= ttab.max ) /* grow */
	{
		tp = (TypeStruct*)realloc(ttab.type,
				(TGROW*ttab.max)*sizeof(TypeStruct));
		if( tp == NULL )
			fatal_error("type_add: out of memory\n");
		ttab.max *= TGROW;
		ttab.type = tp;
	}
	
	ttab.type[ttab.nval].kind     = kind;
	ttab.type[ttab.nval].size     = 1;
	ttab.type[ttab.nval].low      = 0;
	ttab.type[ttab.nval].high     = 0;
	ttab.type[ttab.nval].basetype = -1;
	ttab.type[ttab.nval].strconst = 0;
	ttab.type[ttab.nval].scope    = NULL;

	return ttab.nval++;
}

Type type_add_string(int len)
{
	Type t;
	
	if( len == 1 ) return CHAR_TYPE;

	t=type_add(ARRAY_KIND);	
	ttab.type[t].size = len;
	ttab.type[t].low  = 1;
	ttab.type[t].high = len+1;
	ttab.type[t].basetype = CHAR_TYPE;
	ttab.type[t].strconst = 1;
	
	return t;
}

Type type_check_int(int ival)
{
	if( ival <= 127 ) return SHORTINT_TYPE;
	if( ival <= 32767 ) return INTEGER_TYPE;
	return LONGINT_TYPE;
}

void type_check_assign(Type tv,Type te)
{
	if( tv==te ) return;
	if( is_numeric(tv) && is_numeric(te) && (te<=tv) ) return;
	if( ttab.type[tv].kind==POINTER_KIND && is_nil(te) ) return;
	if( is_string(tv) && ttab.type[te].strconst )
	{
		if( ttab.type[te].size <= ttab.type[tv].size ) return;
	}
	fatal_error("invalid types in assignment\n");
}


Type type_check_expr(int oper,Type t1,Type t2)
{
	switch(oper)
	{
	case T_EQU:
	case T_NEQ:
		if(	(is_char(t1)    && is_char(t2)   ) ||
		    (is_string(t1)  && is_string(t2) ) ||
		    (is_numeric(t1) && is_numeric(t2)) ||
			(is_boolean(t1) && is_boolean(t2)) ) 
		{
			return BOOLEAN_TYPE;
		}
		break;

	case T_LES:
	case T_LEQ:
	case T_GTR:
	case T_GTE:
		if(	(is_char(t1)    && is_char(t2)   ) ||
		    (is_string(t1)  && is_string(t2) ) ||
		    (is_numeric(t1) && is_numeric(t2)) )
		{
			return BOOLEAN_TYPE;
		}
		break;
		
	case T_ADD:
	case T_SUB:
	case T_MUL:
		if( is_numeric(t1) && is_numeric(t2) )
		{
			if(t1>t2) return t1;
			else      return t2;
		}
		break;

	case T_NEG:
		if( is_numeric(t1) )
		{
			return t1;
		}
		break;

	case T_RDIV:
		if( is_numeric(t1) && is_numeric(t2) )
		{
			return REAL_TYPE;
		}
		break;

	case T_MOD:
	case T_IDIV:
		if( is_integer(t1) && is_integer(t2) )
		{
			return INTEGER_TYPE;
		}
		break;

	case T_OR:
	case T_AND:
		if( is_boolean(t1) && is_boolean(t2) ) 
		{
			return BOOLEAN_TYPE;
		}
		break;

	case T_NOT:
		if( is_boolean(t1) )
		{
			return BOOLEAN_TYPE;
		}
		break;		
	}
		
	fatal_error("invalid types in expression\n");
	
	return INTEGER_TYPE;
}

void type_init(void)
{
	/* These MUST stay in order */

	NIL_TYPE = type_add(SIMPLE_KIND);
	ttab.type[NIL_TYPE].size = 0;

	BOOLEAN_TYPE = type_add(SIMPLE_KIND);
	ttab.type[BOOLEAN_TYPE].size = 1;

	CHAR_TYPE = type_add(SIMPLE_KIND);
	ttab.type[CHAR_TYPE].size = 1;

	SHORTINT_TYPE = type_add(SIMPLE_KIND);
	ttab.type[SHORTINT_TYPE].size = 1;

	INTEGER_TYPE = type_add(SIMPLE_KIND);
	ttab.type[INTEGER_TYPE].size = 2;

	LONGINT_TYPE = type_add(SIMPLE_KIND);
	ttab.type[LONGINT_TYPE].size = 4;

	REAL_TYPE = type_add(SIMPLE_KIND);
	ttab.type[REAL_TYPE].size = 4;
}

void type_print(Type typ,Const val)
{
	printf("Type=");

	if(typ==NIL_TYPE)
	{
		printf("NIL\n");
	}
	else if(typ==BOOLEAN_TYPE)
	{
		printf("BOOLEAN, ");
		if(val.ival==0)
			printf("Value==FALSE\n");
		else
			printf("Value==TRUE\n");
	}
	else if(typ==SHORTINT_TYPE)
	{
		printf("SHORTINT, ");	
		printf("Value==%d\n",val.ival);
	}
	else if(typ==INTEGER_TYPE)
	{
		printf("INTEGER, ");
		printf("Value==%d\n",val.ival);
	}
	else if(typ==LONGINT_TYPE)
	{
		printf("LONGINT, ");
		printf("Value==%d\n",val.ival);
	}
	else if(typ==REAL_TYPE)
	{
		printf("REAL, ");
		printf("Value==%f\n",val.rval);
	}
	else if(typ==CHAR_TYPE)
	{
		printf("CHAR, ");
		printf("Value==%s\n",val.sval);
	}
	else
	{
		if(is_string(typ))
		{
			printf("STRING, ");
			printf("Value==%s\n",val.sval);
		}
		else
			printf("type_print: ERROR\n");
	}
}
