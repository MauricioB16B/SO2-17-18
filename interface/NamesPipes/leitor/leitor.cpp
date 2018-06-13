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


DWORD WINAPI EscreveParaGateway(LPVOID param);

int _tmain(int argc, LPTSTR argv[]) {

	TCHAR buf[256];
	HANDLE hPipe, T1, IOReady;
	int i = 0;
	DWORD n;
	OVERLAPPED Ov;
	BOOL ret;
	


	IOReady = CreateEvent(NULL, TRUE, FALSE, NULL);

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif
	
	


	


	_tprintf(TEXT("[LEITOR] Esperar pelo pipe '%s' (WaitNamedPipe)\n"), PIPE_NAME);
	
	if (!WaitNamedPipe(PIPE_NAME, NMPWAIT_WAIT_FOREVER)) {
		_tprintf(TEXT("[ERRO] Ligar ao pipe '%s'! (WaitNamedPipe)\n"), PIPE_NAME);
		exit(-1);
	}
	_tprintf(TEXT("[LEITOR] Ligação ao pipe do escritor... (CreateFile)\n"));
	hPipe = CreateFile(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
		0|FILE_FLAG_OVERLAPPED, NULL);
	
	if (hPipe == NULL) {
		_tprintf(TEXT("[ERRO] Ligar ao pipe '%s'! (CreateFile)\n"), PIPE_NAME);
		exit(-1);
	}
	_tprintf(TEXT("[LEITOR] Liguei-me...\n"));
	
	//alterar modo leitura pipe para "message"
	DWORD modo = PIPE_READMODE_MESSAGE;
	SetNamedPipeHandleState(hPipe, &modo, NULL, NULL);

	// Para enviar pedidos ao Gateway

	T1 = CreateThread(NULL, 0, EscreveParaGateway, (LPVOID) hPipe, 0, NULL);
	if (T1 == NULL)
		_tprintf(TEXT("Erro a lançar Thread que trata Pipes \n"));

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

		buf[n / sizeof(TCHAR)] = TEXT('\0');
		_tprintf(TEXT("[LEITOR] Recebi %d bytes: '%s'... (ReadFile)\n"), n, buf);
	}


	CloseHandle(IOReady);
	

	WaitForSingleObject(T1, INFINITE);
	CloseHandle(T1);
	CloseHandle(hPipe);
	return 0;
}


DWORD WINAPI EscreveParaGateway(LPVOID par) {

	HANDLE hPipe = (HANDLE) par;
	BOOL ret;
	DWORD n;

	OVERLAPPED Ov;
	HANDLE IOReady;

	IOReady = CreateEvent(NULL, TRUE, FALSE, NULL);



	do {
		
		//LER DO TECLADO
		_tprintf(TEXT("[CLIENTE] Frase: "));
		_fgetts(buf, 256, stdin);
		buf[_tcslen(buf) - 1] = '\0';


		//ENVIAR PEDIDOS AO GATEWAY
		ZeroMemory(&Ov, sizeof(Ov));
		ResetEvent(IOReady);
		Ov.hEvent = IOReady;


		WriteFile(hPipe, buf, _tcslen(buf) * sizeof(TCHAR), &n, &Ov);

		WaitForSingleObject(IOReady, INFINITE);

		ret = GetOverlappedResult(hPipe, &Ov, &n, FALSE);
		if (!ret || !n) {
			_tprintf(TEXT("Deu erro"));
		exit(-1);
		}
		
		_tprintf(TEXT("Enviei %d bytes : '%s' (Write file) \n"), n, buf);
			
	} while (_tcscmp(buf, TEXT("fim")));
	

	CloseHandle(IOReady);

	return 0;

}