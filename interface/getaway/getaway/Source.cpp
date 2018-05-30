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

int _tmain() {
	obj * objectos;
	objectos = mapeamento();
	system("pause");
	return 0;
}

obj * mapeamento() {
	TCHAR syNome[] = TEXT("Global\\GlobalGameData");
	HANDLE partilha;
	obj *ponteiro;

	partilha = OpenFileMapping(PAGE_READWRITE,
		FALSE,
		syNome);

	if (partilha == NULL) {
		_tprintf(TEXT("Nao foi possifel abrir o mapeamento,(%d)\nFile nao encontrado ERRO ""2""\nProblemas de permissao ERRO ""5""\n"), GetLastError());
		return NULL;
	}

	ponteiro = (obj *)MapViewOfFile(partilha,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		sizeof(obj) * 200);
	if (ponteiro == NULL)
		_tprintf(TEXT("coco no mapeamento (%d)\n"),GetLastError());

	return ponteiro;
}