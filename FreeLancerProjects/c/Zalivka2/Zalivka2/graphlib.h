// Константы
#define MAXARR 30  // Макс кол-во вершин многоугольника
#define MAXLST 30  // Макс размер списка активных ребер

// Размеры экрана для вывода
#define SWIDTH 75    // ширина
#define SHEIGHT 50   // высота

// Структура для хранения одной точки
struct SPixel
{
	float x;
	float y;
};

//тип связанный с данной структурой
typedef struct SPixel pixel;

// Заголовок функции заливки
void Fill(pixel points[], int count, char screen[SHEIGHT][SWIDTH]);