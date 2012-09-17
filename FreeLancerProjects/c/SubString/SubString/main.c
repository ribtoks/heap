#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <time.h>
#include "SubstringFunctions.h"

#define iterationsCount 60
#define ownTextSize 2000000

// свой текст, который пользователь может ввести
char owntext[ownTextSize];
int readCharsSize;

// возможное значения пути к файлу
char fileName[1000];

// указатель на функцию, которая может 
// находить подстроку в строке
// первый параметр - строка-текст
// второй параметр - строка-образец
// третий параметр - размер текста
// четвертый параметр - размер образца
//
// фукнция возвращает позицию образца в тексте
// или -1, если он не был найден
typedef int (*SearchingFunction)(const char*, const char*, int, int);


/*
	Функция принимает указатель на функцию 
	которая находит вхождение подстроки в строке.
*/
void CompareOneTime(SearchingFunction func, const char* substring, const char* funcName)
{
	int posMy;
	clock_t start, end;
	double myFuncTime;
	int size = strlen(substring);

	// поиск с помощью своего алгоритма

	// засечь время начала
	start = clock();

	// найти позицию строки с помощью моей функции
	posMy = func(owntext, substring, readCharsSize, size);

	// засечь время конца
	end = clock();

	// вичислить время работы своего алгоритма
	myFuncTime = ((double)(end - start) / CLOCKS_PER_SEC);

	printf(funcName);
	printf(" -  Position: %d  Time: %f\n\n", posMy, myFuncTime);
}

// функция спрашывает пользователя
// и возвращает 0 в случае положительного 
// ответа и -1 в случае отрицательного
int AskUser(const char* askWhat)
{
	char c;

	// вивести диалог вопроса пользователю
	printf(askWhat);
	printf(" (y/n) >:");

	// пользовательский ответ
	scanf("%c", &c);

	// убрать с потока введеный символ
	getchar();
	if (c == 'y' || c == 'Y')
		// успех
		return 0;
	// ответ - no
	return -1;
}

// функция считывает весь текст с файла
// в глобальный масив
int ReadUserTextFromFile()
{
	char ch;
	int i = 0;
	FILE* file = 0;

	// открыть файл в режиме чтения 'r' - read
	file = fopen(fileName, "r");
	if (file == 0)
	{
		printf("Warning: Can not open file. Invalid path.\n");
		return -1;
	}

	ch = getc(file);
	i = 0;
	memset(owntext, 0, ownTextSize*sizeof(char));
	// чтение всего файла
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
	//readCharsSize = strlen(owntext);

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

		// задержать закривание консоли до того, как пользователь нажмет Enter
		printf("Press \"Enter\" to exit...");
		getch();
		return 0;
	}

	strcat(fileName, argv[1]);

	if (ReadUserTextFromFile() == -1)
	{
		printf("Something wrong with parameters. Exiting...\n");

		// задержать закривание консоли до того, как пользователь нажмет Enter
		printf("\n\nPress \"Enter\" to exit...");

		getch();
		return 0;
	}

	CompareOneTime(Moore_Horspool, argv[2], "Moore-Horspool");
	CompareOneTime(TurboBoyerMoore, argv[2], "Turbo Boyer-Moore");
	

	// поиск с помощью стандартного алгоритма
	
	// засечь время начала
	start = clock();
	
	// найти позицию строки с помощью стандартной функции
	posStardart =  strstr(owntext, argv[2]) - owntext;	

	// засечь время конца
	end = clock();

	// вичислить время работы своего алгоритма
	standartFuncTime = ((double)(end - start) / CLOCKS_PER_SEC);

	printf("Standart function strstr() -  Position: %d  Time: %f\n\n", posStardart, standartFuncTime);



	// задержать закривание консоли до того, как пользователь нажмет Enter
	printf("\n\nPress \"Enter\" to exit...");

	getch();

	return 0;
}