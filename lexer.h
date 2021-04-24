#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>

/* Velkost tabulky identifikatorov */
#define LEX_IDS_MAX 20

/* Typy symbolov - lexikalnych jednotiek */
typedef enum 
{
    VALUE, ID, READ, PRINT, PLUS, MINUS, MUL, DIV, POWER, LPAR, RPAR, COMMA,
    SEOF, SERROR, 
    EQUAL, NOT_EQUAL, GREATER, GREATER_EQ, LOWER, LOWER_EQ,
    AND, OR, IF, THEN, ELSE,
    SEQUENCE, WHILE, DO,
    LEFTCBRACKET, RIGHTCBRACKET, TRUE_SYMBOL, FALSE_SYMBOL, NEGATION, VAR, ASSIGN
} Symbol;


typedef struct LEX_STATES
{
	int ic;
	Symbol symbol;
    int attr;
    int lex_ids_size;
} LEX_STATE;



/* Vystupny symbol lexikalnej analyzy a jeho atribut.
 * Ak symbol je VALUE, atribut obsahuje jeho celociselnu hodnotu,
 * ak je to ID, atribut obsahuje index do tabulky identifikatorov. */
extern Symbol lex_symbol;
extern int lex_attr;

/* Tabulka identifikatorov */
extern char *lex_ids[LEX_IDS_MAX];
extern int lex_ids_size; // Pocet ulozenych identifikatorov

/* Inicializacia lex. analyzatora. Parametrom je vstupny retazec. */
void init_lexer(char *string);

/* Precitanie dalsieho symbolu.
 * Volanie nastavi nove hodnoty lex_symbol a lex_attr. */
void next_symbol();

/* Vypis vsetky lexikalnych jednotiek zo vstupu */
void print_tokens();

/* Nazov lexikalnej jednotky */
const char *symbol_name(Symbol symbol);


bool look_at_operand();

bool is_bool_exp();

bool is_arithmetic_exp();

bool is_exp();

LEX_STATE get_state();

void set_state(LEX_STATE state);

#endif /* LEXER_H */

