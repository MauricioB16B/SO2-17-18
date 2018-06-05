#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <tchar.h>

#define BufferSize 100
#define Buffers 10
#define PIPE_NAME TEXT("\\\\.\\pipe\\main")



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
	char aux6[1024];
	char aux7[1024];
	char aux8[1024];
}msg;
typedef struct {

	msg dados[Buffers];
	int iEscrita;
	int iLeitura;

}bufferinfo;


obj * mapeamento();
void ler(obj *objectos);
int buffercircular();
int buffercircular2(int tipo, int aux1, int aux2, int aux3, int aux4, int aux5, char *aux6, char *aux7, char *aux8);
DWORD WINAPI RecebeClientesPipeGeral(LPVOID param);
void pipalhadas();
DWORD WINAPI TrataClientes(LPVOID param);

HANDLE ArrayHandles[255];

int _tmain() {
	int a;
	char ola[]{"Ines"};
	char ola2[]{ "Mauricio"};

////////////**//	#ifdef UNICODE
////////////**//		_setmode(_fileno(stdin), _O_WTEXT);
////////////**//		_setmode(_fileno(stdout), _O_WTEXT);
////////////**//	#endif

	while (1){
		printf("\n	Getaway\n");
		printf("\n	Opcoes\n\n");
		printf("[ 1 ] -> Cria Pipe geral [GETAWAY]<--[CLIENTE]\n");
		printf("[ 2 ] -> Cria Jogo classico\n");
		printf("[ 3 ] -> Mostra Objectos mapa\n");
		printf("[ 4 ] -> teste2\n");
		printf("[ 5 ] -> Sair\n");
		scanf_s("%d", &a);
		switch (a)
		{
		case 1:
			CreateThread(NULL, 0, RecebeClientesPipeGeral, (LPVOID)NULL , 0, NULL);
			break;
		case 2:
			buffercircular2(2, 153, 153, 153, 153, 153,ola,ola,ola);
			break;
		case 3:
			ler(mapeamento());
			break;
		case 4:
			buffercircular2(2, 153, 153, 153, 153, 153, ola2, ola2, ola2);
			break;
		case 5:
			return 0;
			break;
		default:
			break;
		}
		system("cls");
	}
	return 0;
}

int buffercircular() {
	TCHAR NomeMemoria[] = TEXT("Nome da Mem�ria Partilhada");
	TCHAR NomeSemaforoPodeEscrever[] = TEXT("Sem�foro Pode Escrever");
	TCHAR NomeSemaforoPodeLer[] = TEXT("Sem�foro Pode Ler");
	TCHAR NomeMutexIndice[] = TEXT("MutexEscritor");

	HANDLE PodeEscrever;
	HANDLE PodeLer;
	HANDLE hMemoria;
	HANDLE mutex;

	bufferinfo *shm;
	int pos;
	char init = 0;


	PodeEscrever = CreateSemaphore(NULL, 0, Buffers, NomeSemaforoPodeEscrever);

	PodeLer = CreateSemaphore(NULL, 0, Buffers, NomeSemaforoPodeLer);


	hMemoria = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(bufferinfo), NomeMemoria);


	mutex = CreateMutex(NULL, FALSE, NomeMutexIndice);

	if (PodeEscrever == NULL || PodeLer == NULL || hMemoria == NULL) {
		_tprintf(TEXT("[Erro]Cria��o de objectos do Windows(%d)\n"), GetLastError());
		return -1;
	}
	if (GetLastError() != ERROR_ALREADY_EXISTS) {

		init = 1;

	}

	shm = (bufferinfo*)MapViewOfFile(hMemoria, FILE_MAP_WRITE, 0, 0, sizeof(bufferinfo));

	if (shm == NULL) {
		_tprintf(TEXT("[Erro]Mapeamento da mem�ria partilhada(%d)\n"), GetLastError());
		return -1;
	}

	if (init) {

		shm->iEscrita = 0;
		shm->iLeitura = 0;
		ReleaseSemaphore(PodeEscrever, 10, NULL);
	}
	for (int i = 0; i < 1; i++)
	{
		WaitForSingleObject(PodeEscrever, INFINITE);

		//METER O MUTEX PARA PROTEGER A ESCRITA -> PODE HAVER MA INFORMACAO PARA OS CLIENTES QUANDO SE LIBERTA O SEMAFORO 
		//DE LEITURA. ESCREVE COM INDICE > primeiro...
		WaitForSingleObject(mutex, INFINITE);
		//ler IN par aa var local POS
		pos = shm->iEscrita;
		shm->iEscrita = (shm->iEscrita + 1) % Buffers;
		//Incrementar valor de IN


		shm->dados[pos].tipo = 1;
		shm->dados[pos].aux1 = 5;
		shm->dados[pos].aux2 = 6;
		shm->dados[pos].aux3 = 7;
		shm->dados[pos].aux4 = 8;
		shm->dados[pos].aux5 = 9;
		//_stprintf_s(shm->buff[pos], BufferSize, TEXT("Pedido %d#%02d"), GetCurrentProcessId(), i);
		//_tprintf(TEXT("Escrever para buffer %d o valor %d \n"), pos, shm->iEscrita);
		_tprintf(TEXT("Escrever para buffer\n"));

		ReleaseMutex(mutex);

		Sleep(1000);
		ReleaseSemaphore(PodeLer, 1, NULL);
	}

	UnmapViewOfFile(shm);
	CloseHandle(PodeEscrever);
	CloseHandle(PodeLer);
	CloseHandle(hMemoria);
	CloseHandle(mutex);
	return 0;
}

int buffercircular2(int tipo, int aux1, int aux2, int aux3, int aux4, int aux5, char *aux6, char *aux7, char *aux8) {
	TCHAR NomeMemoria[] = TEXT("Nome da Mem�ria Partilhada");
	TCHAR NomeSemaforoPodeEscrever[] = TEXT("Sem�foro Pode Escrever");
	TCHAR NomeSemaforoPodeLer[] = TEXT("Sem�foro Pode Ler");
	TCHAR NomeMutexIndice[] = TEXT("MutexEscritor");

	HANDLE PodeEscrever;
	HANDLE PodeLer;
	HANDLE hMemoria;
	HANDLE mutex;

	bufferinfo *shm;
	int pos;
	char init = 0;


	PodeEscrever = CreateSemaphore(NULL, 0, Buffers, NomeSemaforoPodeEscrever);

	PodeLer = CreateSemaphore(NULL, 0, Buffers, NomeSemaforoPodeLer);


	hMemoria = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(bufferinfo), NomeMemoria);


	mutex = CreateMutex(NULL, FALSE, NomeMutexIndice);

	if (PodeEscrever == NULL || PodeLer == NULL || hMemoria == NULL) {
		_tprintf(TEXT("[Erro]Cria��o de objectos do Windows(%d)\n"), GetLastError());
		return -1;
	}
	if (GetLastError() != ERROR_ALREADY_EXISTS) {

		init = 1;

	}

	shm = (bufferinfo*)MapViewOfFile(hMemoria, FILE_MAP_WRITE, 0, 0, sizeof(bufferinfo));

	if (shm == NULL) {
		_tprintf(TEXT("[Erro]Mapeamento da mem�ria partilhada(%d)\n"), GetLastError());
		return -1;
	}

	if (init) {

		shm->iEscrita = 0;
		shm->iLeitura = 0;
		ReleaseSemaphore(PodeEscrever, 10, NULL);
	}
	for (int i = 0; i < 1; i++)
	{
		WaitForSingleObject(PodeEscrever, INFINITE);

		//METER O MUTEX PARA PROTEGER A ESCRITA -> PODE HAVER MA INFORMACAO PARA OS CLIENTES QUANDO SE LIBERTA O SEMAFORO 
		//DE LEITURA. ESCREVE COM INDICE > primeiro...
		WaitForSingleObject(mutex, INFINITE);
		//ler IN par aa var local POS
		pos = shm->iEscrita;
		shm->iEscrita = (shm->iEscrita + 1) % Buffers;
		//Incrementar valor de IN


		shm->dados[pos].tipo = tipo;
		shm->dados[pos].aux1 = aux1;
		shm->dados[pos].aux2 = aux2;
		shm->dados[pos].aux3 = aux3;
		shm->dados[pos].aux4 = aux4;
		shm->dados[pos].aux5 = aux5;
		strcpy_s(shm->dados[pos].aux6, aux6);
		strcpy_s(shm->dados[pos].aux7, aux7);
		strcpy_s(shm->dados[pos].aux8, aux8);
		//_stprintf_s(shm->buff[pos], BufferSize, TEXT("Pedido %d#%02d"), GetCurrentProcessId(), i);
		//_tprintf(TEXT("Escrever para buffer %d o valor %d \n"), pos, shm->iEscrita);
		_tprintf(TEXT("Escrever MSG para buffer circular\n"));

		ReleaseMutex(mutex);

		//Sleep(1000);
		ReleaseSemaphore(PodeLer, 1, NULL);
	}

	UnmapViewOfFile(shm);
	CloseHandle(PodeEscrever);
	CloseHandle(PodeLer);
	CloseHandle(hMemoria);
	CloseHandle(mutex);
	return 0;
}

void ler(obj *objectos) {
	int i;
	for (i = 0;objectos[i].id != NULL;i++) {
		printf("	id: %d\n	tipo: %d\n	x: %d\n	y: %d\n	Largura: %d\n	Altura: %d\n\n	********************************\n", objectos[i].id, objectos[i].tipo, objectos[i].x, objectos[i].y, objectos[i].tamx, objectos[i].tamy);
	}
	system("pause");
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

void pipalhadas() {
	HANDLE hPipeGeral;
	hPipeGeral = CreateNamedPipe(PIPE_NAME , PIPE_ACCESS_INBOUND, PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, PIPE_UNLIMITED_INSTANCES , sizeof(msg), sizeof(msg), 1000, NULL);
}

DWORD WINAPI RecebeClientesPipeGeral(LPVOID param) {
	Sleep(100);
	HANDLE hPipeGeral;
	HANDLE semaforo1 = CreateSemaphore(NULL, 1, 1, TEXT("semaforo1"));
	int i = 0;
	while (1) {
		hPipeGeral = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_INBOUND, PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, PIPE_UNLIMITED_INSTANCES, sizeof(msg), sizeof(msg), 1000, NULL);
		if (hPipeGeral == NULL) {
			printf_s("ERRO no pipe geral(CreateNamedPipe)");
		}
		printf_s("Esperar liga�ao  (ConnectNamedPipe) .....\n");
		if (!ConnectNamedPipe(hPipeGeral, NULL)) {
			printf_s("EROO Liga�ao ao cliente! (ConnectNamedPipe)");
		}
		ArrayHandles[i] = hPipeGeral;
		WaitForSingleObject(semaforo1, INFINITE);
		CreateThread(NULL, 0, TrataClientes, (LPVOID)&i, 0, NULL);
		i++;
	}
	return 0;
}

DWORD WINAPI TrataClientes(LPVOID param) {
	HANDLE semaforo1 = CreateSemaphore(NULL, 1, 1, TEXT("semaforo1"));
	int *e;
	int i;
	e = (int *)param;
	i = *e;
	ReleaseSemaphore(semaforo1, 1, NULL);
	printf_s("A tratar Cliente Numero:%d\n",i);
	return 0;
}