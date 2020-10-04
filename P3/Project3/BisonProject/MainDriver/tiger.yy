/* PROGRAMMER: Cameron Justice */
/* PROGRAM: Project 3 */
/* DUE: October 5th, 2020 */
/* INSTRUCTOR: Dr. Zhijiang Dong */


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

exp		:	STRING											/* string literal */
		|	INT												/* integer literal */
		|	NIL												/* null value */
		|	lvalue											/* assignable location */
		|	ARRAY											/* array token */
		|	MINUS exp										/* negative */
		|	exp AND exp										/* -- START BINARY OPERATORS */
		|	exp OR exp
		|	exp EQ exp
		|	exp NEQ exp
		|	exp GT exp
		|	exp LT exp
		|	exp GE exp
		|	exp LE exp										/* -- END BINARY OPERATORS */
		|	exp PLUS exp									/* -- START MATH OPERATORS */
		|	exp MINUS exp
		|	exp TIMES exp
		|	exp DIVIDE exp									/* -- END MATH OPERATORS */
		|	lvalue ASSIGN exp								/* assignment statement */
		|	ID LPAREN explist RPAREN						/* function call */
		|	LPAREN expseq RPAREN							/* expressions in parentheses */
		|	IF exp THEN exp									/* if */
		|	IF exp THEN exp ELSE exp						/* if else */
		|	WHILE exp DO exp								/* while loop */
		|	FOR ID ASSIGN exp TO exp DO exp					/* for loop */
		|	BREAK											/* break */
		|	LET declist IN expseq END						/* variables + program */
		|	error											/* catch errors thrown and continue parsing */

lvalue	:	ID												
		|	lvalue DOT ID									/*  method call */
		|	ID arr_acc										/* array access */

arr_acc	:	LBRACK exp RBRACK 
		|	LBRACK exp RBRACK arr_acc						/* multidimensional array access */
				
explist	:													/* optionality */
		|	exp												
		|	explist COMMA exp								

expseq	:													/* optionality */
		|	exp 
		|	expseq SEMICOLON exp

declist :	dec
		|	declist dec

dec		:	typedec
		|	vardec

typedec	:	TYPE ID EQ exp									/* type definition */
		|	TYPE ID EQ ARRAY OF exp							/* type array of type */
		|	TYPE ID EQ lvalue OF exp						/* type array */

vardec	:	VAR ID ASSIGN exp								/* variable definition */
		|	VAR ID ASSIGN exp OF exp						/* array definition */

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

