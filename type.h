#ifndef TYPE_H
#define TYPE_H

/*-----------*/
/* Constants */
/*-----------*/

typedef enum
{
	SIMPLE_KIND, ARRAY_KIND, RECORD_KIND, POINTER_KIND, PROCEDURE_KIND
} KIND;

#define is_simple_kind(obj)    (ttab.type[obj->type].kind==SIMPLE_KIND)
#define is_array_kind(obj)     (ttab.type[obj->type].kind==ARRAY_KIND)
#define is_record_kind(obj)    (ttab.type[obj->type].kind==RECORD_KIND)
#define is_pointer_kind(obj)   (ttab.type[obj->type].kind==POINTER_KIND)
#define is_procedure_kind(obj) (ttab.type[obj->type].kind==PRODCEDURE_KIND)

/*-------------------*/
/* Type table record */
/*-------------------*/

typedef int Type;

typedef struct TypeStruct
{
	KIND	kind;		/* kind of structure  */
	int 	nelem;		/* number of elements */
	int		size;		/* size of structure  */
	int		low;		/* array: lower range */
	int		high;		/* array: upper range */
	Type	basetype;	/* elementary type    */
	int     strconst;	/* true for string constants */
	SymPtr	scope; 		/* record: fields procedure: locals */
} TypeStruct;

typedef struct TypeTable
{
	int	nval;			/* current number of types        */
	int	max;			/* allocated number of values     */
	TypeStruct *type;	/* growable array of types        */
} TypeTable;

/*------------------*/
/* Public interface */
/*------------------*/

extern TypeTable ttab;
extern Type NIL_TYPE;
extern Type CHAR_TYPE;
extern Type BOOLEAN_TYPE;
extern Type SHORTINT_TYPE;
extern Type INTEGER_TYPE;
extern Type LONGINT_TYPE;
extern Type REAL_TYPE;

#define is_nil(t)     (t==NIL_TYPE)
#define is_char(t)    (t==CHAR_TYPE)
#define is_string(t)  (ttab.type[t].kind==ARRAY_KIND && ttab.type[t].basetype==CHAR_TYPE)
#define is_boolean(t) (t==BOOLEAN_TYPE)
#define is_integer(t) (t==SHORTINT_TYPE||t==INTEGER_TYPE||t==LONGINT_TYPE)
#define is_real(t)    (t==REAL_TYPE)
#define is_numeric(t) (is_integer(t)||is_real(t))
#define is_record(t)  (ttab.type[t].kind==RECORD_KIND)

Type   type_add(KIND kind);
Type   type_add_string(int len);
Type   type_check_expr(int oper,Type t1,Type t2);
Type   type_check_int(int ival);
void   type_check_assign(Type tv,Type te);
void   type_init(void);
void   type_print(Type typ,Const val);

#endif
