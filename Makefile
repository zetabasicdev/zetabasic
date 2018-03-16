# BSD 3-Clause License
#
# Copyright (c) 2018, Jason Hoyt
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# * Neither the name of the copyright holder nor the names of its
#   contributors may be used to endorse or promote products derived from
#   this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

CC=g++
CFLAGS=-Wall -O0 -g -DDEBUG -std=c++14 -I./src -I./src/Compiler -I./src/Compiler/Nodes -I./src/Interpreter -I./src/Ide -I$(SDL_INC_PATH)
LDFLAGS=-L$(SDL_LIB_PATH) $(SDL_LIBS)
RM=rm -f
CP=cp -f

BINARY=zb
OBJECTS=\
	obj/Analyzer.o \
	obj/AssignmentStatementNode.o \
	obj/BinaryExpressionNode.o \
	obj/Compiler.o \
	obj/ConstantTable.o \
	obj/EditBuffer.o \
	obj/Editor.o \
	obj/EditView.o \
	obj/EndStatementNode.o \
	obj/ExpressionNode.o \
	obj/ForStatementNode.o \
	obj/FunctionCallExpressionNode.o \
	obj/GotoStatementNode.o \
	obj/Ide.o \
	obj/IdentifierExpressionNode.o \
	obj/IdentifierNode.o \
	obj/IfStatementNode.o \
	obj/InputStatementNode.o \
	obj/Instructions.o \
	obj/IntegerLiteralExpressionNode.o \
	obj/Interpreter.o \
	obj/LabelStatementNode.o \
	obj/Lexer.o \
	obj/ModuleNode.o \
	obj/Node.o \
	obj/Parser.o \
	obj/PrintStatementNode.o \
	obj/Stack.o \
	obj/StatementNode.o \
	obj/StatusBar.o \
	obj/StringLiteralExpressionNode.o \
	obj/StringManager.o \
	obj/StringStack.o \
	obj/StringTable.o \
	obj/SymbolTable.o \
	obj/TextSourceStream.o \
	obj/Token.o \
	obj/Translator.o \
	obj/VirtualMachine.o \
	obj/Window.o \
	obj/main.o

all: $(BINARY)

clean:
	$(RM) $(BINARY) $(OBJECTS)

$(BINARY): $(OBJECTS)
	@$(CC) -o $(BINARY) $(OBJECTS) $(LDFLAGS)
	@echo Linking $(BINARY)

obj/%.o: src/%.cpp | obj
	@$(CC) $(CFLAGS) -o $@ -c $<
	@echo Compiling $(<F)

obj/%.o: src/Compiler/%.cpp | obj
	@$(CC) $(CFLAGS) -o $@ -c $<
	@echo Compiling $(<F)

obj/%.o: src/Compiler/Nodes/%.cpp | obj
	@$(CC) $(CFLAGS) -o $@ -c $<
	@echo Compiling $(<F)

obj/%.o: src/Interpreter/%.cpp | obj
	@$(CC) $(CFLAGS) -o $@ -c $<
	@echo Compiling $(<F)

obj/%.o: src/Ide/%.cpp | obj
	@$(CC) $(CFLAGS) -o $@ -c $<
	@echo Compiling $(<F)

obj:
	mkdir -p $@
