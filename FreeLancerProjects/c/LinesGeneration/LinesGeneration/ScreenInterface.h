#pragma once

#include "PointStruct.h"
void InitScreen();

void ClearScreen();

void PrintScreen();

void SetPoint(int x, int y, unsigned char c);

int IsPointOk(int x, int y);

void TransformPoint(struct Point* p);
