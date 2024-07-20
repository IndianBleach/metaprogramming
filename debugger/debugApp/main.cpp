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



	//CreateEvent(NULL, TRUE, FALSE, ts);


	OutputDebugString(TEXT("Hello World!\n"));

	DebugBreak();

	printf("i*2=%i\n", 20 * 2);

	Sleep(2000);
	return 0;
}