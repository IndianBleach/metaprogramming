#pragma once

#include <windows.h>

class DebugFile
{
public:
	static BOOL GetFileNameFromHandle(HANDLE hFile, TCHAR* fileName);
};

