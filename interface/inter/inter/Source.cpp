#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <tchar.h>



#define tamx 30
#define tamy 20

typedef struct map {
	bool ocupado; // existe alguma "coisa" neste ponto do mapa
	bool primeiroPonto; // se este é p primeiro ponto do objecto cord "0""0" do objecto
	struct map *firstpoint; //ponteiro para o primeiro ponto do objecto
	void *tipo;//ponteiro para as definiçoes do tipo em memoria
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
int crianaves(int numero);
DWORD WINAPI Naveinimiga(LPVOID lparam);

mappoint mapa[30][20];
navemedia navemedia1;

int _tmain() {
	int i, e;
	HANDLE MutexMapa;
	MutexMapa = CreateMutex(NULL,FALSE,TEXT("MutexMapa"));
	if (MutexMapa==NULL) {
		printf("\nErro no mutex mapa\n");
	}
	
	//crianaves(0);
	navemedia1.x = 1;
	navemedia1.y = 1;

	for (i = 0;i < tamx;i++) {
	0	for (e = 0;e < tamy;e++) {
			mapa[i][e].ocupado=0;
		}
	}

	
	interfacee(tamx,tamy,mapa);

	return 0;
}

int crianaves(int numero) {
	int option,i;
	HANDLE a;
	printf("\nnumero de naves/threads\n");
	scanf_s("%d", &option);

	for (i = 0;i<option;i++) {
		printf("a lançar uma thread\n");

		a=CreateThread(NULL, 0, Naveinimiga, NULL, 0, NULL);
	}
	system("pause");
	system("cls");
	return 0;
}

DWORD WINAPI Naveinimiga(LPVOID lparam) {
	int i, e,ii,ee, x, y,tipo,tipox,tipoy,livrex=0;
	HANDLE mutex;
	tipox = navemedia1.x;
	tipoy = navemedia1.y;
	
	mutex = OpenMutex(SYNCHRONIZE, TRUE, TEXT("MutexMapa"));//mutex enquanto procura lugar e cria o objecto nave no mapa
	for (e = 0;e<tamx;e++) {
		for (i = 0;i<tamy;i++) {
			if (mapa[e][i].ocupado == 0) {//pensar nisto de testar se ha espaço suficiente para a nave em questao, agora cama
				livrex++
			}

		}
	}
	ReleaseMutex(mutex);
	
	return 0;
}

int interfacee(int x, int y,void *pctx) {
	
	//mappoint **mapa;
	//mapa = (mappoint **)pctx;
	HANDLE mutex;

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
				mutex = OpenMutex(SYNCHRONIZE, TRUE, TEXT("MutexMapa"));
				if (mutex == NULL) {
					printf("\nErro a abrir o mutex do mapa!\n\n");
					return 0;
				}
				if (mapa[i][e].ocupado == 1) {
					printf(" * ");
				}
				else{
					printf("   ");
				}
				ReleaseMutex(mutex);
			}
		}
		printf("\n");
	}
	return 0;
}