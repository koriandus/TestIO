/*
Public domain by Lasse Reinhold, 2010
No copyright pending

This library wraps FILE_FLAG_NO_BUFFERING file I/O into WriteFile_NB() and ReadFile_NB()
functions that are simple to use - they do not need sector sized transfer size or memory 
pointers.

The state structs contain a first-in-first-out queue of size QUEUE_SIZE which is the disk 
transfer I/O size. The states also contain the values 'first' and 'count'. The 'count'
value determins the current payload in in bytes and 'first' points to the first byte.

            ____ count ____
           /               \  
----------------------------------------- 
          ^                             ^
        first                       QUEUE_SIZE

When a file is opend for writing, first is always 0. When it's opend for reading, it
changes.

*/
#include "unbuf.h"

#define false 0
#define true 1

// must be a multipla of 64 K
//const DWORD QUEUE_SIZE = 256*1024;
//const DWORD QUEUE_SIZE = 1*1024*1024;
const DWORD QUEUE_SIZE = 4*1024*1024;

typedef struct 
{
	DWORD count;
	DWORD first;
	unsigned char *queue;
	HANDLE file_handle;
	DWORD dwDesiredAccess;
	_TCHAR file_name[_MAX_PATH];
} state;

state *state_create(DWORD size)
{
	state *s = (state *)malloc(sizeof(state));

	// Allocate page aligned (and thus sector aligned) file queue
	s->queue = (unsigned char *)VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE);
	if (s->queue == NULL)
		return NULL;

	// Avoid lock and release at each disk transfer
	VirtualLock(s->queue, size);
	s->count = 0;
	return s;
}

static DWORD minimum(DWORD a, DWORD b)
{
	return (a > b ? b : a);
}

BOOL WINAPI CloseHandle_NB(__in HANDLE hObject)
{
	state *s = (state *)hObject;
	HANDLE ofile;
	const DWORD MAX_SECTOR_SIZE = 4*1024;

	if(s->dwDesiredAccess == GENERIC_WRITE)
	{
		LARGE_INTEGER li;
		DWORD written = 1;

		// Round up pending data to nearest MAX_SECTOR_SIZE and flush
		DWORD n = (s->count | (MAX_SECTOR_SIZE - 1)) + 1;
		__int64 excess = n - s->count;
		BOOL b = WriteFile(s->file_handle, s->queue, n, &written, 0);
		VirtualFree(s->queue, NULL, MEM_RELEASE);
		if(!CloseHandle(s->file_handle))
            return false;

		// Reopen file, seek to intended size and truncate
        ofile = CreateFile(s->file_name, GENERIC_WRITE, FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(ofile == INVALID_HANDLE_VALUE)
            return false;

		li.QuadPart = -excess;
		SetFilePointer(ofile, li.LowPart, &li.HighPart, FILE_END);
        b = SetEndOfFile(ofile);
		
		if(b == false)
			return false;

		return CloseHandle(ofile);	
	}
	else
	{
		VirtualFree(s->queue, NULL, MEM_RELEASE);
		return CloseHandle(s->file_handle);	
	}
}

BOOL WINAPI WriteFile_NB(__in HANDLE hFile, __in LPCVOID lpBuffer, __in DWORD nNumberOfBytesToWrite, __out_opt LPDWORD lpNumberOfBytesWritten, __inout_opt LPOVERLAPPED lpOverlapped)
{
	state *s = (state *)hFile;	
	char *src = (char *)lpBuffer;
	DWORD written = 1;
	
	*lpNumberOfBytesWritten = 0;

	while(nNumberOfBytesToWrite > 0)
	{
		// Put as much data in queue as fits
		DWORD m = minimum(nNumberOfBytesToWrite, QUEUE_SIZE - s->count);	
		memcpy(s->queue + s->count, src, m);
		s->count += m;
		src += m;
		nNumberOfBytesToWrite -= m;
		*lpNumberOfBytesWritten += m;

		// If queue full, flush it to disk
		if(s->count == QUEUE_SIZE)
		{
			BOOL b = WriteFile(s->file_handle, s->queue, QUEUE_SIZE, &written, 0);
			s->count = 0;
		}
	}

	return true;
}

BOOL WINAPI ReadFile_NB(__in HANDLE hFile, __out LPVOID lpBuffer, __in DWORD nNumberOfBytesToRead, __out_opt LPDWORD lpNumberOfBytesRead, __inout_opt LPOVERLAPPED lpOverlapped)
{
	state *s = (state *)hFile;	
	char *dst = (char *)lpBuffer;
	*lpNumberOfBytesRead = 0;

	for(;;)
	{
		DWORD read = 1;
		DWORD m;

		// If queue empty, fill it from disk
		if(s->count == 0)
		{
			BOOL r = ReadFile(s->file_handle, s->queue, QUEUE_SIZE, &read, 0);
			if(r == false)
				return false;

			s->count += read;
			s->first = 0;
		}

		// Take out as much data from queue as possible
		m = minimum(nNumberOfBytesToRead, s->count);
		memcpy(dst, s->queue + s->first, m);
		s->first += m;
		s->count -= m;
		dst += m;
		*lpNumberOfBytesRead += m;
		nNumberOfBytesToRead -= m;

		if(nNumberOfBytesToRead == 0 || read == 0)
			return true;
	}	
	return true;
}

HANDLE WINAPI CreateFile_NB(__in LPCTSTR lpFileName, __in DWORD dwDesiredAccess, __in DWORD dwShareMode, __in_opt  LPSECURITY_ATTRIBUTES lpSecurityAttributes, __in DWORD dwCreationDisposition, __in DWORD dwFlagsAndAttributes, __in_opt HANDLE hTemplateFile)
{	
	state *s;
	HANDLE h = CreateFile(lpFileName, dwDesiredAccess, dwShareMode, 0, dwCreationDisposition, FILE_FLAG_NO_BUFFERING | FILE_FLAG_SEQUENTIAL_SCAN | dwFlagsAndAttributes, NULL);

	// Wrapper doesn't yet support opening for READ and WRITE at the same time
	if((dwDesiredAccess & GENERIC_READ) && (dwDesiredAccess & GENERIC_WRITE))
		return false;

	if(h == INVALID_HANDLE_VALUE)
		return h;

	s = state_create(QUEUE_SIZE);
	if (s == NULL)
	{
		// Out of memory
		SetLastError(0x11223322);
		return INVALID_HANDLE_VALUE;
	}

	s->file_handle = h;
	s->count = 0;
	s->first = 0;
	s->dwDesiredAccess = dwDesiredAccess;

	// We need to reopen the file again when closing to ajust its file length
	_tcscpy_s(s->file_name, _MAX_PATH, lpFileName);

	return (HANDLE)s;
}
