#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "lexer.h"
#include "generator.h"
#include "interpret.h"
#include "lex_generator.h"
#include "lex_interpret.h"


int sart_console_interpret(int variables [], int declared_var [])
{
    declare_variables(variables, declared_var);
    return stm();
}


int declare_variables( int variables [], int declared_var [])
{
    if (lex_symbol != VAR) return -1;
    else next_symbol();

    int id = 0;
    int value = 0;

    if (lex_symbol == ID)
	{
		id = lex_attr;
		next_symbol();
	}
	else match(ID);
	
    if (lex_symbol == ASSIGN)
	{
		next_symbol();
		value = expr();
		variables[id] = value;
	}
	else variables[id] = 0;

    declared_var[id] = 1;
	
    while (lex_symbol == COMMA)
	{
		next_symbol();
		if (lex_symbol == ID)
		{
			next_symbol();
			id = lex_attr;
		}
		else
		{
			match(ID);
		}
		if (lex_symbol == ASSIGN)
		{
			next_symbol();
			value = expr();
			variables[id] = value;
		}
		else variables[id] = 0; 

        declared_var[id] = 1;
	}
	match(SEQUENCE);

	return 1;
}
