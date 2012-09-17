#include <stdlib.h>
#include "CodingFunctions.h"
#include <stdio.h>
#include <ctype.h>
#include <conio.h>
#include <string.h>

#define MAX_FILEPATH_LENGTH 1000

char inputFileName[MAX_FILEPATH_LENGTH];
char outputFileName[MAX_FILEPATH_LENGTH];


void RunEncodeFile(const char* fileName)
{
	strcat(outputFileName, fileName);
	EncodeFile(strcat(inputFileName, fileName), strcat(outputFileName, ".myarchive"));

	remove(fileName);
	rename(outputFileName, fileName);
}

void RunDecodeFile(const char* fileName)
{
	strcat(outputFileName, fileName);

	DecodeFile(strcat(inputFileName, fileName), strcat(outputFileName, ".unpacked"));

	remove(fileName);
	rename(outputFileName, fileName);	
}

int main (int argc, char *argv[])
{
	char c;

	if (argc < 2 || argc > 3)
	{
		printf("\nUsage: ArithmeticEncoding.exe <filename> <endodingType>\n\n");
		printf("    <filename> - path to file, which is going to be encoded/decoded\n");
		printf("<encodingType> - 'e' for encoding, 'd' for decoding\n\n");

		printf("Sample \"ArithmeticEncoding.exe C:\\test.txt e\"\n\n");

		printf("Press \"Enter\" to exit...");
		getch();
		return 0;
	}

	c = argv[2][0];

	if (c == 'e')
		RunEncodeFile(argv[1]);
	else
		if (c == 'd')
			RunDecodeFile(argv[1]);
	
	printf("Press \"Enter\" to exit...");
	getch();

	return 0;
}
