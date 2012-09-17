#include <stdio.h>
#include <conio.h>

// inludes for file size

#include <sys\stat.h>

#define BITS_IN_REGISTER 16

#define TOP_VALUE (((long) 1 << BITS_IN_REGISTER) - 1)

#define FIRST_QTR (TOP_VALUE / 4 + 1)

#define HALF      (2 * FIRST_QTR)

#define THIRD_QTR (3 * FIRST_QTR)

#define NO_OF_CHARS 256

#define EOF_SYMBOL    (NO_OF_CHARS + 1)

#define NO_OF_SYMBOLS (NO_OF_CHARS + 1)

#define MAX_FREQUENCY 16383



int char_to_index[NO_OF_CHARS];

unsigned char index_to_char[NO_OF_SYMBOLS];

int cum_freq[NO_OF_SYMBOLS + 1];
int freq[NO_OF_SYMBOLS + 1];

long low, high;

long value;

long bits_to_follow;

int	buffer;

int	bits_to_go;

int garbage_bits;

FILE *in, *out;


void start_model()
{
  int i;

  for ( i = 0; i < NO_OF_CHARS; ++i)
  {
    char_to_index[i] = i + 1;
    index_to_char[i + 1] = i;
  }
  
  for ( i = 0; i <= NO_OF_SYMBOLS; ++i)
  {
    freq[i] = 1;
    cum_freq[i] = NO_OF_SYMBOLS - i;
  }
  
  freq [0] = 0;
}

void update_model(int symbol)
{
  int i;
  int ch_i, ch_symbol;
  int cum;  

  if (cum_freq [0] == MAX_FREQUENCY)
  {
    cum = 0;

    for ( i = NO_OF_SYMBOLS; i >= 0; i--)
    {
      freq[i] = (freq[i] + 1) / 2;
      cum_freq[i] = cum;
      cum += freq[i];
    }
  }
  
  for ( i = symbol; freq[i] == freq[i - 1]; i--);
  
  if (i < symbol)
  {
    ch_i                      = index_to_char [i];
    ch_symbol                 = index_to_char [symbol];
    index_to_char [i]         = ch_symbol;
    index_to_char [symbol]    = ch_i;
    char_to_index [ch_i]      = symbol;
    char_to_index [ch_symbol] = i;
  }
  
  freq[i] += 1;
  
  while (i > 0)
  {
    i -= 1;
    cum_freq[i] += 1;
  }
}


void start_inputing_bits()
{
  bits_to_go = 0;
  garbage_bits = 0;
}


int input_bit()
{
  int t;
  
  if (bits_to_go == 0)
  {
    buffer = getc(in);
    if (buffer == EOF)
    {
      garbage_bits += 1;
      if (garbage_bits > BITS_IN_REGISTER - 2)
      {
			printf ("Unexpected error! Must quit!");

			printf("Press \"Enter\" to exit...");
			getch();
			exit (-1);
      }
    }
    bits_to_go = 8;
  }
  t = buffer & 1;
  buffer >>= 1;
  bits_to_go -= 1;

  return t;
}


void start_outputing_bits()
{
  buffer = 0;
  bits_to_go = 8;
}


void output_bit(int bit)
{
  buffer >>= 1;
  if (bit)
    buffer |= 0x80;
    
  bits_to_go -= 1;
  if (bits_to_go == 0)
  {
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
  low = 0l;
  high = TOP_VALUE;

  bits_to_follow = 0l;
}


void done_encoding()
{
  ++bits_to_follow;
  if (low < FIRST_QTR)
    output_bit_plus_follow(0);
  else
    output_bit_plus_follow(1);
}


void start_decoding()
{
  int i;
  value = 0l;
  for ( i = 1; i <= BITS_IN_REGISTER; i++)
    value = 2*value + input_bit ();

  low = 0l;
  high = TOP_VALUE;
}


void encode_symbol ( int symbol)
{
  long range;

  range = (long) (high - low) + 1;

  high = low + (range * cum_freq [symbol - 1]) / cum_freq [0] - 1;
  low = low + (range * cum_freq [symbol]) / cum_freq [0];

  for (;;)
  {
    if (high < HALF)
      output_bit_plus_follow (0);
    else 
		if (low >= HALF)
		{
			output_bit_plus_follow (1);

			low -= HALF;
			high -= HALF;
		}
    else 
		if (low >= FIRST_QTR && high < THIRD_QTR)
		{
			bits_to_follow += 1;

			low -= FIRST_QTR;
			high -= FIRST_QTR;
		}
    else
      break;
    low = 2 * low;
    high = 2 * high + 1;
  }
}


int decode_symbol()
{
  long range;

  int cum, symbol;

  range = (long) (high - low) + 1;

  cum = (int)
    ((((long) (value - low) + 1) * cum_freq [0] - 1) / range);

  for (symbol = 1; cum_freq [symbol] > cum; symbol++);

  high = low + (range * cum_freq [symbol - 1]) / cum_freq [0] - 1;
  low = low + (range * cum_freq [symbol]) / cum_freq [0];

  for (;;)
  {
    if (high < HALF)
    {
    }
    else 
		if (low >= HALF)
		{
			value -= HALF;
			low -= HALF;
			high -= HALF;
		}
    else 
		if (low >= FIRST_QTR && high < THIRD_QTR)
		{
			value -= FIRST_QTR;
			low -= FIRST_QTR;
			high -= FIRST_QTR;
		}
    else
      break;
    low = 2 * low;
    high = 2 * high + 1;
    value = 2 * value + input_bit();
  }
  return symbol;
}




void EncodeFile (char* inputFilePath, char* encodedFilePath)
{
	int ch, symbol;

	struct stat fileInfo;

	int sizeBeforeCompression = 0;
	int sizeAfterCompression = 0;

  	in = fopen(inputFilePath, "r+b");
  	
  	out = fopen(encodedFilePath, "w+b");

	stat(inputFilePath, &fileInfo);
	sizeBeforeCompression = (int)fileInfo.st_size;
	printf("File size (in bytes) of input file before compression: %d\n", sizeBeforeCompression);
  
  	if (in == NULL || out == NULL)
	{
		printf("Can not find one of files. Encoding failed.\n");
	    return;
	}
    
  	start_model();
  	start_outputing_bits();
  	start_encoding();
  
  	for (;;)
  	{
	    ch = getc (in);
	   
	    if (ch == EOF)
    	  	break;
    	  	
    	symbol = char_to_index[ch];
    	
    	encode_symbol(symbol);
    	
    	update_model(symbol);
  	}
  
  	encode_symbol(EOF_SYMBOL);
  	
  	done_encoding();
  	done_outputing_bits();
  
  	fclose(in);
  	fclose(out);

	stat(encodedFilePath, &fileInfo);

	sizeAfterCompression = (int)fileInfo.st_size;
	printf("File size (in bytes) of input file before compression: %d\n\n", sizeAfterCompression);
	printf("Compression percent: %f%%\n\n", (sizeAfterCompression * 100.0 / sizeBeforeCompression));

	printf("Encoding succeed.\n");
}


void DecodeFile (char* encodedFilePath, char* decodedFilePath)
{
  	int ch, symbol;

  	in = fopen(encodedFilePath, "r+b");
  	
  	out = fopen(decodedFilePath, "w+b");
  
  	if (in == NULL || out == NULL)
	{
		printf("Can not find one of files. Decoding failed.\n");
	    return;
	}
    
  	start_model();
  	start_inputing_bits();
  	start_decoding();
  
  	for (;;)
  	{
	    symbol = decode_symbol();
	    
	    if (symbol == EOF_SYMBOL)
    	  	break;
    	  	
    	ch = index_to_char[symbol];
    	
    	putc(ch, out);
    	
    	update_model(symbol);
  	}
  
  	fclose(in);
  	fclose(out);

	printf("\nDecoding succeed.\n");
}
