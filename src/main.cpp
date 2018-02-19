#ifdef _WIN32
#include <Windows.h>
#else
#include <cstdio>
#endif

#include <sstream>

#include "CompileError.h"
#include "Compiler.h"
#include "TextSourceStream.h"

int main(int argc, char* argv[])
{
    try {
        const char* code = "EN$";

        Compiler compiler;
        TextSourceStream stream(code);

        compiler.run(stream);
    }
    catch (const CompileError& err) {
        std::stringstream msg;
        msg << "[" << err.getRange().getStartRow() << ":" << err.getRange().getStartCol() << "] " << err.what();
#ifdef _WIN32
        (void)MessageBoxA(nullptr, msg.str().c_str(), "Compile Error", MB_OK|MB_ICONERROR);
#else
        fprintf(stderr, "%s\n", msg.str().c_str());
#endif
        return -1;
    }

    return 0;
}
