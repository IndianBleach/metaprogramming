#include "DebuggerCmd.h"
#include <stdio.h>
#include <string.h>

DWORD DebuggerCmd::ReadNextCommand()
{
	int c = getchar();
	char t = (char)c;

	if (t >= 'a' && t <= ('a' + 32))
	{
		switch (t)
		{
		case 'c':
			return DEBUG_COMMAND_CONTINUE;
		default:
			return DEBUG_COMMAND_INVALID;
		}
	}

	return DEBUG_COMMAND_INVALID;
}

