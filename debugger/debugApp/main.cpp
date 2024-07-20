#include <iostream>
#include <stdio.h>
#include "windows.h"

#include <debugapi.h>

int s[1]{ 99 };

int main()
{
	LPCWSTR ts = L"test";

	printf("%p\n", s);

	int tt = 12;

	int* hval = (int*)malloc(sizeof(int));

	hval = &tt;

	printf("%p\n", hval);

	//
	//CreateEvent(NULL, FALSE, FALSE, NULL);
	//DebugBreak();

	int remp[2]{ 15, 145 };

	int five = remp[-5];


	//CreateEvent(NULL, TRUE, FALSE, ts);


	for (int i = 0; i < 10; i++)
	{
		DebugBreak();

		OutputDebugString(TEXT("Hello World!\n"));

		printf("i*2=%i\n", 20 * 2);
	}

	Sleep(2000);
	return 0;
}