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

typedef struct {

	HANDLE pipe = INVALID_HANDLE_VALUE;

}pipeBroadcast;

pipeBroadcast arrayHandles[10];

int _tmain(int argc, LPTSTR argv[]) {

	DWORD n;
	int i;
	HANDLE T1, hPipe;



#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif

	//Thread1

	T1 = CreateThread(NULL, 0, RecebeCliente, NULL, init, NULL);
	if (T1 == NULL)
		_tprintf(TEXT("Erro a lan�ar Thread que trata Pipes \n"));



	do {
		_tprintf(TEXT("[ESCRITOR] Frase: "));
		_fgetts(buf, 256, stdin);
		buf[_tcslen(buf) - 1] = '\0';

		for (i = 0;i < 10;i++) {
			if (arrayHandles[i].pipe != INVALID_HANDLE_VALUE) {

				if (!WriteFile(arrayHandles[i].pipe, buf, _tcslen(buf) * sizeof(TCHAR), &n, NULL)) {
					_tprintf(TEXT("[ERRO] Escrever no pipe! (WriteFile)\n"));
					exit(-1);
				}
			}
			_tprintf(TEXT("[ESCRITOR] Enviei %d bytes ao leitor...(WriteFile)\n"), n);

		}
	} while (_tcscmp(buf, TEXT("fim")));


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

		_tprintf(TEXT("[ESCRITOR] Criar uma c�pia do pipe '%s' ... (CreateNamedPipe)\n"),
			PIPE_NAME);

		hPipe = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_OUTBOUND, PIPE_WAIT |
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, 10, sizeof(buf), sizeof(buf), 1000, NULL);

		if (hPipe == INVALID_HANDLE_VALUE) {
			_tprintf(TEXT("[ERRO] Criar Named Pipe! (CreateNamedPipe)"));
			exit(-1);
		}


		_tprintf(TEXT("[ESCRITOR] Esperar liga��o de um leitor...(ConnectNamedPipe)\n"));
		if (!ConnectNamedPipe(hPipe, NULL)) {
			_tprintf(TEXT("[ERRO] Liga��o ao leitor! (ConnectNamedPipe\n"));
			exit(-1);
		}
		arrayHandles[i].pipe = hPipe;
		i++;







	}
	return 0;

}