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

int variables[LEX_IDS_MAX];
int declared_var[LEX_IDS_MAX];

/* Gramatika:
* Op -> Exp [ (= | < | > | != | <= | >= ) Exp]
* Expr -> Mul {("+"|"-") Mul}
* Mul -> Power {("*"|"/") Power}
* Power -> POW_ASOCIARIVE == LEFT   Power -> Term [ "^" Power ]
*       -> POW_ASOCIARIVE == RIGHT  Power -> Term { "^" Term }
*       -> POW_ASOCIARIVE == NO     Power -> Term [ "^" Term ]
* Term -> VALUE | "(" Expr ")"
*/


int main(int argc, char **argv)
{
    
    int input_type = 0;
    char filename[100];
    char* source = calloc (ALLOCATION_RAW_FILE_SIZE, sizeof(char *));

    initialize_declared_variables();

    if (argc > 1) input_type = 3;

    handle_input(source, filename, input_type, argv);
    

    remove_spaces(source);

    if(strlen(source) >= MAX_INPUT_SIZE)
    {
        fprintf(stderr, "Error: Subor je prilis velky\n.");
        free(source);
        exit(1);
    }

    FILE *output_file = fopen("program.bin", "wb");
	init_generator(output_file);

	init_lexer(source);
	print_tokens();

	printf("\nZaciatok syntaxou riadenej interpretacie\n");
	printf("----------------------------------------------\n\n");
	init_lexer(source);
	next_symbol();

	LEX_STATE initial_state = get_state();
	start_generate(declared_var);

	set_state(initial_state);

	sart_console_interpret(variables, declared_var);

	match(SEOF);

	fclose(output_file);
	printf("Program vygenerovany v program.bin\n");

	return 0;
}


/*** Syntakticky analyzator a interpretator ***/

int match(const Symbol expected)
{
    if (lex_symbol == expected) {
        int attr = lex_attr;
        next_symbol();
        return attr;
    } else {
        fprintf(stderr,
            "CHYBA: Chyba symbol %s, namiesto toho sa vyskytol %s.\n",
            symbol_name(expected), symbol_name(lex_symbol));
        exit(1);
    }
}

int op(bool negation){
    int leftOp, rightOp;
    Symbol operator;
    leftOp = bool_term(negation);

    if( lex_symbol == EQUAL || lex_symbol == NOT_EQUAL || 
        lex_symbol == GREATER || lex_symbol == LOWER || 
        lex_symbol == GREATER_EQ || lex_symbol == LOWER_EQ)
    {
        
        operator = lex_symbol;
        next_symbol();
        rightOp = bool_term(negation);

        if(negation)
        {
            switch (operator) {
                case EQUAL:
                    write_not_equal();
                    leftOp = leftOp != rightOp;
                    break;
                case NOT_EQUAL:
                    write_equal();
                    leftOp = leftOp == rightOp;
                    break;
                case GREATER:
                    write_lower();
                    leftOp = leftOp < rightOp;
                    break;
                case LOWER:
                    write_greater();
                    leftOp = leftOp > rightOp;
                    break;
                case GREATER_EQ:
                    write_lower_equal();
                    leftOp = leftOp <= rightOp;
                    break;
                case LOWER_EQ:
                    write_greater_equal();
                    leftOp = leftOp >= rightOp;
                    break;
                default: assert("Neocakavany operator v op()");
            }
        }
        else
        {
            switch (operator) {
                case EQUAL:
                    write_equal();
                    leftOp = leftOp == rightOp;
                    break;
                case NOT_EQUAL:
                    write_not_equal();
                    leftOp = leftOp != rightOp;
                    break;
                case GREATER:
                    write_greater();
                    leftOp = leftOp > rightOp;
                    break;
                case LOWER:
                    write_lower();
                    leftOp = leftOp < rightOp;
                    break;
                case GREATER_EQ:
                    write_greater_equal();
                    leftOp = leftOp >= rightOp;
                    break;
                case LOWER_EQ:
                    write_lower_equal();
                    leftOp = leftOp <= rightOp;
                    break;
                default: assert("Neocakavany operator v op()");
            }
        }
    }

    return leftOp;
}

int term()
{
    int value;
    switch (lex_symbol) {
        case COMMA:
            next_symbol();
            value = term();
        case VALUE:
            write_number(lex_attr);
            value = lex_attr;
            next_symbol();
            break;
        case LPAR:
            next_symbol();
            value = expr();
            match(RPAR);
            break;
        case ID:
            if (declared_var[lex_attr] == 0)
		    {
                fprintf(stderr, "Nedeklarovana premenna: %s", lex_ids[lex_attr]);
                exit(1);
            }
            value = variables[lex_attr];
            write_var(lex_attr);
            next_symbol();
            break;
        default:
            fprintf(stderr, "CHYBA: Chyba operand, namiesto toho sa vyskytol %s\n.",
                    symbol_name(lex_symbol));
            exit(1);
    }
    return value;
}

int expr()
{
    int leftOp, rightOp;
    Symbol operator;
    leftOp = mul();
    //leftOp = term();
    


    while (lex_symbol == PLUS || lex_symbol == MINUS)
    {
        operator = lex_symbol;
        next_symbol();
        rightOp = mul();
        //rigntOp = term();

        switch (operator) {
        case PLUS:
            write_add();
            leftOp = leftOp + rightOp;
            break;
        case MINUS:
            write_sub();
            leftOp = leftOp - rightOp;
            break;
        default:
            assert("Neocakavany operator v expr()");
        }
    }
    return leftOp;
}

int power(){
    int leftOp, rightOp;
    Symbol operator;
    leftOp = integer();


    if(POW_ASOCIARIVE == LEFT)
    {
        if(lex_symbol == POWER)
        {
            operator = lex_symbol;
            next_symbol();
            rightOp = power();
            switch (operator) {
            case POWER:
                write_pow();
                leftOp = pow(leftOp, rightOp);
                break;
            default:
                assert("Neocakavany operator v expr()");
            }
        }
    }
    else if(POW_ASOCIARIVE == RIGHT){
        while ( lex_symbol == POWER)
        {
            operator = lex_symbol;
            next_symbol();
            rightOp = term();
            switch (operator) {
            case POWER:
                write_pow();
                leftOp = pow(leftOp, rightOp);
                break;
            default:
                assert("Neocakavany operator v power()");
            }
        }
    }
    else if(POW_ASOCIARIVE == NO){
        if(lex_symbol == POWER){
            operator = lex_symbol;
            next_symbol();
            rightOp = term();
            switch (operator) {
            case POWER:
                write_pow();
                leftOp = pow(leftOp, rightOp);
                break;
            default:
                assert("Neocakavany operator v expr()");
            }
        }
    }

    return leftOp;
}

int mul(){
    int leftOp, rightOp;
    Symbol operator;
    leftOp = power();

    while ( lex_symbol == MUL  || lex_symbol == DIV)
    {
        operator = lex_symbol;
        next_symbol();
        rightOp = power();

        switch (operator) {
            case MUL:
                write_mul();
                leftOp = leftOp * rightOp;
                break;
            case DIV: 
                write_div();
                leftOp = leftOp / rightOp;
                break;
            default:
                assert("Neocakavany operator v mul()");
        }
    }

    return leftOp;
}

int integer()
{
	if (lex_symbol == MINUS)
	{
		next_symbol();
		int n = -number();
		write_inverse();
		return n;
	}
	else return number();
	
}

int number()
{
	int leftOp;
	if (lex_symbol == LPAR)
	{
		next_symbol();
		leftOp = expr();
		match(RPAR);
		return leftOp;
	}
	else if (lex_symbol == VALUE || lex_symbol == ID)
	{
		int cislo = term();
		return cislo;
	}
	return 0;
}

int bool_term(bool negation)
{
    int term;
    if(lex_symbol == NEGATION)
    {
        term = bool_expr(!negation);
        next_symbol();
        return term;
    }
    else if (lex_symbol == TRUE_SYMBOL)
    {
        write_number(1);
        next_symbol();
        return true;
    }
    else if (lex_symbol == FALSE_SYMBOL)
    {
        write_number(0);
        next_symbol();
        return false;
    }
    else return expr();
}

int bool_expr(bool negation)
{
    int leftOp, rightOp;
	Symbol operator;

    if(lex_symbol == NEGATION)
    {
        next_symbol();
        return bool_expr(!negation);
    }

    if (lex_symbol == LPAR)
	{
		next_symbol();
		int term = bool_expr(negation);
		match(RPAR);

		if (is_bool_exp() == 1) leftOp = term;
		else return term;
	}
	else leftOp = op(negation);

    if (lex_symbol == AND || lex_symbol == OR)
	{
		operator = lex_symbol;
		next_symbol();
		rightOp = op(negation);

		switch (operator)
		{
            case AND:
                write_and();
                leftOp = leftOp && rightOp;
                break;
            case OR:
                write_or();
                leftOp = leftOp || rightOp;
                break;
            default: assert("Neocakavany operator v expr()");
		}

        if(leftOp == 1) return true;
        else return false;
	}
	return leftOp;
}

int sequence()
{
	int value = 0;
	if (lex_symbol == SEQUENCE)
	{
		next_symbol();
		value = stm();
	}
	return value;
}

int stm(){
    int value = 0;
	LEX_STATE state;
	int id = lex_attr;

    switch (lex_symbol)
	{
        case SEQUENCE:
            next_symbol();
            value = stm();
        break;

        case IF:
            
            next_symbol();
            bool exp = bool_expr(false);
            match(THEN);
            match(LEFTCBRACKET);

            if (exp)
            {
                value = stm();

                match(RIGHTCBRACKET);
                if (lex_symbol == ELSE)
                {
                    bracket_at(-1);
                    next_symbol();
                }
            }
            else
            {
                bracket();
                next_symbol();

                if (lex_symbol == ELSE)
                {
                    next_symbol();
                    match(LEFTCBRACKET);
                    value = stm();
                    match(RIGHTCBRACKET);
                }
            }
            sequence();
        break;

        case WHILE:
            next_symbol();
            state = get_state();

            bool b = bool_expr(false);
            match(DO);
            match(LEFTCBRACKET);
            while (b)
            {
                value = stm();
                match(RIGHTCBRACKET);
                set_state(state);

                b = bool_expr(false);
                next_symbol();
                next_symbol();
            }
            bracket();
            next_symbol();

            sequence();
		break;

        case ID:
            next_symbol();
            match(ASSIGN);
            if (declared_var[id] == 0)
            {
                fprintf(stderr, "Nedeklarovana premenna: %s", lex_ids[id]);
                exit(1);
            }
            variables[id] = expr();
            sequence();
		break;

        case PRINT:
            next_symbol();
            if (lex_symbol == TRUE_SYMBOL || lex_symbol == FALSE_SYMBOL)
            {
                if(bool_expr(false)){
                    printf("true \n");  
                } 
                else printf("false \n");
            }
            else
            {
                if (!look_at_operand())
                {
                    if(bool_expr(false)) printf("true \n");
                    else printf("false \n");
                }
                else printf("print: %d\n", expr());
            }
            sequence();
		break;

        case READ:
            value = lex_attr;
            int count = 0;
            do
            {
                next_symbol();
                if (declared_var[lex_attr] == 0)
                {
                    fprintf(stderr, "Nedeklarovana premenna: %s", lex_ids[lex_attr]);
                    exit(1);
                }
                read_var(lex_attr);
                next_symbol();
                count++;
            } while (lex_symbol == COMMA);
            write_begin(count);

            for (int i = 0; i < count; i++)
            {
                if (declared_var[i] == 1)
                {
                    write_ask_var(i, lex_ids[i]);
                }
                else
                {
                    fprintf(stderr, "Nedeklarovana premenna: %s", lex_ids[i]);
                    exit(1);
                }
            }

            sequence();

		break;

        default : return value;
    }
    return value;
}

int bracket_at(int i)
{
	int attr;
	while ((i > 0 || lex_symbol != RIGHTCBRACKET) && lex_symbol != SEOF)
	{
		if (lex_symbol == LEFTCBRACKET)
		{
			i++;
		}
		if (lex_symbol == RIGHTCBRACKET)
		{
			i--;
			attr = lex_attr;
		}
		next_symbol();
	}
	return attr;
}

int bracket()
{
    return bracket_at(0);
}

void read_var(int id_idx)
{
    int value;
    printf("%s = ", lex_ids[id_idx]);
    scanf("%d", &value);
    variables[id_idx] = value;
}

int count_variables()
{
    int count_of_variables = 0;
	LEX_STATE state = get_state();
	if (lex_symbol != VAR)
	{
		return 0;
	}
	next_symbol();
	do
	{
		if (lex_symbol == ID)
		{
			count_of_variables++;
			next_symbol();
			if (lex_symbol == ASSIGN)
			{
				do
				{
					next_symbol();
				} while (lex_symbol != COMMA && lex_symbol != SEQUENCE);
			}
			if (lex_symbol == COMMA) next_symbol();
			
		}
	} while (lex_symbol != SEQUENCE && lex_symbol != SEOF);
	set_state(state);
	return count_of_variables;
}

void clear_stdin()
{
	char c;
	do
	{
		c = getchar();
	} while (c != EOF && c != '\n');
}

void initialize_declared_variables()
{
    for (int i = 0; i < LEX_IDS_MAX; i++) declared_var[i] = 0;
}

void remove_spaces(char* s) 
{
    const char* d = s;
    do {
        while (*d == ' ' || *d == '\n' || *d == '\t') {
            ++d;
        }
    } while (*s++ == *d++);
}

void handle_input(char* source, char filename[], int input_type, char **argv)
{
    if(input_type != 3){
        do
        {
            printf("Zadajte rezim vstupu:\n");
            printf("1 - zadanim rucne\n2 - zo suboru: ");
            scanf("%d", &input_type);
            if (!(input_type == 1 || input_type == 2)) printf("Bol zadany zly vstup!\n");

        } while (!(input_type == 1 || input_type == 2) && getchar());
        clear_stdin();
    }
    
    if (input_type == 2) //file input
	{
		printf("Zadaj nazov suboru: ");

		FILE *file;

		fgets(filename, MAX_INPUT_SIZE, stdin);
		strtok(filename, "\n");

		file = fopen(filename, "r");

		if (file == NULL)
		{
			fprintf(stderr, "Error: Nepodarilo sa otvorit subor: %s\n.", filename);
			exit(1);
		}

		fread (source, 1, MAX_INPUT_SIZE, file);
		fclose(file);
	}
	else if (input_type == 1)
	{ //input

		printf("Vstupny retazec: ");
		fgets(source, MAX_INPUT_SIZE, stdin);
	}
	else
	{ //argv

		FILE *file;
		file = fopen(argv[1], "r");

		if (file == NULL)
		{
			fprintf(stderr, "Error: Nepodarilo sa otvorit subor: %s\n.", filename);
			exit(1);
		}
        

		fread (source, 1, MAX_INPUT_SIZE, file);
		fclose(file);
	}

}



