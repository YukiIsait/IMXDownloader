#include <Windows.h>
#include "IMXDownloader.h"

int Main(int argumentCount, LPWSTR* argumentVector);

DWORD EntryPoint() {
    LPWSTR commandLine = GetCommandLineW();
    if (!*commandLine) {
        return Main(0, &commandLine);
    }
    int argumentCount;
    LPWSTR* argumentVector = CommandLineToArgvW(commandLine, &argumentCount);
    if (!argumentCount) {
        return Main(0, &commandLine);
    }
    int exitCode = Main(argumentCount, argumentVector);
    LocalFree(argumentVector);
    return exitCode;
}
