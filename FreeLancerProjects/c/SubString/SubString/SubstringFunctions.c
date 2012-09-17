#include "SubstringFunctions.h"
#include <string.h>
#include <stdlib.h>

//-------------------------------------
// раздел алгоритма Мура-Хорлспула
//-------------------------------------

// длина таблицы символов
#define ASIZE 257

#define XSIZE 65537

/*
	Функция, которая ищет первое вхождение 
	подстроки в строку методом Мура-Хорлспула.

	Входные параметры:
	source - текст, в котором ищут вхождение подстроки
	substring - подстрока, вхождение которой ищут
	sourceSize - размер текста, в котором ищут
	length - размер подстроки, которую ищут

	Возвращает позицию найденой подстроки, или -1
	 в случае отсутствия ее в тексте
*/
int Moore_Horspool(const char* source, const char* substring, int sourceSize, int length)
{
	int i, a, j;

	// масив сдвигов каждой буквы. Размер масива - 
	// количество букв в таблице символов
	int characterShift[ASIZE];

	unsigned char ch, lastch;
 
	// .......................
	// Дополнительные расчеты
	// .......................

	// сначала сдвиг всех символов - длина масива
	for (a = 0; a < ASIZE; ++a) 
		characterShift[a] = length;
	
	// для тех символов, которые есть в искомой подстроке,
	// обчислить новую длину сдвига - их позиция от конца строки
	for (j = 0; j < length - 1; ++j) 
		characterShift[ substring[j] ] = length - j - 1;
 

	// .............
	// начало поиска
	// .............

	// запомнить последний символ подстроки
	lastch = substring[length - 1];
	i = 0;
	// цикл по участках длиной в подстроку со смещением
	while (i <= sourceSize - length) 
	{
		// взять последний символ данного участка
		ch = source[i + length - 1];

		// если совпал последний символ
		if (ch == lastch)
			// проверить весь участок памяти на предмет совпадения
	        if (memcmp(&source[i], substring, length - 1) == 0) 
				// если не возвращать i, то можна искать
				// все вхождения образца в тексте
				return i;

		// переход на следующий участок
	    i += characterShift[ch];
	}

	return -1;
}

//----------------------------------------
// раздел алгоритма Турбо Боуер-Мура
// Расчеты для "плохих сдвигов"
/*
	Входные параметры:
	substring - подстрока, которую мы ищем
	length - ее длина
	badCharShifts - масив "плохих сдвигов", который данная функция заполняет
*/
void PreprocessBadChars(const char *substring, int length, int badShifts[]) 
{
   int i; 
   for (i = 0; i < ASIZE; ++i)
	  // сначала сдвиг всех символов - длина масива
      badShifts[i] = length;

   for (i = 0; i < length - 1; ++i)
	// для тех символов, которые есть в искомой подстроке,
	// обчислить новую длину сдвига - их позиция от конца строки
      badShifts[ substring[i] ] = length - i - 1;
}
 
/*
	Функция для дополнительных 
	вичислений "хорошых" суффиксов

	Параметры:
	substring - входная строка
	length - ее длина
	suff - масив суффиксов, который и будет
		заполнен в результате вызова функции
*/ 
void ProcessSuffixes(const char *substring, int length, int *suff) 
{
   int f, g, i;
 
   suff[length - 1] = length;
   g = length - 1;
   for (i = length - 2; i >= 0; --i) 
   {
      if (i > g && suff[i + length - 1 - f] < i - g)
         suff[i] = suff[i + length - 1 - f];
      else 
	  {
         if (i < g)
            g = i;

         f = i;
         while (g >= 0 && substring[g] == substring[g + length - 1 - f])
            --g;
         suff[i] = f - g;
      }
   }
}
// расчеты для "хороших сдвигов"
/*
	Входные параметры:
	substring - подстрока, которую мы ищем
	length - ее длина
	goodSuffixes - масив "плохих сдвигов", который данная функция заполняет
*/
void PreprocessGoodShifts(const char *substring, int length, int goodShifts[]) {
   int i, j, suff[XSIZE];
 
   ProcessSuffixes(substring, length, suff);
 
   for (i = 0; i < length; ++i)
      goodShifts[i] = length;

   j = 0;
   // для всех символов строки
   for (i = length - 1; i >= 0; --i)
      if (suff[i] == i + 1)
         for (; j < length - 1 - i; ++j)
            if (goodShifts[j] == length)
				// установить пока для хорошего сдвига
				// позицию символа от конца строки
               goodShifts[j] = length - 1 - i;

   for (i = 0; i <= length - 2; ++i)
      goodShifts[length - 1 - suff[i]] = length - 1 - i;
}

/*
	Функция, которая ищет первое вхождение 
	подстроки в строку методом Турбо Боуер-Мура.

	Входные параметры:
	source - текст, в котором ищут вхождение подстроки
	substring - подстрока, вхождение которой ищут
	sourceSize - размер текста, в котором ищут
	length - размер подстроки, которую ищут

	Возвращает позицию найденой подстроки, или -1
	 в случае отсутствия ее в тексте
*/
int TurboBoyerMoore(char *source, char* substring, int sourceSize, int length) 
{
   int bcShift, i, j, shift, u, v, turboShift,
       goodSuffixes[XSIZE], badChars[ASIZE];

   // вичислить "хорошые сдвиги"
   PreprocessGoodShifts(substring, length, goodSuffixes);

   // вичислить "плохие символы" - символы,
   // через которые невоозможно будет 
   // использовать турбо-сдвиг
   PreprocessBadChars(substring, length, badChars);

   
	// ...........................
	// Собственно, поиск в строке
	// ...........................

   j = u = 0;
   shift = length;
   while (j <= sourceSize - length) 
   {
      i = length - 1;
	  // пока совпадают символы в тексте и образце
      while (i >= 0 && substring[i] == source[i + j]) 
	  {
         --i;
         if (u != 0 && i == length - 1 - shift)
            i -= u;
      }
	  // нашли вхождение
      if (i < 0) 
	  {
         return j;

		 // если бы здесь не возвращать позицию первого вхождения
		 // то можна искать все вхожденя подстоки в строку
         shift = goodSuffixes[0];
         u = length - shift;
      }
      else 
	  {
         v = length - 1 - i;
         turboShift = u - v;

         bcShift = badChars[ source[i + j] ] - length + 1 + i;

		 // shift - максимум с {turboShift, bcShift, goodSuffixes[i]}
         shift = max(turboShift, bcShift);
         shift = max(shift, goodSuffixes[i]);

		 // если максимум - goodSuffixes[i]
         if (shift == goodSuffixes[i])
            u = min(length - shift, v);
         else 
		 {
           if (turboShift < bcShift)
              shift = max(shift, u + 1);
           u = 0;
         }
      }
      j += shift;
   }

   // если до сих пор не было возврата из функции
   // значит не нашли строку - возвратить -1
   return -1;
}