#include "Analyzer.h"
#include "Compiler.h"
#include "Lexer.h"
#include "Parser.h"
#include "Translator.h"

Compiler::Compiler()
    :
    mTokenPool(256),
    mTokens(),
    mStringPool(),
    mNodePool(),
    mBytecode(256)
{
    // intentionally left blank
}

Compiler::~Compiler()
{
    // intentionally left blank
}

void Compiler::run(ISourceStream& source)
{
    mTokenPool.reset();
    mTokens.reset();
    mStringPool.reset();
    mNodePool.reset();
    mBytecode.reset();

    Lexer lexer(mTokenPool, mTokens, mStringPool, source);
    lexer.run();

    Parser parser(mNodePool, mStringPool, mTokens);
    Node& root = parser.run();

    Analyzer analyzer(mNodePool, root);
    analyzer.run();

    Translator translator(mBytecode, root);
    translator.run();
}
