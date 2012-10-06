/* symbol.c */

#include <stdlib.h>
#include <string.h>
#include "symbol.h"
#include "error.h"
#include "type.h"

/*---------*/
/* Private */
/*---------*/

#define MAX_NESTING_LEVEL 10

static int    scope_level=0;
static SymPtr scope_stack[MAX_NESTING_LEVEL];

static char  *string_table;
static char  *string_ptr;
static int    string_size=0;

/*------------------------------------------------------------*/
/* Initialize scope stack, string table, and predefined types */
/*------------------------------------------------------------*/

void sym_init(int s_size)
{
	int ii;
	SymPtr obj;
	
	for(ii=0;ii<MAX_NESTING_LEVEL;ii++)
		scope_stack[ii]=NULL;

	string_table = (char*)calloc(s_size,sizeof(char));
	if(string_table==NULL)
		fatal_error("sym: Cannot allocate string table\n");
	
	string_size = s_size;
	string_ptr  = string_table;
	
	type_init();

	obj=sym_add("NIL");
	obj->mode=CONSTANT_MODE;
	obj->type=NIL_TYPE;

	obj=sym_add("CHAR");
	obj->mode=TYPE_MODE;
	obj->type=CHAR_TYPE;
	
	obj=sym_add("BOOLEAN");
	obj->mode=TYPE_MODE;
	obj->type=BOOLEAN_TYPE;
	
	obj=sym_add("TRUE");
	obj->mode=CONSTANT_MODE;
	obj->type=BOOLEAN_TYPE;
	obj->value.ival=1;
	
	obj=sym_add("FALSE");
	obj->mode=CONSTANT_MODE;
	obj->type=BOOLEAN_TYPE;
	obj->value.ival=0;

	obj=sym_add("SHORTINT");
	obj->mode=TYPE_MODE;
	obj->type=SHORTINT_TYPE;

	obj=sym_add("INTEGER");
	obj->mode=TYPE_MODE;
	obj->type=INTEGER_TYPE;

	obj=sym_add("LONGINT");
	obj->mode=TYPE_MODE;
	obj->type=LONGINT_TYPE;

	obj=sym_add("REAL");
	obj->mode=TYPE_MODE;
	obj->type=REAL_TYPE;	
}

/*----------------------------------------------*/
/* Create a new symbol and add to current scope */
/*----------------------------------------------*/

SymPtr sym_add(char *name)
{
	int cmp;
	SymPtr new_node,node,*npp;

	if(sym_find_in_scope(name,scope_stack[scope_level])!=NULL)
		fatal_error("duplicate symbol: %s\n",name);

	new_node = calloc(1,sizeof(Sym));
	
	new_node->name=sym_strdup(name);

	npp=&scope_stack[scope_level];

	while( (node=*npp) != NULL )
	{
		cmp = strcmp(name,node->name);
		npp = cmp < 0 ? &(node->left) : &(node->right);
	}
	
	*npp=new_node;
	
	return new_node;
}

/*---------------------------------------------------*/
/* Search backward thru all scopes for a symbol name */
/*---------------------------------------------------*/

SymPtr sym_find(char *name)
{
	int ii;
	SymPtr node;
	
	for(ii=scope_level; ii>=0; --ii)
	{
		node=sym_find_in_scope(name,scope_stack[ii]);
		if(node!=NULL) return node;
	}
	
	return NULL;
}

/*----------------------------------------*/
/* Search specified scope only for a name */
/*----------------------------------------*/

SymPtr sym_find_in_scope(char *name,SymPtr scope)
{
	int cmp;
	
	while(scope!=NULL)
	{
		cmp=strcmp(name,scope->name);
		if(cmp==0) return scope;
		scope = cmp < 0 ? scope->left : scope->right;
	}

	return NULL;
}

/*-------------------------*/
/* Start a new scope level */
/*-------------------------*/

void sym_enter_scope(void)
{
	if( ++scope_level > MAX_NESTING_LEVEL )
		fatal_error("sym: nesting too deep\n");
		
	scope_stack[scope_level]=NULL;
}

/*-----------------------------*/
/* Remove top scope from stack */
/*-----------------------------*/

SymPtr sym_exit_scope(void)
{
	SymPtr scope=scope_stack[scope_level--];
	
	return scope;
}

/*----------------------------------------------------------------*/
/* Add a string to the string table and return a pointer to it.   */
/* Bump the pointer into the string table to next avail position. */
/*----------------------------------------------------------------*/

char *sym_strdup(char *s)
{
	char *start=string_ptr;

	while( *s != '\0' )
	{
		if( string_ptr >= &(string_table[string_size-2]) )
			fatal_error("sym: string table overflow\n");

		*string_ptr++ = *s++;
	}
	*string_ptr++ = '\0';

	return start;
}
