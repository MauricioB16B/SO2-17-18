#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <tchar.h>

#define MAPX = 1920
#define MAPY = 1080
#define BufferSize 100
#define Buffers 10

typedef struct obj {
	int id;
	int tipo;
	int x;
	int y;
	int tamx;
	int tamy;
	char bitmap[1024];
	char nome[256];
	struct obj * prox;
}obj;
typedef struct {
	int tipo;
	int aux1;
	int aux2;
	int aux3;
	int aux4;
	int aux5;
}msg;
typedef struct {

	msg dados[Buffers];
	int iEscrita;
	int iLeitura;

}bufferinfo;

int objid;

DWORD WINAPI criaojogo(LPVOID lparam);
int criaobj(obj * objectos, int tipo, int x, int y, int tamx, int tamy);
int listaobjectos(obj * objectos);
int apagaobjecto(obj * objectos, int id);
obj * mapeamento();
int buffercircular();
int tratamsg(msg data);

int _tmain() {     // main main main main main main main main main main main main main main main main main main main main
	int a;
	obj * objectos;
	objid = 0;
	objectos = mapeamento();
	while (1)
	{
		printf("\n	Servidor\n");
		printf("\n	Opcoes\n\n");
		printf("[ 1 ] -> Cria objecto\n");
		printf("[ 2 ] -> Lista objecto\n");
		printf("[ 3 ] -> BufferCircular\n");
		printf("[ 4 ] -> Apaga objecto\n");
		printf("[ 5 ] -> Sair\n");
		scanf_s("%d", &a);
		switch (a)
		{
		case 1:
			//criaobj(objectos);
			break;
		case 2:
			system("cls");
			listaobjectos(objectos);
			system("pause");
			break;
		case 3:
			buffercircular();
			system("pause");
			break;
		case 4:
			//apagaobjecto(objectos);
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

int buffercircular() {

	TCHAR NomeMemoria[] = TEXT("Nome da Memória Partilhada");
	TCHAR NomeSemaforoPodeEscrever[] = TEXT("Semáforo Pode Escrever");
	TCHAR NomeSemaforoPodeLer[] = TEXT("Semáforo Pode Ler");

	TCHAR NomeMutexIndice[] = TEXT("MutexLeitor");

	HANDLE PodeEscrever;
	HANDLE PodeLer;
	HANDLE hMemoria;
	HANDLE mutex;
	while (1) {

		bufferinfo *shm;
		int pos;

		PodeEscrever = CreateSemaphore(NULL, Buffers, Buffers, NomeSemaforoPodeEscrever);
		PodeLer = CreateSemaphore(NULL, 0, Buffers, NomeSemaforoPodeLer);
		hMemoria = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(bufferinfo), NomeMemoria);

		mutex = CreateMutex(NULL, FALSE, NomeMutexIndice);

		if (PodeEscrever == NULL || PodeLer == NULL || hMemoria == NULL) {
			_tprintf(TEXT("[Erro]Criação de objectos do Windows(%d)\n"), GetLastError());
			return -1;
		}

		shm = (bufferinfo*)MapViewOfFile(hMemoria, FILE_MAP_WRITE, 0, 0, sizeof(bufferinfo));
		if (shm == NULL) {
			_tprintf(TEXT("[Erro]Mapeamento da memória partilhada(%d)\n"), GetLastError());
			return -1;
		}

		for (int i = 0;; ++i) {
			WaitForSingleObject(PodeLer, INFINITE);

			WaitForSingleObject(mutex, INFINITE);
			//ler IN par aa var local POS
			pos = shm->iLeitura;
			shm->iLeitura = (shm->iLeitura + 1) % Buffers;//adiciona ate que é == a Buffers
			//Incrementar valor de IN
			ReleaseMutex(mutex);
			
			tratamsg (shm->dados[pos]);// copia data do buffer para variavel da funcao e liberta o buffer
			ReleaseSemaphore(PodeEscrever, 1, NULL);

		}

		UnmapViewOfFile(shm);
		CloseHandle(PodeEscrever);
		CloseHandle(PodeLer);
		CloseHandle(hMemoria);
		CloseHandle(mutex);
	}
	return 0;
}

int tratamsg(msg data) {
	HANDLE semaforo1 = CreateSemaphore(NULL, 1 , 1, TEXT("semaforo1"));
	WaitForSingleObject(semaforo1, INFINITE);
	switch (data.tipo){
	case 1:
		CreateThread(NULL, 0, criaojogo, (LPVOID)&data, 0, NULL);
		break;
	case 2:

		break;
	case 3:
		break;
	case 4:
		break;
	case 5:
		break;
	case 6:
		break;
	default:
		break;
	}
	WaitForSingleObject(semaforo1, INFINITE);
	ReleaseSemaphore(semaforo1, 1, NULL);
	return 0;
}

 DWORD WINAPI criaojogo(LPVOID lparam) {
	 msg *pdata = (msg *)lparam;
	 msg data = *pdata;
	 HANDLE semaforo1 = CreateSemaphore(NULL, 1, 1, TEXT("semaforo1"));
	 ReleaseSemaphore(semaforo1, 1, NULL);

	 obj * objectos = mapeamento();
	 int i;
	 for (i = 0;i < 30;i++) {
		 criaobj(objectos,1,i+10,i+20,10,10);
	 }
	 return 0;
}

 int criaobj(obj * objectos, int tipo, int x, int y, int tamx, int tamy) {
	 int i;

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
		_tprintf(TEXT("Nao foi possivel fazer o mapeamento do vector no espaco mapeado ERRO (%d)\n"), GetLastError());

	return ponteiro;
}

int apagaobjecto(obj * objectos,int id) {
	int i, e;

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