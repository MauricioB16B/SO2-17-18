//Escritor.c
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#define BufferSize 100
#define Buffers 10
TCHAR NomeMemoria[] = TEXT("Nome da Memória Partilhada");
TCHAR NomeSemaforoPodeEscrever[] = TEXT("Semáforo Pode Escrever");
TCHAR NomeSemaforoPodeLer[] = TEXT("Semáforo Pode Ler");
TCHAR NomeMutexIndice[] = TEXT("MutexEscritor");

HANDLE PodeEscrever;
HANDLE PodeLer;
HANDLE hMemoria;
HANDLE mutex;

typedef struct {
	
	TCHAR buff[Buffers][BufferSize];
	int iEscrita;
	int iLeitura;

}bufferinfo;

int _tmain(void)
{
	bufferinfo *shm;
	int pos;
	char init=0;


	PodeEscrever = CreateSemaphore(NULL, 0, Buffers, NomeSemaforoPodeEscrever);

	PodeLer = CreateSemaphore(NULL, 0, Buffers, NomeSemaforoPodeLer);


	hMemoria = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0
		, sizeof(bufferinfo), NomeMemoria);


	mutex = CreateMutex(NULL, FALSE, NomeMutexIndice);
	
	if (PodeEscrever == NULL || PodeLer == NULL || hMemoria == NULL) {
		_tprintf(TEXT("[Erro]Criação de objectos do Windows(%d)\n"), GetLastError());
		return -1;
	}
	if (GetLastError() != ERROR_ALREADY_EXISTS) {

		init = 1;

	}
	
	shm = (bufferinfo*)MapViewOfFile(hMemoria
		, FILE_MAP_WRITE, 0, 0, sizeof(bufferinfo));

	if (shm == NULL) {
		_tprintf(TEXT("[Erro]Mapeamento da memória partilhada(%d)\n"), GetLastError());
		return -1;
	}
	
	if (init) {

		shm->iEscrita = 0;
		shm->iLeitura = 0;
		ReleaseSemaphore(PodeEscrever, 10, NULL);
	}
	for (int i = 0; i < 100; i++)
	{
		WaitForSingleObject(PodeEscrever, INFINITE);

		//METER O MUTEX PARA PROTEGER A ESCRITA -> PODE HAVER MA INFORMACAO PARA OS CLIENTES QUANDO SE LIBERTA O SEMAFORO 
		//DE LEITURA. ESCREVE COM INDICE > primeiro...
		WaitForSingleObject(mutex, INFINITE);
		//ler IN par aa var local POS
		pos = shm->iEscrita;
		shm->iEscrita =(shm->iEscrita +1) %Buffers;
		//Incrementar valor de IN
		

		_stprintf_s(shm->buff[pos], BufferSize, TEXT("Pedido %d#%02d"), GetCurrentProcessId(), i);
		_tprintf(TEXT("Escrever para buffer %d o valor %d \n"), pos, shm->iEscrita);
		
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