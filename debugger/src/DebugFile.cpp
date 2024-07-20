#include "DebugFile.h"
#include "DebugFile.h"

#include <tchar.h>
#include <string.h>
#include <psapi.h>
#include <strsafe.h>

#include <assert.h>

BOOL DebugFile::GetFileNameFromHandle(HANDLE hFile, TCHAR* fileName)
{
	BOOL readSuccess = FALSE;
	//TCHAR fileName[128];

	DWORD dwFileSizeHi = 0;
	DWORD dwFileSizeLo = GetFileSize(hFile, &dwFileSizeHi);

	if (dwFileSizeLo == 0 && dwFileSizeHi == 0)
	{
		_tprintf(TEXT("Cannot map a file with a length of zero.\n"));
		return FALSE;
	}

	HANDLE hFileMap;
	hFileMap = CreateFileMapping(hFile,
		NULL,
		PAGE_READONLY,
		0,
		1,
		NULL);

	if (!hFileMap)
	{
		DWORD stat = GetLastError();

		printf("FileUtils:GetFileNameFromHandle() can't create fileMap, returned NULL");
		return FALSE;
	}

	void* mapView = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 1);

	if (GetMappedFileName(GetCurrentProcess(), mapView, fileName, MAX_PATH))
	{
		int t = 3;
		return TRUE;
	}
	else
	{
		printf("FileUtils:GetFileNameFromHandle() GetMappedFileName return FALSE");
		return FALSE;
	}
}
