#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <tchar.h>



#define tamx 35
#define tamy 25


typedef struct map {
	int teste;
	bool ocupado; // existe alguma "coisa" neste ponto do mapa
	bool primeiroPonto; // se este é p primeiro ponto do objecto cord "0""0" do objecto
	struct map *firstpoint; //ponteiro para o primeiro ponto do objecto
	void *tipo;//ponteiro para as definiçoes do tipo em memoria
}mappoint;

typedef struct {
	char nome[20];
	char bitmap[100];
	int x;
	int y;
	int tipo;
	int speed;
}naveinvagrande;

typedef struct {
	char nome[20];
	char bitmap[100];
	int x;
	int y;
	int tipo;
	int speed;
}naveinvapequena;

typedef struct {
	char nome[20];
	char bitmap[100];
	int x;
	int y;
	int tipo;
	int speed;
}bomba;

typedef struct {
	char nome[20];
	char bitmap[100];
	int x;
	int y;
	int tipo;
}navejogadora;

typedef struct {
	char nome[20];
	char bitmap[100];
	int x;
	int y;
	int tipo;
	int speed;
}powerup;

int interfacee(int x, int y,void *pctx);
int crianaves(int numero);
int definicoes();
DWORD WINAPI Naveinimiga(LPVOID lparam);
DWORD WINAPI MotorJogo(LPVOID lparam);

mappoint mapa[tamx][tamy];
naveinvagrande defnaveinvagrande;
naveinvapequena defnaveinvapequena;
bomba defbomba;
navejogadora defnavejogadora;
powerup defpowerup;


int _tmain() {     // main main main main main main main main main main main main main main main main main main main main
	HANDLE MutexMapa;
	
	definicoes();

	MutexMapa = CreateMutex(NULL,FALSE,TEXT("MutexMapa"));
	if (MutexMapa==NULL) 
		printf("\nErro no ao criar mutex do mapa\n");


	crianaves(0);
	interfacee(tamx,tamy,mapa);

	return 0;
}

int crianaves(int numero) {
	int option,i,ee;
	HANDLE a;
	int familiaid[5] = { 1,2,3,4,5 };
	printf("\nnumero de naves por tipo\n");
	scanf_s("%d", &option);

	for (i = 0;i<5;i++) {
		for (ee = 0;ee<option;ee++) {
			printf("a lancar uma thread\n");
			a = CreateThread(NULL, 0, Naveinimiga, (LPVOID)&familiaid[i], 0, NULL);
		}
	}
	system("pause");
	system("cls");
	return 0;
}

DWORD WINAPI Naveinimiga(LPVOID lparam) {
	int i, e,ii,ee, x=99999, y=99999,tipox,tipoy,livrex=0;
	int livre = 1;
	int *familia;
	HANDLE mutex;
	familia = (int *)lparam;
	
	mutex = OpenMutex(SYNCHRONIZE, FALSE, TEXT("MutexMapa"));

	
	
	if (*familia == defnaveinvagrande.tipo) {
		tipox = defnaveinvagrande.x;
		tipoy = defnaveinvagrande.y;
	}
	if (*familia == defnaveinvapequena.tipo) {
		tipox = defnaveinvapequena.x;
		tipoy = defnaveinvapequena.y;
	}
	if (*familia == defbomba.tipo) {
		tipox = defbomba.x;
		tipoy = defbomba.y;
	}
	if (*familia == defnavejogadora.tipo) {
		tipox = defnavejogadora.x;
		tipoy = defnavejogadora.y;
	}
	if (*familia == defpowerup.tipo) {
		tipox = defpowerup.x;
		tipoy = defpowerup.y;
	}
	
	WaitForSingleObject(mutex, INFINITE);
	for (e = 0;e<tamy;e++) {
		for (i = 0;i<tamx;i++) {
			//anda mapa
			for (ee = e-1;ee<(e+tipoy+1);ee++) { 
				for (ii = i-1;ii<(i+tipox+1);ii++) {
					if (mapa[ii][ee].ocupado==1 || ii>=tamx || ii == 0){
						livre = 1;
					}
				}
			}
			if (livre==0) {
				x = e;
				y = i;
				for (ee = e;ee<(e+tipoy);ee++) {
					for (ii = i;ii<(i+tipox);ii++) {
						mapa[ii][ee].ocupado = 1;
						if(ee==x && ii==y){
							mapa[ii][ee].firstpoint = (mappoint *)&mapa[x][y];// Era pa ser um ponteiro para NULL mas para evitar foturos problemas vou usar o pronteiro para ele proprio so para verificar que esta tudo bem.
						}
						else{
							mapa[ii][ee].firstpoint = (mappoint *)&mapa[x][y];
						}
					}
				}
				i = tamx + 1;
				e = tamy + 1;
			}
			livre = 0;
		}
	}
	if (x == 99999) {
		printf("*******Nao ha espaço para esta nave do tipo %d!!\n",*familia);
		return 0;
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

int definicoes() {
	int i, e;

	for (i = 0;i < tamx;i++) {
		for (e = 0;e < tamy;e++) {
			mapa[i][e].ocupado = 0;
		}
	}

	defnaveinvagrande.tipo=1;
	defnaveinvagrande.x=4;
	defnaveinvagrande.y=4;

	defnaveinvapequena.tipo=2;
	defnaveinvapequena.x=3;
	defnaveinvapequena.y=2;

	defbomba.tipo=3;
	defbomba.x=1;
	defbomba.y=1;

	defnavejogadora.tipo=4;
	defnavejogadora.x=4;
	defnavejogadora.y=2;

	defpowerup.tipo=5;
	defpowerup.x=2;
	defpowerup.y=2;

	return 0;
}

DWORD WINAPI MotorJogo(LPVOID lparam) {
	return 0;
}