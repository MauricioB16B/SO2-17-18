
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>


#define PIPE_NAME TEXT("\\\\.\\pipe\\teste")

char init = 1;
TCHAR buf[256];



DWORD WINAPI RecebeCliente(LPVOID param);
DWORD WINAPI TrataCliente(LPVOID par);

typedef struct {

	HANDLE pipe = INVALID_HANDLE_VALUE;

}pipeBroadcast;

pipeBroadcast arrayHandles[10];

int _tmain(int argc, LPTSTR argv[]) {

	
	int i;
	HANDLE T1,hPipe, IOReady;
	DWORD n;
	OVERLAPPED Ov;
	BOOL ret;



	IOReady = CreateEvent(NULL, TRUE, FALSE, NULL);

	

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif

	

	T1 = CreateThread(NULL, 0, RecebeCliente, NULL, init, NULL);
	if (T1 == NULL)
		_tprintf(TEXT("Erro a lançar Thread que trata Pipes \n"));


	
	do {
		_tprintf(TEXT("[ESCRITOR] Frase: "));
		_fgetts(buf, 256, stdin);
		buf[_tcslen(buf) - 1] = '\0';

		for (i = 0;i < 10;i++) {
			if (arrayHandles[i].pipe != INVALID_HANDLE_VALUE) {

				ZeroMemory(&Ov, sizeof(Ov));
				ResetEvent(IOReady);
				Ov.hEvent = IOReady;

				WriteFile(arrayHandles[i].pipe, buf, _tcslen(buf) * sizeof(TCHAR), &n, &Ov);
				WaitForSingleObject(IOReady,INFINITE);
				ret = GetOverlappedResult(arrayHandles[i].pipe, &Ov, &n, FALSE);
				if (!ret || !n) {
					_tprintf(TEXT("Deu erro"));
					exit(-1);
				}
				
			}
			

		}
	} while (_tcscmp(buf, TEXT("fim")));
	CloseHandle(IOReady);
	

		init = 0;
		hPipe = CreateFile(PIPE_NAME, GENERIC_READ, 0, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, NULL);
		WaitForSingleObject(T1, INFINITE);

		for (i = 0;i < 10;i++) {
			if (arrayHandles[i].pipe != INVALID_HANDLE_VALUE) {
				_tprintf(TEXT("[ESCRITOR] Desligar o pipe (DisconnectNamedPipe)\n"));

				if (!DisconnectNamedPipe(arrayHandles[i].pipe)) {
					_tprintf(TEXT("[ERRO] Desligar o pipe! (DisconnectNamedPipe)"));
					CloseHandle(arrayHandles[i].pipe);
				}
			}
		}
		exit(0);
	}

	
DWORD WINAPI RecebeCliente(LPVOID param) {

	HANDLE hPipe;
	int i = 0;



	while (init) {

		_tprintf(TEXT("[ESCRITOR] Criar uma cópia do pipe '%s' ... (CreateNamedPipe)\n"),
			PIPE_NAME);

		hPipe = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, PIPE_WAIT |
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, 10, sizeof(buf), sizeof(buf), 1000, NULL);

		if (hPipe == INVALID_HANDLE_VALUE) {
			_tprintf(TEXT("[ERRO] Criar Named Pipe! (CreateNamedPipe)"));
			exit(-1);
		}


		_tprintf(TEXT("[ESCRITOR] Esperar ligação de um leitor...(ConnectNamedPipe)\n"));
		if (!ConnectNamedPipe(hPipe, NULL)) {
			_tprintf(TEXT("[ERRO] Ligação ao leitor! (ConnectNamedPipe\n"));
			exit(-1);
		}
		//criar um mutex para aceder a tabela de handles
		arrayHandles[i].pipe = hPipe;
		i++;

		// LANÇAR THREAD PARA TRATAR CLIENTE (FAZER- GUARDAR HANDLES)
		CreateThread(NULL, 0, TrataCliente, (LPVOID) hPipe, init, NULL);
		


	}
	return 0;

}


DWORD WINAPI TrataCliente(LPVOID par) {

	HANDLE hPipe = (HANDLE)par;
	HANDLE IOReady;
	DWORD n;
	BOOL ret;

	OVERLAPPED Ov;



	IOReady = CreateEvent(NULL, TRUE, FALSE, NULL);

	while (1) {
		ZeroMemory(&Ov, sizeof(Ov));
		ResetEvent(IOReady);
		Ov.hEvent = IOReady;

		ReadFile(hPipe, buf, sizeof(buf), &n, &Ov);
		WaitForSingleObject(IOReady, INFINITE);
		ret = GetOverlappedResult(hPipe, &Ov, &n, FALSE);
		if (!ret || !n) {
			_tprintf(TEXT("Deu erro"));
			exit(-1);
		}
		_tprintf(TEXT("Recebi %d bytes : '%s' (Read file) \n"), n, buf);


		CloseHandle(IOReady);
		return 0;
	}
}