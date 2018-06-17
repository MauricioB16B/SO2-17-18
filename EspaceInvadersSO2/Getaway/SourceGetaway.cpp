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
	TCHAR aux6[1024];
	TCHAR aux7[1024];
	TCHAR aux8[1024];
}msg;
typedef struct {

	msg dados[10];
	int iEscrita;
	int iLeitura;

}bufferinfo;

#define PIPE_NAME TEXT("\\\\.\\pipe\\main")
#define Buffers 10

HANDLE arrayhandles[30];

DWORD WINAPI thread1(LPVOID param); // thread Recebe cliente
DWORD WINAPI thread2(LPVOID param); // thread trata cliente 1 (Cliente --> Servidor)
DWORD WINAPI thread3(LPVOID param); // thread trata cliente 2 (Servidor --> Cliente)
HANDLE abreEvento(TCHAR string[1024]);
int buffercircular2(msg dados);
obj * mapeamento();

int _tmain() {
	obj * mapa;
	obj FimTrasmissao;
	HANDLE mapUpdate;
	int i,ih;

	CreateThread(NULL, 0, thread1, (LPVOID)NULL, 0, NULL);
	
	mapUpdate = abreEvento(TEXT("MapUpdate"));
	if (mapUpdate == NULL)
		return 0;
	mapa = mapeamento();
	while (true){
		FimTrasmissao.id = 5000;
		WaitForSingleObject(mapUpdate,INFINITE);
		wprintf_s(L"Vou enviar mapa para o cliente\n");
		for (ih = 0;ih < 30;ih++) {
			if (arrayhandles[ih] != NULL) {
				for (i = 0;i < 300;i++) {
					if (mapa[i].id != NULL) {
						if (!WriteFile(arrayhandles[ih], &mapa[i], sizeof(obj), NULL, NULL)) {
							wprintf_s(L"ERRO na escrita do pipe OBJ\n");
						}
					}
				}
				WriteFile(arrayhandles[ih], &FimTrasmissao, sizeof(obj), NULL, NULL);
			}
		}
	}
	return 0;
}

DWORD WINAPI thread1(LPVOID param) {
	HANDLE handleMain;
	HANDLE semaforo1 = CreateSemaphore(NULL, 1, 1, TEXT("semaforo1"));
	while (true) {

		handleMain = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_INBOUND, PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, PIPE_UNLIMITED_INSTANCES, sizeof(msg), sizeof(msg), 1000, NULL);
		if (handleMain == NULL)
			wprintf_s(L"Erro na criaçao do PipeMain");

		if (!ConnectNamedPipe(handleMain, NULL)) {
			printf_s("EROO Ligaçao ao cliente! (ConnectNamedPipe)");
		}

		WaitForSingleObject(semaforo1, INFINITE);
		CreateThread(NULL, 0, thread2, (LPVOID)&handleMain, 0, NULL);
		WaitForSingleObject(semaforo1, INFINITE);
		ReleaseSemaphore(semaforo1, 1, NULL);

	}
	return 0;
}

DWORD WINAPI thread2(LPVOID param) {
	HANDLE semaforo1 = CreateSemaphore(NULL, 1, 1, TEXT("semaforo1"));
	HANDLE * phandle;
	HANDLE hRead;
	phandle = (HANDLE *)param;
	hRead = *phandle;
	ReleaseSemaphore(semaforo1, 1, NULL);
	msg data;
	HANDLE hWrite,hObj, mutexarrayhandles = CreateMutex(NULL, FALSE, TEXT("mutexarray"));
	TCHAR string1[1024];
	int i;


	ReadFile(hRead, &data, sizeof(msg), NULL, NULL);

	if (!WaitNamedPipe(data.aux8, NMPWAIT_WAIT_FOREVER)) {
		wprintf_s(TEXT("Pipe MSG do cliente nao encontrado.        A terminar thread!\n"));
		return 0;
	}

	hWrite = CreateFile(data.aux8, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hWrite == NULL) {
		wprintf_s(TEXT("ERROR! (hWrite) (CreateFile).              A terminar thread"));
		return 0;
	}
	//  hWrite pronto para usar
	
	Sleep(100);// eu sei que nao se deve usar isto mas é provisorio so para garantir que isto corre bem

	swprintf_s(string1, L"%sobj", data.aux8);
	if (!WaitNamedPipe(string1, NMPWAIT_WAIT_FOREVER)) {
		wprintf_s(TEXT("Pipe OBJ do cliente nao encontrado.        A terminar thread"));
		return 0;
	}

	hObj = CreateFile(string1, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hObj == NULL) {
		wprintf_s(TEXT("ERROR! (hObj) (CreateFile).               A terminar thread"));
		return 0;
	}

	WaitForSingleObject(mutexarrayhandles, INFINITE);

	for (i = 0;i<30;i++) {
		if (arrayhandles[i] == NULL) {
			arrayhandles[i] = hObj;
			i = 35;
		}
	}
	ReleaseMutex(mutexarrayhandles);
	if (i != 36) {
		wprintf_s(TEXT("ERROR! (ArrayHandles).               A terminar thread"));
		return 0;
	}

	CreateThread(NULL, 0, thread3, (LPVOID)&hWrite, 0, NULL);// no need semaforo! so é lancada uma thread destas por cliente

	wprintf_s(TEXT("Cliente \"%s\" Sem erros! tudo OK!!!\n"), data.aux8);

	while (true){
		
		ReadFile(hRead, &data, sizeof(msg), NULL, NULL);
		
		if (data.aux1 == 33) {// aux1 == 33 é mensagem para testar comunicaçao com o getaway

		}
		else {
			buffercircular2(data);
		}

	}

	return 0;
}

DWORD WINAPI thread3(LPVOID param) {
	HANDLE * phWrite;
	HANDLE hWrite;
	phWrite = (HANDLE*)param;
	hWrite = *phWrite;

	//le do buffer circular2 e descarrega a msg para o pipe MSG getaway -> cliente
	
	return 0;
}

int buffercircular2(msg dados) {
	TCHAR NomeMemoria[] = TEXT("Nome da Memória Partilhada");
	TCHAR NomeSemaforoPodeEscrever[] = TEXT("Semáforo Pode Escrever");
	TCHAR NomeSemaforoPodeLer[] = TEXT("Semáforo Pode Ler");
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
		_tprintf(TEXT("[Erro]Criação de objectos do Windows(%d)\n"), GetLastError());
		return -1;
	}
	if (GetLastError() != ERROR_ALREADY_EXISTS) {

		init = 1;

	}

	shm = (bufferinfo*)MapViewOfFile(hMemoria, FILE_MAP_WRITE, 0, 0, sizeof(bufferinfo));
	if (shm == NULL) {
		_tprintf(TEXT("[Erro]Mapeamento da memória partilhada(%d)\n"), GetLastError());
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
		
		shm->dados[pos] = dados;
		
		//_stprintf_s(shm->buff[pos], BufferSize, TEXT("Pedido %d#%02d"), GetCurrentProcessId(), i);
		//_tprintf(TEXT("Escrever para buffer %d o valor %d \n"), pos, shm->iEscrita);
		_tprintf(TEXT("MSG roteada e enviada\n"));

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

obj * mapeamento() {
	TCHAR syNome[] = TEXT("Global\\GlobalGameData");
	HANDLE partilha;
	obj *ponteiro;

	partilha = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(obj) * 300, syNome);

	if (partilha == NULL) {
		_tprintf(TEXT("Nao foi possifel criar o mapeamento no systema, problemas de permissao ERRO ""5"" (%d).\n"), GetLastError());
		return NULL;
	}

	ponteiro = (obj *)MapViewOfFile(partilha, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(obj) * 300);

	if (ponteiro == NULL)
		_tprintf(TEXT("Nao foi possivel fazer o mapeamento do vector no espaco mapeado ERRO (%d)\n"), GetLastError());

	return ponteiro;
}

HANDLE abreEvento(TCHAR string[1024]) {
	int i=0;
	HANDLE hand;
	do {
		hand = OpenEvent(EVENT_ALL_ACCESS, FALSE, string);
		if (hand == NULL) {
			Sleep(5);
		}
		if (i==3000) {//depois de mais ou menos 5 segundoas a tentar abrir o evento (see sleep(5))
			wprintf_s(L"Falha Catastrofica a abrir EVENTO\n");
			return NULL;
		}
		i++;
	} while (hand == NULL);

	return hand;
}
