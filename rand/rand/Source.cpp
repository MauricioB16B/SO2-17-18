#define _CRT_RAND_S  
#include <stdlib.h>  
#include <stdio.h>  
#include <time.h>  
#include <Windows.h>

void SimpleRandDemo(int n)
{
	// Print n random numbers.  
	int i;
	for (i = 0; i < n; i++)
		printf("  %6d\n", rand());
}

void RangedRandDemo(int range_min, int range_max, int n)
{
	// Generate random numbers in the half-closed interval  
	// [range_min, range_max). In other words,  
	// range_min <= random number < range_max  
	int i;
	for (i = 0; i < n; i++)
	{
		int u = (double)rand() / (RAND_MAX + 1) * (range_max - range_min)
			+ range_min;
		printf("  %6d\n", u);
	}
}

int randominate(int range_min, int range_max) {
	errno_t err;
	unsigned int number;
	err = rand_s(&number);
	if (err != 0)
		return -1;
	int num = (int)((double)number / ((double)UINT_MAX + 1) * range_max + range_min);
	return num;
}

int main(void)
{
	// Seed the random-number generator with the current time so that  
	// the numbers will be different every time we run.  
	/*
	srand((unsigned)time(NULL));

	SimpleRandDemo(0);
	printf("\n");
	RangedRandDemo(-100, 100, 1);
	*/
	while (true)
	{


		printf_s("%d\n", randominate(0, 100));
		Sleep(500);
	}
}