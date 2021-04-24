#include <string.h>

#include "generator.h"

// Position of additional working memory, something like an additional register you can use.
#define WORK_MEM (short)2
// The start of the variables area. It is 3, since you already have JMP, its address and WORK_MEM in memory.
#define VAR_OFFSET (short)3
// Maximum size of your code,
#define MAX_CODE_LENGTH 20000
// Starting position of our stack.
#define STACK_START 20000

// All of our instructions.
enum OP_Code {
    NOP, BZE, JMP, JSR, RTS, EXIT,
    INPC, INP, INPR, OUTC, OUT, OUTR,
    POP, POPR, PUSH, PUSHR,
    LDA, LDAM, LDAI, LDAX,
    LDR, LDRI, STA,
    STAI, STRI,
    LDX, STX, LDS, STS,
    OR, AND, NOT,
    EQ, NE, LT, LE, GT, GE,
    EQR, NER, LTR, LER, GTR, GER,
    ADD, ADDM, SUB, SUBM,
    MUL, DIV, NEG,
    ADDR, SUBR, MULR, DIVR, NEGR
};


// File where our binary computron code will reside.
FILE *output_stream;
// An array of shorts, where we are adding our code.
// This will be copied to the output_stream at the end of the code generation.
short *code_list;
// The current end position of our code
short address;

short get_address(){
    return address;
}

void set_address(const short addr){
    address = addr;
}

void put_word(short value){
    code_list[address] = value;
    address++;
}

void put_op_attr(short op, short value){
    put_word(op);
    put_word(value);
}


/* Code for real numbers only.
 * Just ignore this, if you are not working with them.
 *
 */

typedef union {
    float R;

    struct {
        unsigned short RL;
        unsigned short RH;
    } Dword;
} RealRegister;

void put_real(float arg) {
    RealRegister rvalue;
    rvalue.R = arg;
    put_word(rvalue.Dword.RL);
    put_word(rvalue.Dword.RH);
}
//Code for real numbers ends here.

// Call this only at the start.
void init_generator(FILE *output) {
    address = 0;
    // We need to initialize our code to NOP instruction, that is why we call calloc().
    code_list = calloc(sizeof(short), MAX_CODE_LENGTH);
    output_stream = output;
}

// Call this function at the end of process
void generate_output(){
    fwrite(code_list, sizeof(short), (size_t) address, output_stream);
}

void write_begin(short num_vars) {
    // Jump over stored variables
    put_op_attr(JMP,VAR_OFFSET + num_vars);
    // Save stack start at your WORK_MEM "register".
    put_word(STACK_START);
    // The area, where your variables are stored
    for (int i = 0; i < num_vars; i++) {
        put_word(NOP);
    }
    // Set the stack pointer.
    put_op_attr(LDS,2);
}

void write_end(){
    put_word(EXIT);
}

void write_result() {
    put_word(POP);
    put_word(OUT);
}

void write_number(short value) {
    put_op_attr(LDAM, value);
    put_word(PUSH);
}

void write_string(const char *str){
    size_t len = strlen( str );
    for (int i = 0; i < len; i++){
        put_op_attr(LDAM, str[i]);
        put_word(OUTC);
    }
}

void write_var(short index) {
    put_op_attr(LDA, VAR_OFFSET + index);
    put_word(PUSH);
}

void write_ask_var(short index, char *name) {

    write_string(name);
    write_string(" = ");

    put_word(INP);
    put_op_attr(STA, VAR_OFFSET + index );
}

void write_nop(int count){
    for (int i = 0; i < count; i++){
        put_word(NOP);      
    }
}

void write_add() {
    put_word(POP);
    put_op_attr(STA, WORK_MEM);
    put_word(POP);
    put_op_attr(ADD, WORK_MEM);
    put_word(PUSH);
}

void write_sub() {
    put_word(POP);
    put_op_attr(STA, WORK_MEM);
    put_word(POP);
    put_op_attr(SUB, WORK_MEM);
    put_word(PUSH);
}

void write_mul() {
    put_word(POP);
    put_op_attr(STA, WORK_MEM);
    put_word(POP);
    put_op_attr(MUL, WORK_MEM);
    put_word(PUSH);
}

void write_div() {
    put_word(POP);
    put_op_attr(STA, WORK_MEM);
    put_word(POP);
    put_op_attr(DIV, WORK_MEM);
    put_word(PUSH);
}

void write_pow(){
    short value_addr = get_address() + 2;
	put_op_attr(JMP, value_addr + 3);
	write_nop(3);
	put_word(POP);
	put_op_attr(STA, value_addr + 1); 
	put_word(POP);
	put_op_attr(STA, value_addr); 
	put_op_attr(LDAM, 1);
	put_op_attr(STA, value_addr + 2); 
	
	short while_addr = get_address();
	put_op_attr(LDAM, 1);
	put_op_attr(STA, WORK_MEM);
	put_op_attr(LDA, value_addr + 1);
	put_op_attr(GE, WORK_MEM);
	put_op_attr(BZE, get_address()); 
	short rewrite_addr = get_address() - 1;
	
	put_op_attr(LDA,value_addr);
	put_op_attr(MUL, value_addr + 2);
	put_op_attr(STA, value_addr + 2);
	
	put_op_attr(LDAM, 1);
	put_op_attr(STA, WORK_MEM);
	put_op_attr(LDA, value_addr + 1);
	put_op_attr(SUB, WORK_MEM);
	put_op_attr(STA, value_addr + 1);
	
	put_op_attr(JMP, while_addr);
	rewrite_value(rewrite_addr, get_address());
	
	put_op_attr(LDA, value_addr + 2);
	put_word(PUSH);
}

void write_to_var(const short index){
    put_word(POP);
    put_op_attr(STA, VAR_OFFSET + index);
}

void write_not(){
    put_word(POP);
    put_word(NOT);
    put_word(PUSH);
}

void write_inverse(){
    put_word(POP);
	put_op_attr(STA, WORK_MEM);
	put_op_attr(LDAM,0);
	put_word(PUSH);
	put_op_attr(LDA, WORK_MEM);
	put_word(PUSH);
	write_sub();
}

void write_lower() {
    put_word(POP);
    put_op_attr(STA, WORK_MEM);
    put_word(POP);
    put_op_attr(LT, WORK_MEM);
    put_word(PUSH);
}

void write_lower_equal(){
    put_word(POP);
    put_op_attr(STA, WORK_MEM);
    put_word(POP);
    put_op_attr(LE, WORK_MEM);
    put_word(PUSH);
}

void write_greater() {
    put_word(POP);
    put_op_attr(STA, WORK_MEM);
    put_word(POP);
    put_op_attr(GT, WORK_MEM);
    put_word(PUSH);
}

void write_greater_equal(){
    put_word(POP);
    put_op_attr(STA, WORK_MEM);
    put_word(POP);
    put_op_attr(GE, WORK_MEM);
    put_word(PUSH);
}

void write_equal() {
    put_word(POP);
    put_op_attr(STA, WORK_MEM);
    put_word(POP);
    put_op_attr(EQ, WORK_MEM);
    put_word(PUSH);
}

void write_not_equal() {
    put_word(POP);
    put_op_attr(STA, WORK_MEM);
    put_word(POP);
    put_op_attr(NE, WORK_MEM);
    put_word(PUSH);
}

void write_and() {
    put_word(POP);
    put_op_attr(STA, WORK_MEM);
    put_word(POP);
    put_op_attr(AND, WORK_MEM);
    put_word(PUSH);
}

void write_or() {
    put_word(POP);
    put_op_attr(STA, WORK_MEM);
    put_word(POP);
    put_op_attr(OR, WORK_MEM);
    put_word(PUSH);
}

short write_if(){
	put_word(POP);
	put_op_attr(STA, WORK_MEM);
	put_op_attr(BZE, get_address());
	return get_address() - 1;
}

short write_jump(const short addr){
	put_word(POP);
	put_op_attr(STA, WORK_MEM);
	put_op_attr(JMP,addr);
	return get_address() - 1;
}

void rewrite_value(const short address, const short value){
	code_list[address] = value;
}

