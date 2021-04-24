#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdio.h>
#include <stdlib.h>

void init_generator(FILE *output);
void generate_output();
short get_address();
void write_begin(short num_vars);
void write_end();
void write_result();
void write_number(short value);
void write_var(short index);

void write_add();
void write_sub();
void write_mul();
void write_div();
void write_pow();

void write_greater();
void write_greater_equal();
void write_lower();
void write_lower_equal();
void write_equal();
void write_not_equal();

void write_not();
void write_and();
void write_or();
void write_nop(int count);


short write_if();
short write_jump(short addr);
short chage_value(short address,short value);
void write_inverse();
void write_string(const char *str);

void write_to_var(short index);
void write_ask_var(short index, char *name);

void rewrite_value(short address,short value);

#endif /* GENERATOR_H */
