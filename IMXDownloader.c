#include <Windows.h>
#include "Console.h"
#include "FileOperator.h"
#include "IMXHeader.h"

BOOL WriteDataToSdDevice(LPCWSTR sdDeviceName, LPCWSTR binFileName, DWORD memSize) {
    BOOL result = FALSE;

    HANDLE binFile = FileOperator_OpenFile(binFileName);
    if (!binFile) {
        Console_PrintError(L"Error: Failed to open bin file.\n");
        goto Return;
    }

    HANDLE sdFile = FileOperator_OpenFile(sdDeviceName);
    if (!sdFile) {
        Console_PrintError(L"Error: Failed to open sd device.\n");
        goto CleanBinFile;
    }

    DWORD binSize = FileOperator_GetFileSize(binFile);
    if (!binSize) {
        Console_PrintError(L"Error: Failed to get size of bin file.\n");
        goto CleanSDFile;
    }

    DWORD sectorSize = FileOperator_GetSectorSize(sdFile);
    if (sectorSize != 512) {
        Console_PrintError(L"Error: Only 512 bytes sector size is supported.\n");
        goto CleanSDFile;
    }

    DWORD bufferSize;
    LPVOID buffer = FileOperator_AllocateSectorBuffer(sectorSize, binSize + 3072, &bufferSize);
    if (!buffer) {
        Console_PrintError(L"Error: Failed to allocate memory.\n");
        goto CleanSDFile;
    }

    DWORD headerSize;
    LPCVOID header = IMXHeader_GetHeader(memSize, &headerSize);
    if (!header) {
        Console_PrintError(L"Error: Failed to get header.\n");
        goto CleanSectorBuffer;
    }
    memcpy_s(buffer, bufferSize, header, headerSize);

    if (!FileOperator_ReadFile(binFile, (PCHAR) buffer + 3072, binSize)) {
        Console_PrintError(L"Error: Failed to read bin file.\n");
        goto CleanSectorBuffer;
    }

    if (!FileOperator_WriteSector(sdFile, 2, sectorSize, buffer, bufferSize)) {
        Console_PrintError(L"Error: Failed to write to sd device.\n");
        goto CleanSectorBuffer;
    }

    result = TRUE;
CleanSectorBuffer:
    FileOperator_FreeBuffer(buffer);
CleanSDFile:
    CloseHandle(sdFile);
CleanBinFile:
    CloseHandle(binFile);
Return:
    return result;
}

int Main(int argc, LPWSTR* argv) {
    if ((argc != 3) && (argc != 4)) {
        Console_PrintError(L"Error Usage! Reference Below:\n");
        Console_PrintError(L"IMXDownload [-512m or -256m] <source_bin> <sd_device>\n");
        return -1;
    }

    LPCWSTR sourceBin = argv[1];
    LPCWSTR sdDevice = argv[2];
    DWORD memSize = 512;
    if (argc == 4) {
        sourceBin = argv[2];
        sdDevice = argv[3];
        if (lstrcmpW(argv[1], L"-256m") == 0) {
            memSize = 256;
        } else if (lstrcmpW(argv[1], L"-512m") == 0) {
            memSize = 512;
        } else {
            Console_PrintError(L"Error: Invalid memory size.\n");
            return -1;
        }
    }

    if (!WriteDataToSdDevice(sdDevice, sourceBin, memSize)) {
        return -1;
    }

    return 0;
}
