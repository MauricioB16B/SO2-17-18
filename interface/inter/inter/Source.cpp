#include<stdio.h>
#include<stdlib.h>

#define tamx 30
#define tamy 20

int interfacee(int x, int y,int **mapa);

void main() {
	int i,e;
	//int mapa[tamx][tamy];
	
	int **mapa;
	mapa = (int **)malloc(tamx * sizeof *mapa);
	for (i = 0; i<tamx; i++){
		mapa[i] = (int *)malloc(tamy * sizeof *mapa[i]);
	}


	for (i = 0;i < tamx;i++) {
		for (e = 0;e < tamy;e++) {
			mapa[i][e] = 0;
		}
	}
	mapa[0][0] = 1;
	mapa[1][1] = 1;
	mapa[29][19] = 1;
	mapa[10][10] = 1;
	mapa[5][19] = 1;

	
	interfacee(tamx,tamy,mapa);
}

int interfacee(int x, int y,int **mapa) {
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