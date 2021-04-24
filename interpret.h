#ifndef INTERPRET_H
#define INTERPRET_H

enum ASOCIATIVE{
    NO, LEFT, RIGHT
};

#define MAX_INPUT_SIZE 500 // default 160
#define ALLOCATION_RAW_FILE_SIZE 500
#define POW_ASOCIARIVE LEFT // 0 = no , 1 left, 2 right


// Expr -> Mul {("+"|"-") Mul}
int expr();

//Term -> VALUE | "(" Expr ")"
int term();

//Mul -> Power {("*"|"/") Power}
int mul();

/**
* Power -> POW_ASOCIARIVE == LEFT   Power -> Integer [ "^" Power ]
*       -> POW_ASOCIARIVE == RIGHT  Power -> Integer { "^" Integer }
*       -> POW_ASOCIARIVE == NO     Power -> Integer [ "^" Intefer ]
*/
int power();


//Integer ->  [-] Number
int integer();

//Number ->  VALUE | "(" Expr ")"
int number();


//Stm -> "Sequence" | "Read" | "If" | "While" | "Print" | "Id"
int stm();


// Op -> Exp [ ("=" | "<" | ">" | "!=" | "<=" | ">=" ) Exp]
int op( bool);


// BoolExpr -> (Op [ ( "&&" | "||" ) Op])   |   (BoolExpr [ ( "!" | "(" ) Op ])
int bool_expr(bool);


// BoolTerm ->  ("true" | "false") | ("!") BoolExpr | Expr 
int bool_term(bool);


//Sequence ->  ";" Stm
int sequence();

//to get values from user (declared variables)
void read_var(int id_idx);

/* Overenie symbolu na vstupe a precitanie dalsieho.
 * Vracia atribut overeneho symbolu. */
int match(const Symbol expected);


// to remove white spaces from string
void remove_spaces(char* s);

void clear_stdin();

int count_variables();

int bracket_at(int i);

int bracket();

void initialize_declared_variables();

void handle_input(char* , char [], int , char **);

#endif /* INTERPRET_H */