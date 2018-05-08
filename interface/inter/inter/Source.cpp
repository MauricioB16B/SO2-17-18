#include<stdio.h>

int interfacee(int x, int y);

int mapa[20][20];

void main() {
	int i,e;

	for (i = 0;i <= 20;i++) {
		for (e = 0;e <= 20;e++) {
			mapa[i][e] = 0;
		}
	}
	
	mapa[0][0] = 1;
	mapa[1][1] = 1;
	mapa[19][19] = 1;
	mapa[10][10] = 1;
	mapa[5][19] = 1;
	
	interfacee(30,25);
}

int interfacee(int x, int y) {
	int e, i,t=0;
	for (e = -2;e<y+1;e++) {
		for (i = -1;i<x+1;i++) {
			t = 0;
			if (e==-2 && t == 0) {
				if(i<10 && i>-1)
					printf("%d  ",i);
				if(i>=10 && i<x)
					printf("%d ", i);
				if (i <= -1 || i >= x)
					printf("   ");
				t = 1;
			}
			if (e==-1 && t == 0) {
				printf("___");
				t = 1;
			}
			if (e >= y && t == 0) {
				printf("---");
				t = 1;
			}
			if (i == -1 && t == 0) {
				if(e<10)
					printf("%d |",e);
				if(e>=10)
					printf("%d|", e);
				t = 1;
			}
			if (i >= x && t == 0) {
				printf("| |");
				t = 1;
			}
			if (t==0) {
				if (mapa[i][e] == 1) {
					printf(" * ");
				}
				else{
					printf("   ");
				}
			}
		}
		printf("\n");
	}
	return 0;
}