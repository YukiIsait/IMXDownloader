#include "Console.h"

void Console_Print(LPCWSTR message) {
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    if (console == INVALID_HANDLE_VALUE) {
        return;
    }
    WriteConsoleW(console, message, lstrlenW(message), NULL, NULL);
}

void Console_PrintError(LPCWSTR message) {
    HANDLE console = GetStdHandle(STD_ERROR_HANDLE);
    if (console == INVALID_HANDLE_VALUE) {
        return;
    }
    WriteConsoleW(console, message, lstrlenW(message), NULL, NULL);
}
