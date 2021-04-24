#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "lexer.h"

/* Nazvy symbolov (len pre ich jednoduchsi vypis) */
const char *SYM_NAMES[] = {
    [VALUE]="VALUE", [ID]="ID", [READ]="READ", [PRINT]="PRINT",
    [PLUS]="PLUS", [MINUS]="MINUS", [MUL]="MUL", [DIV]="DIV", [POWER]="POWER",
    [LPAR]="LPAR", [RPAR]="RPAR", [COMMA]="COMMA",
    [SEOF]="SEOF", [SERROR]="SERROR", 
    [TRUE_SYMBOL] = "TRUE SYMBOL", [FALSE_SYMBOL] = "FALSE SYMBOL", 
    [OR] = "OR", [AND] = "AND",
    [EQUAL] = "EQUAL", [NOT_EQUAL]= "NOT_EQUAL", 
    [GREATER] = "GREATER", [GREATER_EQ] = "GREATER_EQUAL",
    [LOWER] = "LOWER", [LOWER_EQ] = "LOWER_EQUAL",
    [SEQUENCE] = "SEQ", [IF] = "IF", [THEN] = "THEN", [ELSE] = "ELSE",
    [WHILE] = "WHILE", [DO] = "DO",
    [LEFTCBRACKET] = "LEFTCBRACKET", [RIGHTCBRACKET] = "RIGHTCBRACKET",
    [NEGATION] = "NEGATION",
	[VAR] = "VAR", [ASSIGN] = "ASSIGN"
}; 

/* Globalne premenne, "public" */
Symbol lex_symbol;
int lex_attr;

char *lex_ids[LEX_IDS_MAX];
int lex_ids_size; // Pocet ulozenych identifikatorov


/* Vstupne premenne */
static char *input;     // Vstupny retazec
static char c;          // Spracovavany vstupny znak
static int ic;          // Index dalsieho znaku vo vstupnom retazci


/* Inicializacia lex. analyzatora. Parametrom je vstupny retazec. */
void init_lexer(char *string)
{
    input = string;
    ic = 0;
    lex_ids_size = 0;
}


/* Ulozenie identifikatora `id` do tabulky identifikatorov ak tam este nie je.
 * Vracia index, na ktorom je identifikator ulozeny. */
int store_id(char *id) {
    int i = 0;
    while (i < lex_ids_size) {
        if (strcmp(id, lex_ids[i]) == 0)
            return i;
        i++;
    }
    lex_ids[i] = strdup(id);
    lex_ids_size++;
    return i;
}



bool is_bool_exp()
{
    // switch (lex_symbol)
    // {
    //     case GREATER: return true;
    //     case GREATER_EQ: return true;
    //     case LOWER: return true;
    //     case LOWER_EQ: return true;
    //     case EQUAL: return true;
    //     case NOT_EQUAL: return true;
    //     case AND: return true;
    //     case OR: return true;
    //     default: return false;
    // }

    if (lex_symbol == GREATER || lex_symbol == GREATER_EQ || lex_symbol == LOWER ||
    lex_symbol == LOWER_EQ || lex_symbol == EQUAL ||
    lex_symbol == NOT_EQUAL || lex_symbol == AND || lex_symbol == OR)
	{
		return 1;
	}
	return 0;
}

bool is_arithmetic_exp()
{
    // switch (lex_symbol)
    // {
    //     case PLUS: return true;
    //     case MINUS: return true;
    //     case MUL: return true;
    //     case DIV: return true;
    //     case EQUAL: return true;
    //     case POWER: return true;
    //     default: return false;
    // }

    if (lex_symbol == PLUS || lex_symbol == MINUS || lex_symbol == MUL ||
    lex_symbol == DIV || lex_symbol == POWER)
	{
		return true;
	}
	return false;
}

bool is_exp()
{
    return (is_bool_exp() || is_arithmetic_exp());
}




bool look_at_operand()
{
    LEX_STATE state = get_state();
    do
    {
        next_symbol();
		if (lex_symbol == VALUE || lex_symbol == ID) 
            continue;
		
		if (lex_symbol == SEOF || lex_symbol == SEQUENCE || lex_symbol == RIGHTCBRACKET) 
            break;

		continue;
    } while (!is_exp());
    
    bool aep = is_arithmetic_exp();
	bool bep = is_bool_exp();
	set_state(state);
	if (aep == 0 && bep == 0 ) return true;

	return aep;
}

LEX_STATE get_state()
{
    LEX_STATE state;
	state.ic = ic;
	state.symbol = lex_symbol;
	state.attr = lex_attr;
	state.lex_ids_size = lex_ids_size;
	return state;
}

void set_state(LEX_STATE state)
{
	ic = state.ic;
	lex_symbol = state.symbol;
	lex_attr = state.attr;
	lex_ids_size = state.lex_ids_size;
}



/* Precitanie dalsieho symbolu.
 * Volanie nastavi nove hodnoty lex_symbol a lex_attr. */
void next_symbol()
{
    c = input[ic];
    ic++;
    while (isspace(c)) { // Preskocenie medzier
        c = input[ic];
        ic++;
    }
    switch (c) {
        case ',': lex_symbol = COMMA; break;
        case '+': lex_symbol = PLUS;  break;
        case '-': lex_symbol = MINUS; break;
        case '*': lex_symbol = MUL;   break;
        case '/': lex_symbol = DIV;   break;
        case '^': lex_symbol = POWER; break;
        case '(': lex_symbol = LPAR;  break;
        case ')': lex_symbol = RPAR;  break;
        case '{': lex_symbol = LEFTCBRACKET;  break;
        case '}': lex_symbol = RIGHTCBRACKET; break;
        case ';': lex_symbol = SEQUENCE; break;
        case ':':
            if (input[ic] == '=')
            {
                lex_symbol = ASSIGN;
                ic++;
            }
            else lex_symbol = SERROR;
            
            break;
        case '=': lex_symbol = EQUAL; break;
        case '!': 
            if (input[ic] == '=')
            {
                lex_symbol = NOT_EQUAL;
                ic++;
            }
            else lex_symbol = NEGATION;
            break;
        case '|':
            if (input[ic] == '|')
            {
                lex_symbol = OR;
                ic++;
            }
            else lex_symbol = SERROR;
            break;
        case '&':
            if (input[ic] == '&')
            {
                lex_symbol = AND;
                ic++;
            }
            else lex_symbol = SERROR;
            break;
        case '<':
            if (input[ic] == '=')
            {
                lex_symbol = LOWER_EQ;
                ic++;
            }
            else
                lex_symbol = LOWER;
            break;
        case '>':
            if (input[ic] == '=')
            {
                lex_symbol = GREATER_EQ;
                ic++;
            }
            else lex_symbol = GREATER;
            break;
        case '\0': lex_symbol = SEOF; break; // Koniec retazce

    default:
        if (isdigit(c)) 
        {

            int id_start = ic - 1; // Index zaciatku identifikatora
			do
			{
				c = input[ic];
				ic++;
			} while (isdigit(c));
			ic--; // "Vratenie" posledneho znaku

			int id_len = ic - id_start;
			char *id = malloc(id_len + 1);
			memcpy(id, &input[id_start], id_len);
			id[id_len] = 0;

			int number = atoi(id);

			lex_attr = number;
			lex_symbol = VALUE;

			free(id);

        } 
        else if (isalpha(c)) {
            int id_start = ic - 1; // Index zaciatku identifikatora
            do {
                c = input[ic];
                ic++;
            } while (isalnum(c));
            ic--; // "Vratenie" posledneho znaku

            // Skopirovanie identifikatora
            // char *id = strndup(&input[id_start], ic - id_start);
            int id_len = ic - id_start;
            char *id = malloc(id_len + 1);
            memcpy(id, &input[id_start], id_len);
            id[id_len] = 0;

            // Kontrola klucovych slov
			if (strcmp(id, "read") == 0) lex_symbol = READ;
			else if (strcmp(id, "print") == 0) lex_symbol = PRINT;
			else if (strcmp(id, "true") == 0) lex_symbol = TRUE_SYMBOL;
			else if (strcmp(id, "false") == 0) lex_symbol = FALSE_SYMBOL;
			else if (strcmp(id, "var") == 0) lex_symbol = VAR;
			else if (strcmp(id, "if") == 0) lex_symbol = IF;
			else if (strcmp(id, "then") == 0)  lex_symbol = THEN;
			else if (strcmp(id, "else") == 0) lex_symbol = ELSE;
			else if (strcmp(id, "while") == 0) lex_symbol = WHILE;
			else if (strcmp(id, "do") == 0) lex_symbol = DO;
			
			else
			{ // Ulozenie do tabulky identifikatorov
				lex_attr = store_id(id);
				lex_symbol = ID;
			}
			free(id);
        } else {
            lex_symbol = SERROR;
        }
    }
}


/* Nazov lexikalnej jednotky */
const char *symbol_name(Symbol symbol)
{
    return SYM_NAMES[symbol];
}


/* Vypis vsetky lexikalnych jednotiek zo vstupu */
void print_tokens()
{
    printf("\nVystup lexikalnej analyzy (retazec symbolov)\n");
    do {
        next_symbol();
        printf("  [%2d] %s", lex_symbol, symbol_name(lex_symbol));
        if (lex_symbol == VALUE) printf(" <%d>", lex_attr);
        if (lex_symbol == ID) printf(" <%d> -> %s", lex_attr, lex_ids[lex_attr]);
        printf("\n");
    } while (lex_symbol != SEOF);
}
