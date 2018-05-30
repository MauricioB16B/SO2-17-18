#include<stdio.h>
#include<stdlib.h>

void main(){
	int niga[100][100];
	niga[50][50]=21230574;

	printf("%d\n",*(*(niga+50)+50));



}