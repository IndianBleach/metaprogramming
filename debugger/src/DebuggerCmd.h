#pragma once

#include <windows.h>

#define DEBUG_COMMAND_INVALID ((DWORD)0x0)
#define DEBUG_COMMAND_CONTINUE ((DWORD)0x1)



class DebuggerCmd
{
public:
	DWORD ReadNextCommand();
};
