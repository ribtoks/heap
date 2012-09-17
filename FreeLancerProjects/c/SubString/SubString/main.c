#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <time.h>
#include "SubstringFunctions.h"

#define iterationsCount 60
#define ownTextSize 2000000

char owntext[ownTextSize];
int readCharsSize;

char fileName[1000];

typedef int (*SearchingFunction)(const char*, const char*, int, int);


void CompareOneTime(SearchingFunction func, const char* substring, const char* funcName)
{
	int posMy;
	clock_t start, end;
	double myFuncTime;
	int size = strlen(substring);


	start = clock();

	posMy = func(owntext, substring, readCharsSize, size);

	end = clock();

	myFuncTime = ((double)(end - start) / CLOCKS_PER_SEC);

	printf(funcName);
	printf(" -  Position: %d  Time: %f\n\n", posMy, myFuncTime);
}

int AskUser(const char* askWhat)
{
	char c;

	printf(askWhat);
	printf(" (y/n) >:");

	scanf("%c", &c);

	getchar();
	if (c == 'y' || c == 'Y')
		return 0;
	return -1;
}

int ReadUserTextFromFile()
{
	char ch;
	int i = 0;
	FILE* file = 0;

	file = fopen(fileName, "r");
	if (file == 0)
	{
		printf("Warning: Can not open file. Invalid path.\n");
		return -1;
	}

	ch = getc(file);
	i = 0;
	memset(owntext, 0, ownTextSize*sizeof(char));
	while (ch != EOF)
	{
		owntext[i] = ch;
		ch = getc(file);
		++i;

		if (i >= ownTextSize - 1)
		{
			printf("\n\nFile is too large! Max size is 2 milion characters!\n\n");
			break;
		}
	}

	owntext[i] = '\0';
	readCharsSize = i;

	fclose(file);
	printf("\nText was read.\n\n");
	
	if (AskUser("Write file text on console?") == 0)
	{
		printf("\n");
		puts(owntext);
		printf("\n");
	}

	return 0;
}

int main(int argc, char* argv[])
{
	int posStardart;
	clock_t start, end;
	double standartFuncTime;

	if (argc != 3)
	{
		printf("\nUsage: SubString.exe <filename> <string to find>\n\n");
		printf("      <filename> - path to file, where string must be found\n");
		printf("<string to find> - string without spaces, which is going to be found\n\n");

		printf("Sample \"SubString.exe C:\\test.txt myprettyword\"\n\n");

		printf("Press \"Enter\" to exit...");
		getch();
		return 0;
	}

	strcat(fileName, argv[1]);

	if (ReadUserTextFromFile() == -1)
	{
		printf("Something wrong with parameters. Exiting...\n");

		printf("\n\nPress \"Enter\" to exit...");

		getch();
		return 0;
	}

	CompareOneTime(Moore_Horspool, argv[2], "Moore-Horspool");
	CompareOneTime(TurboBoyerMoore, argv[2], "Turbo Boyer-Moore");
	

	
	start = clock();
	
	posStardart =  strstr(owntext, argv[2]) - owntext;	

	end = clock();

	standartFuncTime = ((double)(end - start) / CLOCKS_PER_SEC);

	printf("Standart function strstr() -  Position: %d  Time: %f\n\n", posStardart, standartFuncTime);



	printf("\n\nPress \"Enter\" to exit...");

	getch();

	return 0;
}
