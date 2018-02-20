CC=g++
CFLAGS=-Wall -O0 -g -DDEBUG -std=c++14
LDFLAGS=
RM=rm -f
CP=cp -f

BINARY=zb
OBJECTS=\
	obj/Analyzer.o \
	obj/Compiler.o \
	obj/EndStatementNode.o \
	obj/Interpreter.o \
	obj/Lexer.o \
	obj/ModuleNode.o \
	obj/Node.o \
	obj/Parser.o \
	obj/StatementNode.o \
	obj/TextSourceStream.o \
	obj/Token.o \
	obj/Translator.o \
	obj/main.o

all: $(BINARY)

clean:
	$(RM) $(BINARY) $(OBJECTS)

$(BINARY): $(OBJECTS)
	$(CC) -o $(BINARY) $(OBJECTS) $(LDFLAGS)

obj/%.o: src/%.cpp | obj
	$(CC) $(CFLAGS) -o $@ -c $<

obj:
	mkdir -p $@
