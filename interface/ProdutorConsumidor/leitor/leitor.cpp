//Leitor.c
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#define BufferSize 100
#define Buffers 10
TCHAR NomeMemoria[] = TEXT("Nome da Memória Partilhada");
TCHAR NomeSemaforoPodeEscrever[] = TEXT("Semáforo Pode Escrever");
TCHAR NomeSemaforoPodeLer[] = TEXT("Semáforo Pode Ler");

TCHAR NomeMutexIndice[] = TEXT("MutexLeitor");

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


	PodeEscrever = CreateSemaphore(NULL, Buffers, Buffers, NomeSemaforoPodeEscrever);
	PodeLer = CreateSemaphore(NULL, 0, Buffers, NomeSemaforoPodeLer);
	hMemoria = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0
		, sizeof(TCHAR[Buffers][BufferSize]), NomeMemoria);

	mutex = CreateMutex(NULL, FALSE, NomeMutexIndice);

	if (PodeEscrever == NULL || PodeLer == NULL || hMemoria == NULL) {
		_tprintf(TEXT("[Erro]Criação de objectos do Windows(%d)\n"), GetLastError());
		return -1;
	}
	
	shm = (bufferinfo*)MapViewOfFile(hMemoria
		, FILE_MAP_WRITE, 0, 0, sizeof(bufferinfo));
	if (shm == NULL) {
		_tprintf(TEXT("[Erro]Mapeamento da memória partilhada(%d)\n"), GetLastError());
		return -1;
	}
	
	for (int i = 0;; ++i)
	{
		WaitForSingleObject(PodeLer, INFINITE);

		WaitForSingleObject(mutex, INFINITE);
		//ler IN par aa var local POS
		pos = shm->iLeitura;
		shm->iLeitura= (shm->iLeitura + 1) % Buffers;
		//Incrementar valor de IN
		ReleaseMutex(mutex);


		_tprintf(TEXT("Estou a ler do buffer %d o valor '%s' \n "), pos, shm->buff[pos]); // Reader reads data
		ReleaseSemaphore(PodeEscrever, 1, NULL);
	}

	UnmapViewOfFile(shm);
	CloseHandle(PodeEscrever);
	CloseHandle(PodeLer);
	CloseHandle(hMemoria);
	CloseHandle(mutex);
	return 0;
}