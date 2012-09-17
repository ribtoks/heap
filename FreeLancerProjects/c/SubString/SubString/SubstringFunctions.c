#include "SubstringFunctions.h"
#include <string.h>
#include <stdlib.h>


#define ASIZE 257

#define XSIZE 65537

int Moore_Horspool(const char* source, const char* substring, int sourceSize, int length)
{
	int i, a, j;

	int characterShift[ASIZE];

	unsigned char ch, lastch;
 

	for (a = 0; a < ASIZE; ++a) 
		characterShift[a] = length;
	
	for (j = 0; j < length - 1; ++j) 
		characterShift[ substring[j] ] = length - j - 1;
 


	lastch = substring[length - 1];
	i = 0;
	while (i <= sourceSize - length) 
	{
		ch = source[i + length - 1];

		if (ch == lastch)
	        if (memcmp(&source[i], substring, length - 1) == 0) 
				return i;

	    i += characterShift[ch];
	}

	return -1;
}

void PreprocessBadChars(const char *substring, int length, int badShifts[]) 
{
   int i; 
   for (i = 0; i < ASIZE; ++i)
      badShifts[i] = length;

   for (i = 0; i < length - 1; ++i)
      badShifts[ substring[i] ] = length - i - 1;
}
 
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
void PreprocessGoodShifts(const char *substring, int length, int goodShifts[]) {
   int i, j, suff[XSIZE];
 
   ProcessSuffixes(substring, length, suff);
 
   for (i = 0; i < length; ++i)
      goodShifts[i] = length;

   j = 0;
   for (i = length - 1; i >= 0; --i)
      if (suff[i] == i + 1)
         for (; j < length - 1 - i; ++j)
            if (goodShifts[j] == length)
               goodShifts[j] = length - 1 - i;

   for (i = 0; i <= length - 2; ++i)
      goodShifts[length - 1 - suff[i]] = length - 1 - i;
}

int TurboBoyerMoore(char *source, char* substring, int sourceSize, int length) 
{
   int bcShift, i, j, shift, u, v, turboShift,
       goodSuffixes[XSIZE], badChars[ASIZE];

   PreprocessGoodShifts(substring, length, goodSuffixes);

   PreprocessBadChars(substring, length, badChars);

   

   j = u = 0;
   shift = length;
   while (j <= sourceSize - length) 
   {
      i = length - 1;
      while (i >= 0 && substring[i] == source[i + j]) 
	  {
         --i;
         if (u != 0 && i == length - 1 - shift)
            i -= u;
      }
      if (i < 0) 
	  {
         return j;

         shift = goodSuffixes[0];
         u = length - shift;
      }
      else 
	  {
         v = length - 1 - i;
         turboShift = u - v;

         bcShift = badChars[ source[i + j] ] - length + 1 + i;

         shift = max(turboShift, bcShift);
         shift = max(shift, goodSuffixes[i]);

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

   return -1;
}
