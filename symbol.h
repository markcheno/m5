/* symbol.h */
#ifndef SYMBOL_H
#define SYMBOL_H

/*-----------*/
/* Constants */
/*-----------*/

typedef enum
{
	CONSTANT_MODE,TYPE_MODE,VARIABLE_MODE,PROCEDURE_MODE,MODULE_MODE
} MODE;

#define is_constant_mode(obj)  (obj->mode==CONSTANT_MODE)
#define is_type_mode(obj)      (obj->mode==TYPE_MODE)
#define is_variable_mode(obj)  (obj->mode==VARIABLE_MODE)
#define is_procedure_mode(obj) (obj->mode==PROCEDURE_MODE)
#define is_module_mode(obj)    (obj->mode==MODULE_MODE)

/*---------------------*/
/* Symbol table record */
/*---------------------*/

#define	_bit_mask(N) 		(1 << ((N) % 32))
#define	bit_test(Set, N) 	(Set &   _bit_mask(N))
#define	bit_set(Set, N) 	(Set |=  _bit_mask(N)) 
#define	bit_clear(Set, N)  	(Set &= ~_bit_mask(N))  

typedef union
{
	char* sval;
	float rval;
	int   ival;
	unsigned int uval;
	unsigned int set;
} Const;

typedef struct Sym  Sym;
typedef Sym* SymPtr;
struct Sym
{
	/* minimum definition */
	char*  name;
	SymPtr left;
	SymPtr right;

	/* info */
	MODE  	mode;		/* constant,type,variable,procedure,module */
	int   	type;		/* type table index */
	Const 	value; 		/* constant: value  */
	SymPtr  next;		/* for chaining formal parameters */
};

/*------------------*/
/* Public interface */
/*------------------*/

void   sym_init(int s_size);
SymPtr sym_add(char *name);
SymPtr sym_find(char *name);
SymPtr sym_find_in_scope(char *name,SymPtr scope);
void   sym_enter_scope(void);
SymPtr sym_exit_scope(void);
char*  sym_strdup(char *s);

#endif
