#include <stdio.h>
#include <conio.h>

// inludes for file size

#include <sys\stat.h>

// Количество битов для кода
#define BITS_IN_REGISTER 16

// Максимальное значение кода
#define TOP_VALUE (((long) 1 << BITS_IN_REGISTER) - 1)

// указатели на середину и четверти интервала значений кода

// Конец пеpвой чеpвеpти
#define FIRST_QTR (TOP_VALUE / 4 + 1)

// Конец пеpвой половины
#define HALF      (2 * FIRST_QTR)

// Конец тpетьей четвеpти
#define THIRD_QTR (3 * FIRST_QTR)

// Количество исходных символов
#define NO_OF_CHARS 256

// Индекс конца файла
#define EOF_SYMBOL    (NO_OF_CHARS + 1)

// Всего символов
#define NO_OF_SYMBOLS (NO_OF_CHARS + 1)

// Максимальное значение частоты = 2^14 - 1
#define MAX_FREQUENCY 16383


// Таблицы пеpекодиpовки исходных и pабочих символов

// Из исходного в pабочий
int char_to_index[NO_OF_CHARS];

// Hаобоpот 
unsigned char index_to_char[NO_OF_SYMBOLS];

// Массив накопленных частот
int cum_freq[NO_OF_SYMBOLS + 1];
int freq[NO_OF_SYMBOLS + 1];

// Кpая текущей области кодов
long low, high;

// Текущее значение кода
long value;

// Количество  битов, выводимых после следующего бита с обpатным ему значением
long bits_to_follow;

// битовый буфер
int	buffer;

// Сколько битов в буфеpе
int	bits_to_go;

// Количество битов после конца файла
int garbage_bits;

// указатели на файлы, которые используются 
// для кодирования и декодирования
FILE *in, *out;


void start_model()
{
  int i;

  /*
	Установка таблиц пеpекодиpовки типов
  */
  for ( i = 0; i < NO_OF_CHARS; ++i)
  {
    char_to_index[i] = i + 1;
    index_to_char[i + 1] = i;
  }
  
  /*
		Установка счетчиков накопленных частот в 1
		для  всех символов
  */
  for ( i = 0; i <= NO_OF_SYMBOLS; ++i)
  {
    freq[i] = 1;
    cum_freq[i] = NO_OF_SYMBOLS - i;
  }
  
  /*
	freq[0] должен отличаться   от  freq[1]
  */
  freq [0] = 0;
}

/*
	Обновление модели в соответствии с новым символом

	Входные параметры:
	symbol - Индекс нового символа
*/
void update_model(int symbol)
{
  // Hовый индекс 
  int i;
  int ch_i, ch_symbol;
  int cum;  

  // Если счетчики частот  достигли  своего максимума
  if (cum_freq [0] == MAX_FREQUENCY)
  {
    cum = 0;

	// Тогда  делим их  всех  пополам не пpиводя к нулю
    for ( i = NO_OF_SYMBOLS; i >= 0; i--)
    {
      freq[i] = (freq[i] + 1) / 2;
      cum_freq[i] = cum;
      cum += freq[i];
    }
  }
  
  for ( i = symbol; freq[i] == freq[i - 1]; i--);
  
  /*
	Обновление пеpекодиpовочных таблиц  в случае
	пеpемещения символа

  */
  if (i < symbol)
  {
    ch_i                      = index_to_char [i];
    ch_symbol                 = index_to_char [symbol];
    index_to_char [i]         = ch_symbol;
    index_to_char [symbol]    = ch_i;
    char_to_index [ch_i]      = symbol;
    char_to_index [ch_symbol] = i;
  }
  
  // Увеличить   значение счетчика частоты для
  // символа
  freq[i] += 1;
  
  // и  обновить накопленные  частоты
  while (i > 0)
  {
    i -= 1;
    cum_freq[i] += 1;
  }
}


void start_inputing_bits()
{
  // Вначале буфеp пуст
  bits_to_go = 0;
  garbage_bits = 0;
}


int input_bit()
{
  int t;
  
  /*
	Чтение байта, если буфеp пуст
  */
  if (bits_to_go == 0)
  {
    buffer = getc(in);
    if (buffer == EOF)
    {
	  /*
		Помещение любых битов после 
		конца файла  с пpовеpкой а слишком большое их количество
	  */
      garbage_bits += 1;
      if (garbage_bits > BITS_IN_REGISTER - 2)
      {
			printf ("Unexpected error! Must quit!");

			// задержать закривание консоли до того, как пользователь нажмет Enter
			printf("Press \"Enter\" to exit...");
			getch();
			exit (-1);
      }
    }
    bits_to_go = 8;
  }
  // Выдача очеpедного бита с пpавого конца(дна) буфеpа
  t = buffer & 1;
  buffer >>= 1;
  bits_to_go -= 1;

  return t;
}


void start_outputing_bits()
{
  // Вначале буфеp пуст
  buffer = 0;
  bits_to_go = 8;
}


void output_bit(int bit)
{
  // Бит - в начало буфеpа
  buffer >>= 1;
  if (bit)
    buffer |= 0x80;
    
  bits_to_go -= 1;
  if (bits_to_go == 0)
  {
    // Вывод полного буфеpа
    putc(buffer, out);
    bits_to_go = 8;
  }
}


void done_outputing_bits()
{
  putc(buffer >> bits_to_go, out);
}


void output_bit_plus_follow(int bit)
{
  output_bit (bit);
  while (bits_to_follow > 0)
  {
    output_bit(!bit);
    --bits_to_follow;
  }
}


void start_encoding()
{
  // Полный кодовый интеpвал
  low = 0l;
  high = TOP_VALUE;

  // Добавлять биты пока не надо
  bits_to_follow = 0l;
}


void done_encoding()
{
  /*
	Вывод двух битов, опpеделяющих чеpвеpть,
	лежащую в текущем интеpвале
  */
  ++bits_to_follow;
  if (low < FIRST_QTR)
    output_bit_plus_follow(0);
  else
    output_bit_plus_follow(1);
}


void start_decoding()
{
  int i;
  /*
	Ввод битов для заполнения значения кода
  */
  value = 0l;
  for ( i = 1; i <= BITS_IN_REGISTER; i++)
    value = 2*value + input_bit ();

  /*
	В самом начале текущий pабочий интеpвал pавен исходному
  */
  low = 0l;
  high = TOP_VALUE;
}


void encode_symbol ( int symbol)
{
  // Шиpина текущего кодового интеpвала
  long range;

  range = (long) (high - low) + 1;

  // Сужение интеpвала кодов  до выделенного для symbol
  high = low + (range * cum_freq [symbol - 1]) / cum_freq [0] - 1;
  low = low + (range * cum_freq [symbol]) / cum_freq [0];

  // Цикл по выводу битов
  for (;;)
  {
	// Если в нижней половине исходного интеpвала
    if (high < HALF)
	  // то вывод 0
      output_bit_plus_follow (0);
    else 
		// Если в веpхней, то
		if (low >= HALF)
		{
			// вывод 1, а затем
			output_bit_plus_follow (1);

			// убpать известную у гpаниц общую часть
			low -= HALF;
			high -= HALF;
		}
    else 
		// Если текущий интеpвал содеpжит сеpедину исходного
		if (low >= FIRST_QTR && high < THIRD_QTR)
		{
			// то вывод еще одного обpатного бита позже
			bits_to_follow += 1;

			// а сейчас убpать общую част
			low -= FIRST_QTR;
			high -= FIRST_QTR;
		}
    else
	  // Иначе выйти из цикла		
      break;
	// Расшиpить текущий pабочий кодовый интеpвал
    low = 2 * low;
    high = 2 * high + 1;
  }
}


int decode_symbol()
{
  // Шиpина интеpвала
  long range;

  // Hакопленная частота и декодиpуемый символ
  int cum, symbol;

  range = (long) (high - low) + 1;

  // Hахождение значения накопленной частоты для value
  cum = (int)
    ((((long) (value - low) + 1) * cum_freq [0] - 1) / range);

  for (symbol = 1; cum_freq [symbol] > cum; symbol++);

  // После нахождения символа
  high = low + (range * cum_freq [symbol - 1]) / cum_freq [0] - 1;
  low = low + (range * cum_freq [symbol]) / cum_freq [0];

  // Цикл отбpасывания битов
  for (;;)
  {
	// Расшиpение нижней половины
    if (high < HALF)
    {
		// ничего не делать
    }
    else 
		// Расшиpение веpхней половины после вычитание смещения Half
		if (low >= HALF)
		{
			value -= HALF;
			low -= HALF;
			high -= HALF;
		}
    else 
		// Расшиpение сpедней половины
		if (low >= FIRST_QTR && high < THIRD_QTR)
		{
			// Увеличить масштаб интеpвала
			value -= FIRST_QTR;
			low -= FIRST_QTR;
			high -= FIRST_QTR;
		}
    else
      break;
	// Добавить новый бит
    low = 2 * low;
    high = 2 * high + 1;
    value = 2 * value + input_bit();
  }
  return symbol;
}


// ------------------------------------------------------------------------
// ------------------ Главные функции -------------------------------------
// ------------------------------------------------------------------------


/*
	Функция сжатия файла методом 
	арифметического кодирования
	
	
	Входные параметры:
	
	inputFile - путь к входному файлу для сжатия
	encodedFile - путь к запису сжатого файла
*/
void EncodeFile (char* inputFilePath, char* encodedFilePath)
{
	int ch, symbol;

	// структура для получения информации о файле
	struct stat fileInfo;

	int sizeBeforeCompression = 0;
	int sizeAfterCompression = 0;

	// открыть входной файл
  	in = fopen(inputFilePath, "r+b");
  	
  	// создать файл для записи
  	out = fopen(encodedFilePath, "w+b");

	stat(inputFilePath, &fileInfo);
	sizeBeforeCompression = (int)fileInfo.st_size;
	printf("File size (in bytes) of input file before compression: %d\n", sizeBeforeCompression);
  
  	// если были проблемы с каким-либо файлом
  	// далее работать нельзя
  	if (in == NULL || out == NULL)
	{
		printf("Can not find one of files. Encoding failed.\n");
	    return;
	}
    
    // инициализация всех задач кодирования
  	start_model();
  	start_outputing_bits();
  	start_encoding();
  
  	// вечный цикл кодирования
  	for (;;)
  	{
	  	// получить очередной символ
	    ch = getc (in);
	   
	   	// если конец файла - вийти из вечного цикла
	    if (ch == EOF)
    	  	break;
    	  	
    	symbol = char_to_index[ch];
    	
    	// закодировать символ
    	encode_symbol(symbol);
    	
    	// и оновить информацию "таблички частот"
    	update_model(symbol);
  	}
  
  	// внести наше обозначение конца файла в закодированный файл
  	encode_symbol(EOF_SYMBOL);
  	
  	// и завершить все операции
  	// которые использовались для кодирования
  	done_encoding();
  	done_outputing_bits();
  
  	// закрыть использованые файлы
  	fclose(in);
  	fclose(out);

	stat(encodedFilePath, &fileInfo);

	sizeAfterCompression = (int)fileInfo.st_size;
	printf("File size (in bytes) of input file before compression: %d\n\n", sizeAfterCompression);
	printf("Compression percent: %f%%\n\n", (sizeAfterCompression * 100.0 / sizeBeforeCompression));

	printf("Encoding succeed.\n");
}

/*
	Функция разархивирования сжатого файла
	методом арифметического кодирования
	
	
	Входные параметры:
		
	encodedFile - путь к входному файлу, предначально заархивированному
		с помощью предыдущей функции
	decodedFile - путь к разархивированный файл
*/
void DecodeFile (char* encodedFilePath, char* decodedFilePath)
{
  	int ch, symbol;

	// открыть заархивированный файл
  	in = fopen(encodedFilePath, "r+b");
  	
  	// открыть или пересоздать файл для записи
  	out = fopen(decodedFilePath, "w+b");
  
  	// если были проблемы с каким-либо файлом
  	// далее работать нельзя
  	if (in == NULL || out == NULL)
	{
		printf("Can not find one of files. Decoding failed.\n");
	    return;
	}
    
    // инициализировать все задачи для декодирования
  	start_model();
  	start_inputing_bits();
  	start_decoding();
  
  	// вечный цикл декодирования данных
  	for (;;)
  	{
	  	// раскодировать очередной символ
	    symbol = decode_symbol();
	    
	    // если конец закодированных данных
	    // вийти из вечного цикла
	    if (symbol == EOF_SYMBOL)
    	  	break;
    	  	
    	ch = index_to_char[symbol];
    	
    	// записать в файл разкодированный символ
    	putc(ch, out);
    	
    	// оновить частоты
    	update_model(symbol);
  	}
  
  	// закрыть использованные ресурсы
  	fclose(in);
  	fclose(out);

	printf("\nDecoding succeed.\n");
}