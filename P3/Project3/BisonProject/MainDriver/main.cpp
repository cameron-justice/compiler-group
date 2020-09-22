#include <iostream>
#include <fstream>
#include <string>
#include <FlexLexer.h>
#include "ErrorMsg.h"

using namespace std;

extern int yyparse(void);
extern ErrorMsg	errormsg;

yyFlexLexer	lexer;

void parse(string fname) 
{
	ifstream	ifs(fname);
	errormsg.reset(fname);
	lexer.switch_streams(&ifs, NULL);

	if ( yyparse() == 0 ) /* parsing worked */
		cout << "Parsing successful!\n" << endl;
	else
		cout << "Parsing failed\n" << endl;
}

extern int yydebug;
int main(int argc, char **argv) 
{
	yydebug = 0;	//change it to 1 if you want to debug the parser
	if (argc!=2) 
	{
		cerr <<"usage: " << argv[0] << " filename" << endl; 
		return 1;
	}
	parse( argv[1] );
	return 0;
}