#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <tchar.h>

#define MAPX = 1920
#define MAPY = 1080

typedef struct obj {
	int id;
	int tipo;
	int x;
	int y;
	int tamx;
	int tamy;
	char bitmap[1024];
	struct obj * prox;
}obj;

int objid;

int criaobj(obj * objectos);
int listaobjectos(obj * objectos);
int apagaobjecto(obj * objectos);
obj * mapeamento();

int _tmain() {     // main main main main main main main main main main main main main main main main main main main main
	int a;
	obj * objectos;
	objid = 0;
	objectos = mapeamento();
	while (1)
	{
		printf("\n	Opcoes\n\n");
		printf("[ 1 ] -> Cria objecto\n");
		printf("[ 2 ] -> Lista objecto\n");
		printf("[ 3 ] -> Mapeamento de memoria\n");
		printf("[ 4 ] -> Apaga objecto\n");
		printf("[ 5 ] -> Sair\n");
		scanf_s("%d", &a);
		switch (a)
		{
		case 1:
			criaobj(objectos);
			system("pause");
			break;
		case 2:
			system("cls");
			listaobjectos(objectos);
			system("pause");
			break;
		case 3:
			objectos = mapeamento();
			system("pause");
			break;
		case 4:
			apagaobjecto(objectos);
			system("pause");
			break;
		case 5:
			return 0;
			break;
		default:
			break;
		}
		system("cls");
	}


}


int criaobj(obj * objectos) {
	int i;
	
	int tipo;
	int x;
	int y;
	int tamx;
	int tamy;
	printf("\nTipo:");
	scanf_s("%d", &tipo);
	printf("\nx:");
	scanf_s("%d", &x);
	printf("\ny:");
	scanf_s("%d", &y);
	printf("\ntamx:");
	scanf_s("%d", &tamx);
	printf("\ntamy");
	scanf_s("%d", &tamy);

	for (i = 0;objectos[i].id != NULL;i++) {
	}
	objid++;
	objectos[i].id = objid;
	objectos[i].tipo = tipo;
	objectos[i].x = x;
	objectos[i].y = y;
	objectos[i].tamx = tamx;
	objectos[i].tamy = tamy;
	return 0;
}

int listaobjectos(obj * objectos) {
	int i;
	for (i = 0;objectos[i].id != NULL;i++) {
		printf("	id: %d\n	tipo: %d\n	x: %d\n	y: %d\n	Largura: %d\n	Altura: %d\n\n	********************************\n", objectos[i].id, objectos[i].tipo, objectos[i].x, objectos[i].y, objectos[i].tamx, objectos[i].tamy);
	}
	return 0;
}

obj * mapeamento() {
	TCHAR syNome[] = TEXT("Global\\GlobalGameData");
	HANDLE partilha;
	obj *ponteiro;

	partilha = CreateFileMapping(INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(obj)*200,
		syNome);

	if (partilha == NULL){
		_tprintf(TEXT("Nao foi possifel criar o mapeamento, problemas de permissao ERRO ""5"" (%d).\n"),GetLastError());
		return NULL;
	}

	if (GetLastError() == ERROR_ALREADY_EXISTS) // ERROR_ALREADY_EXISTS == 183
		_tprintf(TEXT("Ja existia este mapeamento de memoria no systema(%d).\n"), GetLastError());
	
	ponteiro = (obj *)MapViewOfFile(partilha,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		sizeof(obj) * 200);

	if (ponteiro == NULL)
		_tprintf(TEXT("coco no mapeamento\n"));

	return ponteiro;
}

int apagaobjecto(obj * objectos) {
	int id, i, e;
	printf("\nid:\n");
	scanf_s("%d", &id);
	for (i = 0;objectos[i].id != NULL;i++) {
		if (objectos[i].id==id) {
			for (e = i;objectos[e].id != NULL;e++) {
				objectos[e].id = objectos[e + 1].id;
				objectos[e].tipo = objectos[e + 1].tipo;
				objectos[e].x = objectos[e + 1].x;
				objectos[e].y = objectos[e + 1].y;
				objectos[e].tamx = objectos[e + 1].tamx;
				objectos[e].tamy = objectos[e + 1].tamy;
			}
			return 1;
		}
	}

	return 0;
}