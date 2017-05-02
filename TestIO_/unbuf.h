#ifndef UNBUF_HEADER

#include "tchar.h"
#include "windows.h"

BOOL WINAPI CloseHandle_NB(__in HANDLE hObject);
BOOL WINAPI WriteFile_NB(__in HANDLE hFile, __in LPCVOID lpBuffer, __in DWORD nNumberOfBytesToWrite, __out_opt LPDWORD lpNumberOfBytesWritten, __inout_opt LPOVERLAPPED lpOverlapped);
BOOL WINAPI ReadFile_NB(__in HANDLE hFile, __out LPVOID lpBuffer, __in DWORD nNumberOfBytesToRead, __out_opt LPDWORD lpNumberOfBytesRead, __inout_opt LPOVERLAPPED lpOverlapped);
HANDLE WINAPI CreateFile_NB(__in LPCTSTR lpFileName, __in DWORD dwDesiredAccess, __in DWORD dwShareMode, __in_opt  LPSECURITY_ATTRIBUTES lpSecurityAttributes, __in DWORD dwCreationDisposition, __in DWORD dwFlagsAndAttributes, __in_opt HANDLE hTemplateFile);

#endif
