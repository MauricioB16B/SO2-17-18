#include<stdio.h>
#include<stdlib.h>

#define tamx 30
#define tamy 20

typedef struct map {
	bool ocupado; // existe alguma "coisa" neste ponto do mapa
	bool primeiroPonto; // se este é p primeiro ponto do objecto cord "0""0" do objecto
	struct map *firstpoint; //ponteiro para o primeiro ponto do objecto
	void *tipo;//ponteiro para as definiçoes do dono em memoria
	//evento 
}mappoint;

typedef struct {
	char nome[20];
	char bitmap[100];
	int x;
	int y;
	int tipo;
}navegrande;

typedef struct {
	char nome[20];
	char bitmap[100];
	int x;
	int y;
	int tipo;
}navepequena;

typedef struct {
	char nome[20];
	char bitmap[100];
	int x;
	int y;
	int tipo;
}navemedia;

typedef struct {
	char nome[20];
	char bitmap[100];
	int x;
	int y;
	int tipo;
}navejogadora;

int interfacee(int x, int y,void *pctx);

void main() {
	int i,e;
	
	mappoint **mapa;
	mapa = (mappoint **)malloc(tamx * sizeof *mapa);
	for (i = 0; i < tamx; i++) {
		mapa[i] = (mappoint *)malloc(tamy * sizeof *mapa[i]);
	}



	for (i = 0;i < tamx;i++) {
		for (e = 0;e < tamy;e++) {
			mapa[i][e].ocupado=0;
		}
	}

	
	interfacee(tamx,tamy,mapa);
}

int interfacee(int x, int y,void *pctx) {
	
	mappoint **mapa;
	mapa = (mappoint **)pctx;

	
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
				if (mapa[i][e].ocupado == 1) {
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