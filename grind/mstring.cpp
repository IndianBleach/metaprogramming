#include "mstring.h"
#include <stdio.h>

void mstring::strcpy(char* dest, const char* from)
{
	int i = 0;
	while (char p = *from++) {
		if (p == '\0') break;
		if (dest[i] == '\0') break;

		dest[i++] = p;
	}
};
