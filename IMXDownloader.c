#include <Windows.h>
#include <stdio.h>
#include "FileOperator.h"
#include "IvtDcdTable.h"

#define BIN_OFFSET (3072)

BOOL WriteDataToSdDevice(LPCWSTR sdDeviceName, LPCWSTR binFileName, DWORD memSize) {
    BOOL result = FALSE;

    HANDLE binFile = FileOperator_OpenFile(binFileName);
    if (!binFile) {
        wprintf_s(L"Error: Failed to open %s.\n", binFileName);
        goto Return;
    }

    HANDLE sdFile = FileOperator_OpenFile(sdDeviceName);
    if (!sdFile) {
        wprintf_s(L"Error: Failed to open %s.\n", sdDeviceName);
        goto CleanBinFile;
    }

    DWORD binSize = FileOperator_GetFileSize(binFile);
    if (!binSize) {
        wprintf_s(L"Error: Failed to get size of %s.\n", binFileName);
        goto CleanSDFile;
    }

    DWORD sectorSize = FileOperator_GetSectorSize(sdFile);
    if (!sectorSize) {
        wprintf_s(L"Error: Failed to get sector size of %s.\n", sdDeviceName);
        goto CleanSDFile;
    }

    DWORD bufferSize;
    LPVOID buffer = FileOperator_AllocateSectorBuffer(sectorSize, binSize + BIN_OFFSET, &bufferSize);
    if (!buffer) {
        wprintf_s(L"Error: Failed to allocate memory.\n");
        goto CleanSDFile;
    }

    DWORD tableSize;
    LPCVOID header = IvtDcdTable_GetTable(memSize, &tableSize);
    if (!header) {
        wprintf_s(L"Error: Failed to get header.\n");
        goto CleanSectorBuffer;
    }
    memcpy_s(buffer, tableSize, header, tableSize);

    if (!FileOperator_ReadFile(binFile, (PCHAR) buffer + BIN_OFFSET, binSize)) {
        wprintf_s(L"Error: Failed to read %s.\n", binFileName);
        goto CleanSectorBuffer;
    }

    if (!FileOperator_WriteSector(sdFile, 2, sectorSize, buffer, bufferSize)) {
        wprintf_s(L"Error: Failed to write to %s.\n", sdDeviceName);
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

int wmain(int argc, wchar_t** argv) {
    if ((argc != 3) && (argc != 4)) {
        wprintf_s(L"Error Usage! Reference Below:\n");
        wprintf_s(L"IMXDownload [-512m or -256m] <source_bin> <sd_device>\n");
        return -1;
    }

    LPCWSTR sourceBin = argv[1];
    LPCWSTR sdDevice = argv[2];
    DWORD memSize = 512;
    if (argc == 4) {
        sourceBin = argv[2];
        sdDevice = argv[3];
        if (wcscmp(argv[1], L"-256m") == 0) {
            memSize = 256;
        } else if (wcscmp(argv[1], L"-512m") == 0) {
            memSize = 512;
        } else {
            wprintf_s(L"Error: Invalid memory size.\n");
            return -1;
        }
    }

    if (!WriteDataToSdDevice(sdDevice, sourceBin, memSize)) {
        return -1;
    }

    return 0;
}