/* PROGRAMMER: Cameron Justice		*/
/* PROGRAM #: Program 2				*/
/* DUE DATE: Monday, 9/21/20		*/
/* INSTRUCTOR: Dr. Zhijiang Dong	*/

%option noyywrap
%option c++

%{
#include <iostream>
#include <string>
#include <sstream>
#include "tiger.tab.hh"
#include "ErrorMsg.h"

using std::string;
using std::stringstream;

ErrorMsg			errormsg;	//error handler

int		comment_depth = 0;	// depth of the nested comment
string	value = "";			// the value of current string
string	comment_line = "";

int			beginLine=-1;	//beginning line no of a string or comment
int			beginCol=-1;	//beginning column no of a string or comment

int		linenum = 1;		//beginning line no of the current matched token
int		colnum = 1;			//beginning column no of the current matched token
int		tokenCol = 1;		//column no after the current matched token

//the following defines actions that will be taken automatically after 
//each token match. It is used to update colnum and tokenCol automatically.
#define YY_USER_ACTION {colnum = tokenCol; tokenCol=colnum+yyleng;}

int string2int(string);			//convert a string to integer value
void newline(void);				//trace the line #
void error(int, int, string);	//output the error message referring to the current token
%}

ALPHA		[A-Za-z]
DIGIT		[0-9]
INT			[0-9]+
IDENTIFIER	{ALPHA}(({ALPHA}|{DIGIT}|"_")*)

%x STR
%x CMT

%%
" "				{}
\t				{}
\b				{}
\n				{newline(); }

"/*"			{ comment_depth++;  beginLine = linenum; beginCol = colnum; BEGIN(CMT); }
<CMT>"*"+"/"	{ if(comment_depth > 0) comment_depth--; else error(linenum, colnum, "closing nonexistant comment"); 
				  if(comment_depth == 0) BEGIN(INITIAL); }
<CMT>"/*"		{ comment_depth++; }
<CMT>\n			{ newline(); }
<CMT>[^(*/)]	{ }
<CMT><<EOF>>	{error(beginLine, beginCol, "unclosed comment"); yyterminate(); }

<INITIAL>\"		{ value = ""; beginLine = linenum; beginCol = colnum; BEGIN(STR); }
<STR>\"			{ yylval.sval = new string(value); BEGIN(INITIAL); return STRING; }
<STR>\\n		{ value += '\n'; }
<STR>\\\"		{ value += '\"'; }
<STR>\\\\		{ value += '\\'; }
<STR>\\t		{ value += '\t'; }
<STR>\\.		{ error(linenum, colnum, string(YYText()) + " illegal token");}
<STR>.			{ value += YYText(); }
<STR>\n			{ error(beginLine, beginCol, "unclosed string"); newline(); yylval.sval = new string(value); BEGIN(INITIAL); return STRING; }

","				{ return COMMA; }
":"				{ return COLON; }
";"				{ return SEMICOLON; }
"("				{ return LPAREN; }
")"				{ return RPAREN; }
"["				{ return LBRACK; }
"]"				{ return RBRACK; }
"{"				{ return LBRACE; }
"}"				{ return RBRACE; }
"."				{ return DOT; }
"+"				{ return PLUS; }
"-"				{ return MINUS; }
"*"				{ return TIMES; }
"/"				{ return DIVIDE; }
"="				{ return EQ; }
"<>"			{ return NEQ; }
"<" 			{ return LT; }
"<="			{ return LE; }
">" 			{ return GT; }
">="			{ return GE; }
"&"				{ return AND; }
"|"				{ return OR; }
":="			{ return ASSIGN; }

"array"			{ return ARRAY; }
"if"			{ return IF; }
"then"			{ return THEN; }
"else"			{ return ELSE; }
"while"			{ return WHILE; }
"for"			{ return FOR; }
"to"			{ return TO; }
"do"			{ return DO; }
"let"			{ return LET; }
"in"			{ return IN; }
"end"			{ return END; }
"of"			{ return OF; }
"break"			{ return BREAK; }
"nil"			{ return NIL; }
"function"		{ return FUNCTION; }
"var"			{ return VAR; }
"type"			{ return TYPE; }

{IDENTIFIER} 	{ value = YYText(); yylval.sval = new string(value); return ID; }
{INT}		 	{ yylval.ival = string2int(YYText()); return INT; }

<<EOF>>			{	yyterminate(); }
.				{	error(linenum, colnum, string(YYText()) + " illegal token");}

%%

int string2int( string val )
{
	stringstream	ss(val);
	int				retval;

	ss >> retval;

	return retval;
}

void newline()
{
	linenum ++;
	colnum = 1;
	tokenCol = 1;
}

void error(int line, int col, string msg)
{
	errormsg.error(line, col, msg);
}
