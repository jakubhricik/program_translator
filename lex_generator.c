#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "lex_generator.h"
#include "lexer.h"
#include "generator.h"
#include "interpret.h"

void start_generate(int declared_var[])
{
    start_declaration_variables(declared_var);
    do
    {
        process_statement();
    } while (lex_symbol != SEOF);

    write_end();
    generate_output();
}

int generate_bool_expr()
{
    if (lex_symbol == VALUE || lex_symbol == LPAR || lex_symbol == ID ||
		lex_symbol == TRUE_SYMBOL || lex_symbol == FALSE_SYMBOL || lex_symbol == NEGATION)
	    return bool_expr(false);
	return -1;
}

int generate_sequence()
{
    do
    {
        process_statement();
    } while (lex_symbol != RIGHTCBRACKET && lex_symbol != SEOF);
    next_symbol();
	return get_address();
    
}

void generate_expr()
{
    if (lex_symbol == MINUS) expr();
	if (lex_symbol == VALUE) expr();
	else if (lex_symbol == ID) expr();
	else if (lex_symbol == LPAR)
	{
		next_symbol();
		expr();
		match(RPAR);
	}
}

void start_declaration_variables(int declared_var[])
{
	int cvar = count_variables();
	write_begin(cvar);

	if (lex_symbol != VAR)
	{
		return;
	}
	next_symbol();
	do
	{
		if (lex_symbol == ID)
		{
            declared_var[lex_attr] = 1;
			short id = lex_attr;
			next_symbol();
			if (lex_symbol == ASSIGN)
			{
				next_symbol();
				generate_expr();
				write_to_var(id);
			}
			if (lex_symbol == COMMA) next_symbol();
			
		}
	} while (lex_symbol != SEQUENCE && lex_symbol != SEOF);
}

void process_statement()
{
    if(lex_symbol == SEQUENCE) next_symbol();
    if (lex_symbol == IF)
	{
		next_symbol();
		bool expresion = generate_bool_expr();

        if(expresion != 0 && expresion != 1)
        {
            fprintf(stderr, "Error: Ocakava sa pravdivostny vyraz!\n.");
            exit(1);
	    }

		match(THEN);
		match(LEFTCBRACKET);
		short if_addr = write_if();

		generate_sequence();
		if (lex_symbol == ELSE)
		{
			next_symbol();
			match(LEFTCBRACKET);
			short jump_to_end = write_jump(get_address());
			rewrite_value(if_addr, get_address());
			generate_sequence();
			short end_addr = get_address();
			rewrite_value(jump_to_end, end_addr);
		}
		else rewrite_value(if_addr, get_address());
	}
    if (lex_symbol == WHILE)
	{
		next_symbol();
		short conditioAddress = get_address();
		bool expresion = generate_bool_expr();

        if(expresion != 0 && expresion != 1)
        {
            fprintf(stderr, "Error: Ocakava sa pravdivostny vyraz!\n.");
            exit(1);
	    }

		match(DO);
		match(LEFTCBRACKET);

		short whileValueAddr = write_if();

		generate_sequence();

		write_jump(conditioAddress);
		rewrite_value(whileValueAddr, get_address());
	}
    if (lex_symbol == PRINT)
	{
		next_symbol();

		generate_bool_expr();
		write_result();
	}
	if (lex_symbol == READ)
	{
		next_symbol();
		do
		{
			if (lex_symbol == ID)
			{
				write_ask_var(lex_attr, lex_ids[lex_attr]);
				next_symbol();
			}
			if (lex_symbol == COMMA)
			{
				next_symbol();
			}

		} while (lex_symbol != SEQUENCE && lex_symbol != SEOF);
		next_symbol();
	}
    if (lex_symbol == ID)
	{
		int id = lex_attr;
		next_symbol();
		match(ASSIGN);
		generate_expr();
		write_to_var(id);
	}
}