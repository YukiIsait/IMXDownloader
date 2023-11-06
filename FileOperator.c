#include "FileOperator.h"

#define ROUND_UP_SIZE(Value, Pow2) ((SIZE_T) ((((ULONG) (Value)) + (Pow2) -1) & (~(((LONG) (Pow2)) - 1))))

HANDLE FileOperator_OpenFile(LPCWSTR fileName) {
    HANDLE file = CreateFileW(fileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                              NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) {
        return NULL;
    }
    return file;
}

DWORD FileOperator_GetFileSize(HANDLE file) {
    DWORD fileSize = GetFileSize(file, NULL);
    if (fileSize == INVALID_FILE_SIZE) {
        return 0;
    }
    return fileSize;
}

BOOL FileOperator_ReadFile(HANDLE file, LPVOID buffer, DWORD bufferSize) {
    DWORD numberOfBytesRead;
    if (!ReadFile(file, buffer, bufferSize, &numberOfBytesRead, NULL)) {
        return FALSE;
    }
    return TRUE;
}

DWORD FileOperator_GetSectorSize(HANDLE device) {
    DWORD bytesReturned;
    DISK_GEOMETRY diskGeometry = { 0 };
    if (!DeviceIoControl(device, IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, 0, &diskGeometry,
                         sizeof(DISK_GEOMETRY), &bytesReturned, NULL)) {
        return 0;
    }
    return diskGeometry.BytesPerSector;
}

LPVOID FileOperator_AllocateSectorBuffer(DWORD sectorSize, DWORD bufferSize, LPDWORD roundedBufferSize) {
    HANDLE processHeap = GetProcessHeap();
    if (!processHeap) {
        return FALSE;
    }
    SIZE_T size = ROUND_UP_SIZE(bufferSize, sectorSize);
    LPVOID buffer = HeapAlloc(processHeap, HEAP_ZERO_MEMORY, size);
    if (!buffer) {
        return FALSE;
    }
    *roundedBufferSize = (DWORD) size;
    return buffer;
}

BOOL FileOperator_FreeBuffer(LPVOID buffer) {
    HANDLE processHeap = GetProcessHeap();
    if (!processHeap) {
        return FALSE;
    }
    if (!HeapFree(processHeap, 0, buffer)) {
        return FALSE;
    }
    return TRUE;
}

BOOL FileOperator_WriteSector(HANDLE device, DWORD sector, DWORD sectorSize, LPCVOID buffer, DWORD bufferSize) {
    OVERLAPPED overlapped = {
        .Offset = sector * sectorSize,
    };
    if (!WriteFile(device, buffer, bufferSize, NULL, &overlapped)) {
        return FALSE;
    }
    return TRUE;
}
