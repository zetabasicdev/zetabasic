#include "CompileError.h"
#include "Compiler.h"
#include "TextSourceStream.h"

int main(int argc, char* argv[])
{
    const char* code = "END";

    Compiler compiler;
    TextSourceStream stream(code);

    compiler.run(stream);

    return 0;
}
