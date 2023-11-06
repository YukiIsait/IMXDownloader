#pragma once
#include <Windows.h>

HANDLE FileOperator_OpenFile(LPCWSTR fileName);
DWORD FileOperator_GetFileSize(HANDLE file);
BOOL FileOperator_ReadFile(HANDLE file, LPVOID buffer, DWORD bufferSize);
DWORD FileOperator_GetSectorSize(HANDLE device);
LPVOID FileOperator_AllocateSectorBuffer(DWORD sectorSize, DWORD bufferSize, LPDWORD roundedBufferSize);
BOOL FileOperator_FreeBuffer(LPVOID buffer);
BOOL FileOperator_WriteSector(HANDLE device, DWORD sector, DWORD sectorSize, LPCVOID buffer, DWORD bufferSize);
