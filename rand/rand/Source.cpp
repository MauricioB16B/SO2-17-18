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


int randominate(int range_min, int range_max) {
	errno_t err;
	unsigned int number;
	err = rand_s(&number);
	if (err != 0)
		return -1;
	int num = (int)((double)number / ((double)UINT_MAX + 1) * range_max + range_min);
	return num;
}

int random_number(int min_num, int max_num)
{
	int result = 0, low_num = 0, hi_num = 0;

	if (min_num < max_num)
	{
		low_num = min_num;
		hi_num = max_num + 1; // include max_num in output
	}
	else {
		low_num = max_num + 1; // include max_num in output
		hi_num = min_num;
	}

	srand(time(NULL));
	result = (rand() % (hi_num - low_num)) + low_num;
	return result;
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


		printf_s("%d\n", random_number(0,100));
		Sleep(500);
	}
}