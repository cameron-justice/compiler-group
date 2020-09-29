
%debug
%verbose	/*generate file: tiger.output to check grammar*/
%{
#include <iostream>
#include "ErrorMsg.h"
#include <FlexLexer.h>

int yylex(void);		/* function prototype */
void yyerror(char *s);	//called by the parser whenever an eror occurs

%}

%union {
	int		ival;	//integer value of INT token
	string* sval;	//pointer to name of IDENTIFIER or value of STRING	
					//I have to use pointers since C++ does not support 
					//string object as the union member
}

/* TOKENs and their associated data type */
%token <sval> ID STRING
%token <ival> INT

%token 
  COMMA COLON SEMICOLON LPAREN RPAREN LBRACK RBRACK 
  LBRACE RBRACE DOT 
  ARRAY IF THEN ELSE WHILE FOR TO DO LET IN END OF
  ASSIGN
  BREAK NIL
  FUNCTION VAR TYPE 

%right ASSIGN
%left AND OR
%nonassoc EQ NEQ GT LT GE LE
%left PLUS MINUS
%left TIMES DIVIDE
%left UMINUS

%start program

%%

program	:	exp

exp		:	INT		
		|	MINUS INT
		|	STRING	
		|	NIL								
		|	lvalue	
		|	exp AND exp						
		|	exp OR exp						
		|	exp EQ exp						
		|	exp NEQ exp						
		|	exp GT exp						
		|	exp LT exp						
		|	exp GE exp						
		|	exp LE exp						
		|	UMINUS exp
		|	exp PLUS exp
		|	exp MINUS exp
		|	exp TIMES exp
		|	exp DIVIDE exp
		|	lvalue ASSIGN exp				
		|	lvalue LPAREN RPAREN				
		|	lvalue LPAREN explist RPAREN
		|	LPAREN RPAREN
		|	LPAREN expseq RPAREN						
		|	IF exp THEN exp					
		|	IF exp THEN exp ELSE exp	
		|	WHILE exp DO exp				
		|	FOR lvalue ASSIGN exp TO exp DO exp
		|	BREAK							
		|	LET decs IN expseq END	
		|	LET decs IN END			
		|	error

expseq  :	exp								
		|	expseq SEMICOLON exp			

explist	:	exp										
		|	explist COMMA exp					

lvalue	:	ID								
		|	ID arr_access

arr_access	:	LBRACK exp RBRACK
			|	LBRACK exp RBRACK arr_access
		
decs	:	dec
		|	decs dec

dec		:	vardec
		|	typedec

vardec	:	VAR lvalue ASSIGN exp
		|	VAR lvalue COLON type ASSIGN exp
		|	VAR lvalue ASSIGN type

typedec	:	TYPE lvalue EQ type

type	:	LBRACE RBRACE
		|	LBRACE typeflds RBRACE
		|	lvalue OF exp
		|	ARRAY OF type
		|	lvalue

typeflds	:	typefld
			|	typeflds typefld

typefld	:	lvalue COLON lvalue

%%
extern yyFlexLexer	lexer;
int yylex(void)
{
	return lexer.yylex();
}

void yyerror(char *s)
{
	extern int	linenum;			//line no of current matched token
	extern int	colnum;
	extern void error(int, int, std::string);

	error(linenum, colnum, s);
}

