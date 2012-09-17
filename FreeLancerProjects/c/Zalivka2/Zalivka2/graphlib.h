#define MAXARR 30
#define MAXLST 30

#define SWIDTH 75
#define SHEIGHT 50

struct SPixel
{
	float x;
	float y;
};

typedef struct SPixel pixel;

void Fill(pixel points[], int count, char screen[SHEIGHT][SWIDTH]);
