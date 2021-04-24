#ifndef LEX_GENERATOR_H
#define LEX_GENERATOR_H


void start_generate(int array []);

void process_statement();

int generate_bool_expr();

int generate_sequence();

void start_declaration_variables(int array []);

void generate_expr();

#endif /* LEX_GENERATOR_H */