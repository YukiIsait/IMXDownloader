#include <Windows.h>
#include "IMXDownloader.h"

int WINAPI wWinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE prevInstance, _In_ PWSTR cmdLine, _In_ int cmdShow) {
    UNREFERENCED_PARAMETER(prevInstance);
    UNREFERENCED_PARAMETER(prevInstance);
    UNREFERENCED_PARAMETER(cmdShow);
    if (!*cmdLine) {
        return Main(0, NULL);
    }
    int argc;
    LPWSTR* argv = CommandLineToArgvW(cmdLine, &argc);
    if (!argv) {
        return Main(0, NULL);
    }
    int result = Main(argc, argv);
    LocalFree(argv);
    return result;
}
