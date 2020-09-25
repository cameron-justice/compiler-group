
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
%type <ival> exp expseq explist lvalue

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

exp		:	INT								{ $$ = $1;						}
		|	STRING							{ $$ = $1;}						
		|	NIL								{ $$ = NULL;					}	
		|	lvalue							{}
		|	exp AND exp						{ $$ = $1 && $3;				}
		|	exp OR exp						{ $$ = $1 || $3;				}
		|	exp EQ exp						{ $$ = $1 == $3;				}
		|	exp NEQ exp						{ $$ = $1 != $3;				}
		|	exp GT exp						{ $$ = $1 >  $3;				}
		|	exp LT exp						{ $$ = $1 <  $3;				}
		|	exp GE exp						{ $$ = $1 >= $3;				}
		|	exp LE exp						{ $$ = $1 <= $3;				}
		|	UMINUS exp 						{ $$ = -$2;						}
		|	lvalue ASSIGN exp				{ $$ = $1 = $3;					}
		|	ID LPAREN RPAREN				{ $1();					}
		|	ID LPAREN explist RPAREN		{ $$ = $1( $3 );				}
		|	LPAREN expseq RPAREN			{ ( $2 );					}			
		|	IF exp THEN exp					{ if($2){$$ = $4;}				}
		|	IF exp THEN exp ELSE exp		{ if($2){$$ = $4;}else{$$ = $6;}		}
		|	WHILE exp DO exp				{ while($2){$4;};			}
		|	FOR ID ASSIGN exp TO exp DO exp { for($2 = $4; $6;){$8;}	}
		|	BREAK							{ break;					}
		|	LET decs IN explist END			{	}
		|	LET decs IN END					{	}

expseq  :	exp								{ $$ = $1; }
		|	expseq SEMICOLON exp			{ $$ = $1; $3				}

explist	:	exp								{ $$ = $1;					}		
		|	explist COMMA exp				{ $$ = $1 , $3;				}	

lvalue	:	ID								{ $1;					}
		|	lvalue LBRACK exp RBRACK		{ $1[$3];				}
		
decs	:	dec
		|	decs dec

dec		:	vardec
		|	fundec

vardec	:	VAR ID ASSIGN exp				{ }
fundec	:	FUNCTION ID LPAREN RPAREN		{}

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

