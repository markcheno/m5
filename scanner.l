%{
#include "scanner.h"
#include "error.h"

Token_Type Token;
int line_number=1;

static void invalid_text(char *text);
static void reserved(char *word);
static void skip_comments(void);

%}

whitespace          [ \t]
digit               [0-9]
hexDigit			({digit}|[A-F])
letter              [a-zA-Z]
underscore          "_"
letter_or_digit     ({letter}|{digit})
underscored_tail    ({underscore}{letter_or_digit}+)
ident               ({letter}{letter_or_digit}*{underscored_tail}*)
integer				{digit}+
hexInteger			{digit}{hexDigit}*H
scale_factor		((E|D)(("+"|"-")?){digit}+)
real				{digit}+"."({digit}*)({scale_factor}?)

%%

{whitespace}		{/* ignore whitespace */}
"\n"                {line_number++;}
ARRAY|array			{return T_ARRAY;}
BEGIN|begin			{return T_BEGIN;}
BY|by				{return T_BY;}
CASE|case			{return T_CASE;}
CONST|const			{return T_CONST;}
DIV|div				{return T_IDIV;}
DO|do				{return T_DO;}
ELSE|else			{return T_ELSE;}
ELSIF|elsif			{return T_ELSIF;}
END|end				{return T_END;}
EXIT|exit			{return T_EXIT;}
FOR|for				{return T_FOR;}
IMPORT|import		{return T_IMPORT;}
IF|if				{return T_IF;}
LOOP|loop			{return T_LOOP;}
MODULE|module		{return T_MODULE;}
MOD|mod				{return T_MOD;}
NIL|nil				{return T_NIL;}
OF|of				{return T_OF;}
OR|or				{return T_OR;}
POINTER|pointer		{return T_POINTER;}
PROCEDURE|procedure	{return T_PROCEDURE;}
RECORD|record		{return T_RECORD;}
REPEAT|repeat		{return T_REPEAT;}
RETURN|return		{return T_RETURN;}
THEN|then			{return T_THEN;}
TO|to				{return T_TO;}
TYPE|type			{return T_TYPE;}
UNTIL|until			{return T_UNTIL;}
VAR|var				{return T_VAR;}
WHILE|while			{return T_WHILE;}
IN|in				{reserved(yytext);}
IS|is				{reserved(yytext);}
WITH|with			{reserved(yytext);}
{ident}				{return T_IDENT;}
{integer}			{return T_INTEGER;}
{hexInteger}		{return T_INTEGER;}
{real}				{return T_REAL;}
\"[^"]*\"			{return T_STRING;}
\'[^']*\'			{return T_STRING;}
"(*"				{skip_comments();}
"&"					{return T_AND;}
"^"					{return T_CARET;}
"}"					{return T_RBRACE;}
"]"					{return T_RBRACK;}
")"					{return T_RPAREN;}
":="				{return T_EQUALS;}
":"					{return T_COLON ;}
","					{return T_COMMA;}
"="					{return T_EQU;}
"/"					{return T_RDIV;}
"#"					{return T_NEQ;}
"-"					{return T_SUB;}
"<="				{return T_LEQ;}
"<"					{return T_LES;}
"{"					{return T_LBRACE;}
"["					{return T_LBRACK;}
"("					{return T_LPAREN;}
"."					{return T_PERIOD;}
"+"					{return T_ADD;}
">="				{return T_GTE;}
">"					{return T_GTR;}
";"					{return T_SEMI;}
"~"					{return T_NOT;}
"*"					{return T_MUL;}
"|"					{return T_BAR;}
".."				{reserved(yytext);}
.					{invalid_text(yytext);}

%%

void start_lex(char *filename)
{
	if(filename==NULL)
	{
		yyin=stdin;
	}
	else
	{
		yyin=fopen(filename,"r");
	}
	
	next_token();
}

void next_token(void)
{
	Token.class = yylex();
	
	if(Token.class==0)
	{
		Token.class = T_EOF; 
		Token.sval  = "<EOF>"; 
		return;
    }
	else if(Token.class==T_INTEGER)
	{
		Token.ival=strtoul(yytext,NULL,10);
	}
	else if(Token.class==T_REAL)
	{
		Token.rval=strtod(yytext,NULL);
	}

	Token.sval=yytext;
	
	Token.pos.line_number = line_number;
	
	/* printf("token = '%s'\n",Token.sval); */
	/* strcpy(Token.sval=(char*)malloc(strlen(yytext)+1),yytext); */
}

void skip(int token)
{
	match(token);		
	next_token();
}

void match(int token)
{
	if(Token.class!=token)
	{
		fatal_error("scanner: line %d: expected token - %d\n",line_number,token);
	}
}


int yywrap(void) {return 1;}

static void invalid_text(char *text)
{
	fatal_error("scanner: line %d: invalid text '%s'\n",line_number,text);
}

static void reserved(char *word)
{
	fatal_error("scanner: line %d: reserved word '%s'\n",line_number,word);
}

static void skip_comments(void)
{
	int c;

	while( c=input(), c != 0 )
	{
		if(c=='(')
		{
			c=input();
			if(c==0) 
				goto unterminated_comment;
			else if(c=='*')
				skip_comments();
		    else
				unput(c);
		}
		else if(c=='*')
		{
			c=input();
			if(c==0)
				goto unterminated_comment;
			else if(c==')')
				break;
			else
				unput(c);
		}
	
		if(c=='\n') line_number += 1;
	}
	if(c!=0) return;

unterminated_comment:
    fatal_error("scanner: line %d: unterminated comment",line_number);
}
