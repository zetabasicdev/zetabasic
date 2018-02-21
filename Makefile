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
