#include <Windows.h>
#include "Console.h"
#include "FileOperator.h"
#include "IMXHeader.h"

BOOL WriteDataToSdDevice(LPCWSTR sdDeviceName, LPCWSTR binFileName, DWORD memSize) {
    BOOL result = FALSE;

    HANDLE binFile = FileOperator_OpenFile(binFileName);
    if (!binFile) {
        Console_PrintError(L"Error: Failed to open file.\n");
        goto Return;
    }

    HANDLE sdFile = FileOperator_OpenFile(sdDeviceName);
    if (!sdFile) {
        Console_PrintError(L"Error: Failed to open device.\n");
        goto CleanBinFile;
    }

    DWORD binSize = FileOperator_GetFileSize(binFile);
    if (!binSize) {
        Console_PrintError(L"Error: Failed to get file size.\n");
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
    CopyMemory(buffer, header, headerSize);

    if (!FileOperator_ReadFile(binFile, (PCHAR) buffer + 3072, binSize)) {
        Console_PrintError(L"Error: Failed to read file.\n");
        goto CleanSectorBuffer;
    }

    if (!FileOperator_WriteSector(sdFile, 2, sectorSize, buffer, bufferSize)) {
        Console_PrintError(L"Error: Failed to write sector.\n");
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

int Main(int argumentCount, LPWSTR* argumentVector) {
    if ((argumentCount != 3) && (argumentCount != 4)) {
        Console_PrintError(L"Error Usage! Reference Below:\n");
        Console_PrintError(L"IMXDownload [-512m or -256m] <source_bin> <sd_device>\n");
        return -1;
    }

    LPCWSTR sourceBin = argumentVector[1];
    LPCWSTR sdDevice = argumentVector[2];
    DWORD memSize = 512;
    if (argumentCount == 4) {
        sourceBin = argumentVector[2];
        sdDevice = argumentVector[3];
        if (lstrcmpW(argumentVector[1], L"-256m") == 0) {
            memSize = 256;
        } else if (lstrcmpW(argumentVector[1], L"-512m") == 0) {
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
