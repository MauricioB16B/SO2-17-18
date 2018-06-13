#define _CRT_RAND_S
#include <limits.h> 
//Before <stdlib.h> to implement a thread-safe rand_s()

#include <stdio.h>
#include <io.h>
#include <Windows.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <tchar.h>
#include <conio.h>
#include <stdlib.h>
#include <math.h>
#include "algorithms.h"

int RangedRandDemo(int range_min, int range_max)
{
	//return range_min + rand() % (range_max + 1 - range_min);

	// Generate random numbers in the half-closed interval  
	// [range_min, range_max). In other words,  
	// range_min <= random number < range_max 
	//return (int)rand() / (RAND_MAX + 1) * (range_max - range_min) + range_min;
	errno_t err;
	unsigned int number;
	err = rand_s(&number);
	if (err != 0)
		return -1;
	int num = (int)((double)number / ((double)UINT_MAX + 1) * range_max + range_min);
	return num;
}

/*Distance betwween two points */
int distanceBetween2Points(int x1,int y1,int x2,int y2)
{
	int diffY = abs(x2 - x1);
	int diffX = abs(y2 - y1);

	return static_cast<int>(sqrt(pow(diffY, 2) + pow(diffX, 2)));
}

bool get4NeighborCords(int *xcords, int *ycords, const int x, const int y)
{
	// Up
	xcords[0] = x;
	ycords[0] = y + 1;

	// Down
	xcords[1] = x;
	ycords[1] = y - 1;
	
	// Right
	xcords[2] = x + 1;
	ycords[2] = y;
	
	// Left
	xcords[3] = x - 1;
	ycords[3] = y;
	return true;
}

/*
// Example how function works
// Get as parameter:
// Two vector with dimensions 8 And 2 integer coords
| x-- y-- | | x   y-- | | x++ y-- |   -> 0 1 2
| x-- y   | |dontcount| | x++ y   |   -> 3   4
| x-- y++ | | x   y++ | | x++ y++ |   -> 5 6 7
*/
bool getNeighborCords(int *xcords, int *ycords, const int x, const int y)
{
	// first row
	xcords[0] = x - 1;
	ycords[0] = y - 1;

	xcords[1] = x - 1;
	ycords[1] = y + 1;

	xcords[2] = x + 1;
	ycords[2] = y - 1;

	//second row
	xcords[3] = x - 1;
	ycords[3] = y;

	xcords[4] = x + 1;
	ycords[4] = y;

	xcords[5] = x - 1;
	ycords[5] = y + 1;

	//third row
	xcords[6] = x;
	ycords[6] = y + 1;

	xcords[7] = x + 1;
	ycords[7] = y + 1;

	return true;
}

int randomObj()
{
	int r = RangedRandDemo(1, 10);
	if (r < 8) // Common
		return 1; // food

	r = RangedRandDemo(1, 10);
	if (r < 7) // Uncommon
	{
		r = RangedRandDemo(1, 3);
		if (r == 1) return 2; // ice
		else if (r == 2) return 5; // oil
		else if (r == 3) return 6; // glue
	}

	r = RangedRandDemo(1, 7);
	switch (r)
	{
	case 1: return 3; // grenade
	case 2: return 4; // vodka
	case 3: return 7; // o-vodka
	case 4:	return 8; // o-oil
	case 5: return 9; // o-glue
	}
	
	return 10;
}

int avoidAgainstWall(int x, int y, int currentDir, int fieldSizeX, int fieldSizeY)
{
	int KEY_UP = 1, KEY_DOWN = 2, KEY_RIGHT = 3, KEY_LEFT = 4;
	int neighborX[4];
	int neighborY[4];
	get4NeighborCords(neighborX, neighborY, x, y);
	// Avoid go against border
	for (int i = 0; i < 4; i++)
		if (neighborX[i] <= 1 && currentDir != KEY_LEFT) // Left Border
			return KEY_RIGHT; // Right
		else if (neighborX[i] >= fieldSizeX - 1 &&
			currentDir != KEY_RIGHT) // Right Border
			return KEY_LEFT;
		else if (neighborY[i] <= 1 && currentDir != KEY_UP) // Top Border
			return KEY_DOWN;
		else if (neighborY[i] >= fieldSizeY - 1 && // Bottom Border
			currentDir != KEY_DOWN)
			return KEY_UP;
	return 0;
}