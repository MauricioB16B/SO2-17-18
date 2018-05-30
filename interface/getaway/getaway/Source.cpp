#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <tchar.h>

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

obj * mapeamento();
void ler(obj *objectos);

int _tmain() {
	obj * objectos;
	objectos = mapeamento();
	system("cls");

	while (1) {
		system("cls");
		ler(objectos);
		Sleep(500);
	}

	system("pause");
	return 0;
}

void ler(obj *objectos) {
	int i;
	for (i = 0;objectos[i].id != NULL;i++) {
		printf("	id: %d\n	tipo: %d\n	x: %d\n	y: %d\n	Largura: %d\n	Altura: %d\n\n	********************************\n", objectos[i].id, objectos[i].tipo, objectos[i].x, objectos[i].y, objectos[i].tamx, objectos[i].tamy);
	}
}

obj * mapeamento() {
	TCHAR syNome[] = TEXT("Global\\GlobalGameData");
	HANDLE partilha;
	obj *ponteiro;

	partilha = CreateFileMapping(INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(obj) * 300,
		syNome);

	if (partilha == NULL) {
		_tprintf(TEXT("Nao foi possifel criar o mapeamento no systema, problemas de permissao ERRO ""5"" (%d).\n"), GetLastError());
		return NULL;
	}

	ponteiro = (obj *)MapViewOfFile(partilha,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		sizeof(obj) * 300);

	if (ponteiro == NULL)
		_tprintf(TEXT("Nao foi possivel fazer o mapeamento do vector no espaco mapeado ERRO (%d)\n"),GetLastError());

	return ponteiro;
}