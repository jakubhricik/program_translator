# variables
CC=gcc
CFLAGS=-std=c11 -Wall -Werror
LDLIBS=
OUTPUT=prekladac

# targets
all: $(OUTPUT)

$(OUTPUT): lexer.o generator.o interpret.o lex_generator.o lex_interpret.o
	$(CC) $(CFLAGS) lexer.o generator.o interpret.o lex_generator.o lex_interpret.o $(LDLIBS) -o $(OUTPUT)

lexer.o: lexer.c lexer.h
	$(CC) $(CFLAGS) -c lexer.c

generator.o: generator.c generator.h
	$(CC) $(CFLAGS) -c generator.c

lex_generator.o: lex_generator.c lex_generator.h
	$(CC) $(CFLAGS) -c lex_generator.c

lex_interpret.o: lex_interpret.c lex_interpret.h
	$(CC) $(CFLAGS) -c lex_interpret.c

interpret.o: interpret.c interpret.h
	$(CC) $(CFLAGS) -c interpret.c $(LDLIBS) -o interpret.o
	
	
#remove all generated files
clean:
	rm -rf $(OUTPUT) *.o program.bin