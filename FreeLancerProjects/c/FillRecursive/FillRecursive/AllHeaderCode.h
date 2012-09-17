#pragma once

struct Point
{
	int x;
	int y;
};

void InitializeScreen();



void ClearScreen();


void PrintScreen();

int IsPointOk(int x, int y);

void TransformPoint(struct Point* p);

int IsFilled(int x, int y);

void Fill(int x, int y);

void DrawLine(struct Point p1, struct Point p2);

int IsPointInsidePolygon (struct Point p[], int Number, int x, int y);

void FillPolygon(struct Point Points[], int Count, int x, int y);
