#include <stdlib.h>
#include "CodingFunctions.h"
#include <stdio.h>
#include <ctype.h>
#include <conio.h>
#include <string.h>

// максимальная длина пути к файлам
#define MAX_FILEPATH_LENGTH 1000

// строки - пути к файлам
char inputFileName[MAX_FILEPATH_LENGTH];
char outputFileName[MAX_FILEPATH_LENGTH];

//-----------------------------------------------------------------

// запустить кодирование файла
void RunEncodeFile(const char* fileName)
{
	strcat(outputFileName, fileName);
	// попытатся закодировать входной файл
	EncodeFile(strcat(inputFileName, fileName), strcat(outputFileName, ".myarchive"));

	remove(fileName);
	rename(outputFileName, fileName);
}

// запустить декодирование файла
void RunDecodeFile(const char* fileName)
{
	strcat(outputFileName, fileName);

	// попытатся разархивировать входной файл
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

		// задержать закривание консоли до того, как пользователь нажмет Enter
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
	
	// задержать закривание консоли до того, как пользователь нажмет Enter
	printf("Press \"Enter\" to exit...");
	getch();

	return 0;
}