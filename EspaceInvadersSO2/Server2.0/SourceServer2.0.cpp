#include <windows.h>  
#include <stdlib.h>  
#include <string.h>  
#include <tchar.h>  
#include "resource.h"
#include <Commctrl.h>
#include <Windowsx.h>

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
	TCHAR aux6[1024];
	TCHAR aux7[1024];
	TCHAR aux8[1024];
}msg;
typedef struct {

	msg dados[10];
	int iEscrita;
	int iLeitura;

}bufferinfo;
typedef struct {
	int tamx;
	int tamy;
	int tipo;
	TCHAR bitmap[1024];
}tipo;
typedef struct definicoes {
	int maxx;
	int maxy;
	int nnaves;
	int Tdireita;
	int Tesquerda;
	int Tdisparo;
	int Tdireita2;
	int Tesquerda2;
	int Tdisparo2;
	TCHAR jogador1[1024];
	TCHAR jogador2[1024];
	int pid1;
	int pid2;
	int pontos1;
	int pontos2;
	tipo naveg;
	tipo navep;
	tipo tiro;
	tipo bomba;
	tipo tjogador1;
	tipo tjogador2;
	tipo power1;
	tipo power2;
	tipo power3;
	tipo power4;
	tipo power5;
	tipo power6;
	tipo power7;
	tipo power8;
	int folgax;
	int folgay;
	int folgahor;
	int folgaver;
}def;

static TCHAR szWindowClass[] = _T("win32app");
static TCHAR szTitle[] = _T("Server");

HINSTANCE hInst;
def definicoes;
int objCounterID,nnaves,nnavesprontas,direcao,descida,level=1;
HANDLE mapUpdate, clock, mutex, clock2, jalitudo;

int tratamsg(msg data, obj * objectos);
int crianave(obj objecto,int indice, obj * objectos);
int criamapa(obj * objectos);
int criamapa1p();
int criamapa2p();
int criajogador1(obj *objectos);
int criajogador2(obj *objectos);
void SendDefinitions(int pid);
obj * mapeamento();
int CriaNovoJogo(msg data);
int tiro(msg data, obj * objectos);
int move(msg data, obj * objectos);
int buffercircular();
int buffercircular2(msg dados);
int apanhatecla();
DWORD WINAPI thread1(LPVOID param);
DWORD WINAPI thread2(LPVOID param);
DWORD WINAPI thread3(LPVOID param);
DWORD WINAPI thread4(LPVOID param);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK Dialog1Proc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

int CALLBACK WinMain(_In_ HINSTANCE hInstance,_In_ HINSTANCE hPrevInstance,_In_ LPSTR lpCmdLine, _In_ int nCmdShow){
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	if (!RegisterClassEx(&wcex)){
		MessageBox(NULL,_T("Call to RegisterClassEx failed!"),_T("Win32 Guided Tour"),NULL);
		return 1;
	}

	hInst = hInstance; // Store instance handle in our global variable  

					   // The parameters to CreateWindow explained:  
					   // szWindowClass: the name of the application  
					   // szTitle: the text that appears in the title bar  
					   // WS_OVERLAPPEDWINDOW: the type of window to create  
					   // CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)  
					   // 500, 100: initial size (width, length)  
					   // NULL: the parent of this window  
					   // NULL: this application does not have a menu bar  
					   // hInstance: the first parameter from WinMain  
					   // NULL: not used in this application  
	HWND hWnd = CreateWindow(szWindowClass,szTitle,WS_OVERLAPPEDWINDOW,CW_USEDEFAULT, CW_USEDEFAULT, 500, 500,NULL,NULL,hInstance,NULL);

	if (!hWnd){
		MessageBox(NULL,_T("Call to CreateWindow failed!"),_T("Win32 Guided Tour"),NULL);
		return 1;
	}

	// The parameters to ShowWindow explained:  
	// hWnd: the value returned from CreateWindow  
	// nCmdShow: the fourth parameter from WinMain  
	ShowWindow(hWnd,nCmdShow);
	UpdateWindow(hWnd);

	// Main message loop:  
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message){
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_FILE_STARTSERVER:
			if (DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, (DLGPROC)Dialog1Proc) == 1) {
				CreateThread(NULL, 0, thread1, NULL, 0, NULL);
			}

			break;
		case ID_FILE_SAIR:
			break;
		default:
			break;
		}
		break;
	case WM_CREATE:
		mapUpdate = CreateEvent(NULL, FALSE, FALSE, TEXT("MapUpdate"));
		clock = CreateEvent(NULL, TRUE, FALSE, TEXT("clock"));
		clock2 = CreateEvent(NULL, TRUE, FALSE, TEXT("clock2"));
		mutex = CreateMutex(NULL, FALSE, L"mutexmapa");
		jalitudo = CreateSemaphore(NULL, 0, 1, L"PodeEscreverMapa");
		nnaves = 0;
		nnavesprontas = 0;
		direcao = 1;
		descida = 0;
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

BOOL CALLBACK Dialog1Proc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		TCHAR string1[1024];
	case WM_COMMAND:
		switch (LOWORD(wParam)){
		case IDC_CHECK2:
			if (HIWORD(wParam) == BN_CLICKED) {
				if (Button_GetCheck(GetDlgItem(hwndDlg, IDC_CHECK2)) == BST_CHECKED) 
					EnableWindow(GetDlgItem(hwndDlg, IDC_EDIT2), TRUE);
				if (Button_GetCheck(GetDlgItem(hwndDlg, IDC_CHECK2)) == BST_UNCHECKED) 
					EnableWindow(GetDlgItem(hwndDlg, IDC_EDIT2), FALSE);
			}
			break;
		case IDC_CHECK1:
			if (HIWORD(wParam) == BN_CLICKED) {
				if (Button_GetCheck(GetDlgItem(hwndDlg, IDC_CHECK1)) == BST_CHECKED)
					EnableWindow(GetDlgItem(hwndDlg, IDC_EDIT12), FALSE);
				if (Button_GetCheck(GetDlgItem(hwndDlg, IDC_CHECK1)) == BST_UNCHECKED)
					EnableWindow(GetDlgItem(hwndDlg, IDC_EDIT12), TRUE);
			}	
			break;
		case IDCANCEL:
			EndDialog(hwndDlg, LOWORD(wParam));
			break;
		case IDC_BUTTON1:
			EnableWindow(GetDlgItem(hwndDlg, IDC_BUTTON1), FALSE);
			definicoes.Tesquerda = apanhatecla();
			swprintf_s(string1, L"%d", definicoes.Tesquerda);
			SetDlgItemText(hwndDlg, IDC_EDIT3, string1);
			EnableWindow(GetDlgItem(hwndDlg, IDC_BUTTON1), TRUE);
			break;
		case IDC_BUTTON2:
			EnableWindow(GetDlgItem(hwndDlg, IDC_BUTTON2), FALSE);
			definicoes.Tdireita= apanhatecla();
			swprintf_s(string1, L"%d", definicoes.Tdireita);
			SetDlgItemText(hwndDlg, IDC_EDIT4, string1);
			EnableWindow(GetDlgItem(hwndDlg, IDC_BUTTON2), TRUE);
			break;
		case IDC_BUTTON3:
			EnableWindow(GetDlgItem(hwndDlg, IDC_BUTTON3), FALSE);
			definicoes.Tdisparo = apanhatecla();
			swprintf_s(string1, L"%d", definicoes.Tdisparo);
			SetDlgItemText(hwndDlg, IDC_EDIT5, string1);
			EnableWindow(GetDlgItem(hwndDlg, IDC_BUTTON3), TRUE);
			break;
		case IDC_BUTTON4:
			EnableWindow(GetDlgItem(hwndDlg, IDC_BUTTON4), FALSE);
			definicoes.Tesquerda2 = apanhatecla();
			swprintf_s(string1, L"%d", definicoes.Tesquerda2);
			SetDlgItemText(hwndDlg, IDC_EDIT6, string1);
			EnableWindow(GetDlgItem(hwndDlg, IDC_BUTTON4), TRUE);
			break;
		case IDC_BUTTON5:
			EnableWindow(GetDlgItem(hwndDlg, IDC_BUTTON5), FALSE);
			definicoes.Tdireita2 = apanhatecla();
			swprintf_s(string1, L"%d", definicoes.Tdireita2);
			SetDlgItemText(hwndDlg, IDC_EDIT7, string1);
			EnableWindow(GetDlgItem(hwndDlg, IDC_BUTTON5), TRUE);
			break;
		case IDC_BUTTON6:
			EnableWindow(GetDlgItem(hwndDlg, IDC_BUTTON6), FALSE);
			definicoes.Tdisparo2 = apanhatecla();
			swprintf_s(string1, L"%d", definicoes.Tdisparo2);
			SetDlgItemText(hwndDlg, IDC_EDIT8, string1);
			EnableWindow(GetDlgItem(hwndDlg, IDC_BUTTON6), TRUE);
			break;
		case IDOK:
			if (Button_GetCheck(GetDlgItem(hwndDlg, IDC_CHECK2)) == BST_UNCHECKED){
				GetDlgItemText(hwndDlg, IDC_EDIT1, definicoes.jogador1, 1024);
				lstrcpyW(definicoes.jogador2,L"\0");
			}else{
				GetDlgItemText(hwndDlg, IDC_EDIT1, definicoes.jogador1, 1024);
				GetDlgItemText(hwndDlg, IDC_EDIT2, definicoes.jogador2, 1024);
			}
			if (Button_GetCheck(GetDlgItem(hwndDlg, IDC_CHECK1)) == BST_UNCHECKED) {
				GetDlgItemText(hwndDlg, IDC_EDIT12, string1, 1024);
				definicoes.nnaves = _wtoi(string1);
			}else{
				definicoes.nnaves = 40;// fazer calculo automatico com base o maxx e o maxy
			}
			
			definicoes.maxx = SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER1), (UINT)TBM_GETPOS, (WPARAM)0, (LPARAM)0);
			definicoes.maxy = SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER2), (UINT)TBM_GETPOS, (WPARAM)0, (LPARAM)0);
			
			definicoes.naveg.tamx = 65;
			definicoes.naveg.tamy = 40;
			definicoes.naveg.tipo = 1;
			swprintf_s(definicoes.naveg.bitmap, L"img\\colec\\naveg.bmp"); 

			definicoes.navep.tamx = 45;
			definicoes.navep.tamy = 30;
			definicoes.navep.tipo = 2;
			swprintf_s(definicoes.navep.bitmap, L"img\\colec\\navep.bmp");

			definicoes.tiro.tamx = 5;
			definicoes.tiro.tamy = 30;
			definicoes.tiro.tipo = 3;
			swprintf_s(definicoes.tiro.bitmap, L"img\\colec\\tiro.bmp");

			definicoes.bomba.tamx = 5;
			definicoes.bomba.tamy = 30;
			definicoes.bomba.tipo = 4;
			swprintf_s(definicoes.bomba.bitmap, L"img\\colec\\bomba.bmp");

			definicoes.tjogador1.tamx = 80;
			definicoes.tjogador1.tamy = 30;
			definicoes.tjogador1.tipo = 5;
			swprintf_s(definicoes.tjogador1.bitmap, L"img\\colec\\Player1.bmp");
			
			definicoes.tjogador2.tamx = 80;
			definicoes.tjogador2.tamy = 30;
			definicoes.tjogador2.tipo = 6;
			swprintf_s(definicoes.tjogador2.bitmap, L"img\\colec\\Player2.bmp");

			definicoes.power1.tamx = 30;
			definicoes.power1.tamy = 30;
			definicoes.power1.tipo = 7;
			swprintf_s(definicoes.power1.bitmap, L"img\\colec\\pwerup1.bmp");

			definicoes.power2.tamx = 30;
			definicoes.power2.tamy = 30;
			definicoes.power2.tipo = 8;
			swprintf_s(definicoes.power2.bitmap, L"img\\colec\\pwerup2.bmp");

			definicoes.power3.tamx = 30;
			definicoes.power3.tamy = 30;
			definicoes.power3.tipo = 9;
			swprintf_s(definicoes.power3.bitmap, L"img\\colec\\pwerup3.bmp");

			definicoes.power4.tamx = 30;
			definicoes.power4.tamy = 30;
			definicoes.power4.tipo = 10;
			swprintf_s(definicoes.power4.bitmap, L"img\\colec\\pwerup4.bmp");

			definicoes.power5.tamx = 30;
			definicoes.power5.tamy = 30;
			definicoes.power5.tipo = 11;
			swprintf_s(definicoes.power5.bitmap, L"img\\colec\\pwerup5.bmp");

			definicoes.power6.tamx = 30;
			definicoes.power6.tamy = 30;
			definicoes.power6.tipo = 12;
			swprintf_s(definicoes.power6.bitmap, L"img\\colec\\pwerup6.bmp");

			definicoes.power7.tamx = 30;
			definicoes.power7.tamy = 30;
			definicoes.power7.tipo = 13;
			swprintf_s(definicoes.power7.bitmap, L"img\\colec\\pwerup7.bmp");

			definicoes.power8.tamx = 30;
			definicoes.power8.tamy = 30;
			definicoes.power8.tipo = 14;
			swprintf_s(definicoes.power8.bitmap, L"img\\colec\\pwerup8.bmp");

			definicoes.folgahor=100;
			definicoes.folgaver=50;
			definicoes.folgay=20;
			definicoes.folgax = 20;

			swprintf_s(string1,L"x:%d\ny:%d\nNnaves:%d\nJogador1:%s\nJogador2:%s",definicoes.maxx,definicoes.maxy,definicoes.nnaves,definicoes.jogador1,definicoes.jogador2);
			MessageBox(NULL,string1, _T("Janela de testes!! "), NULL);
			EnableMenuItem(GetMenu(GetWindow(hwndDlg, GW_OWNER)), ID_FILE_STARTSERVER, MF_DISABLED);
			EndDialog(hwndDlg, 1);
			break;
		default:
			break;
		}
		break;

	case WM_HSCROLL:
		swprintf_s(string1,L"%d", SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER1), (UINT)TBM_GETPOS, (WPARAM)0, (LPARAM)0));
		SetDlgItemText(hwndDlg, IDC_EDIT13, string1);
		swprintf_s(string1, L"%d", SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER2), (UINT)TBM_GETPOS, (WPARAM)0, (LPARAM)0));
		SetDlgItemText(hwndDlg, IDC_EDIT14, string1);
		break;

	case WM_INITDIALOG:
		SendDlgItemMessage(hwndDlg, IDC_SLIDER1, TBM_SETRANGE, (WPARAM)1, (LPARAM)MAKELONG(500, 1920));
		SendDlgItemMessage(hwndDlg, IDC_SLIDER2, TBM_SETRANGE, (WPARAM)1, (LPARAM)MAKELONG(300, 1080));
		SendDlgItemMessage(hwndDlg, IDC_CHECK1, BM_SETCHECK, BST_CHECKED, 0);
		SetDlgItemText(hwndDlg, IDC_EDIT13, L"500");
		SetDlgItemText(hwndDlg, IDC_EDIT14, L"300");
		definicoes.Tdireita = 39;
		definicoes.Tesquerda = 37;
		definicoes.Tdisparo = 32;
		definicoes.Tdireita2 = 102;
		definicoes.Tesquerda2 = 100;
		definicoes.Tdisparo2 = 96;
		SetDlgItemText(hwndDlg, IDC_EDIT3, L" ");
		SetDlgItemText(hwndDlg, IDC_EDIT4, L" ");
		SetDlgItemText(hwndDlg, IDC_EDIT5, L" ");
		SetDlgItemText(hwndDlg, IDC_EDIT6, L" ");
		SetDlgItemText(hwndDlg, IDC_EDIT7, L" ");
		SetDlgItemText(hwndDlg, IDC_EDIT8, L" ");
		break;
	case WM_CLOSE:
		EndDialog(hwndDlg, LOWORD(wParam));
		break;
	}
	return FALSE;
}

int buffercircular() {
	obj * objectos = mapeamento();
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

			tratamsg(shm->dados[pos], objectos);// copia data do buffer para variavel da funcao e liberta o buffer
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

int buffercircular2(msg dados) {
	TCHAR NomeMemoria[] = TEXT("Nome da Memória Partilhada2");
	TCHAR NomeSemaforoPodeEscrever[] = TEXT("Semáforo Pode Escrever2");
	TCHAR NomeSemaforoPodeLer[] = TEXT("Semáforo Pode Ler2");
	TCHAR NomeMutexIndice[] = TEXT("MutexEscritor2");

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
		//_tprintf(TEXT("MSG roteada e enviada\n"));

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

int tratamsg(msg data, obj * objectos) {
	HANDLE semaforo1 = CreateSemaphore(NULL, 1, 1, TEXT("semaforo1"));
	WaitForSingleObject(semaforo1, INFINITE);
	//TCHAR string1[1024];
	//swprintf_s(string1,L"MSG:\n***	tipo:%d\n	aux1:%d\n	aux2:%d\n	aux3:%d\n	aux4:%d\n	aux5:%d\n	aux6:%s\n	aux7:%s\n	aux8:%s\n***\n", data.tipo, data.aux1, data.aux2, data.aux3, data.aux4, data.aux5, data.aux6, data.aux7, data.aux8);
	//MessageBox(NULL, string1, _T("Janela de testes!! "), NULL);
	switch (data.tipo) {
	case 1:
		CriaNovoJogo(data);
		break;
	case 2:
		move(data, objectos);
		break;
	case 3:
		tiro(data, objectos);
		break;
	case 4:
		ReleaseSemaphore(semaforo1, 1, NULL);
		break;
	case 5:
		ReleaseSemaphore(semaforo1, 1, NULL);
		break;
	case 6:
		ReleaseSemaphore(semaforo1, 1, NULL);
		break;
	default:
		ReleaseSemaphore(semaforo1, 1, NULL);
		break;
	}
	//WaitForSingleObject(semaforo1, INFINITE);
	ReleaseSemaphore(semaforo1, 1, NULL);
	return 0;
}

int tiro(msg data, obj * objectos) {
	HANDLE semaforoThTiro = CreateSemaphore(NULL, 1, 1, TEXT("semaforoThtiro"));
	msg infotiro;
	
	if (data.aux5 == definicoes.pid1) {
		infotiro.aux1 = objectos[0].x + (objectos[0].tamx/2);
		infotiro.aux2 = objectos[0].y - objectos[0].tamy;
		infotiro.aux3 = 1;

		WaitForSingleObject(semaforoThTiro, INFINITE);
		CreateThread(NULL, 0, thread4, (LPVOID)&infotiro, 0, NULL);
		WaitForSingleObject(semaforoThTiro, INFINITE);
		ReleaseSemaphore(semaforoThTiro, 1, NULL);
	}
	if (data.aux5 == definicoes.pid2) {
		infotiro.aux1 = objectos[1].x + (objectos[1].tamx / 2);
		infotiro.aux2 = objectos[1].y - objectos[1].tamy;
		infotiro.aux3 = 2;

		WaitForSingleObject(semaforoThTiro, INFINITE);
		CreateThread(NULL, 0, thread4, (LPVOID)&infotiro, 0, NULL);
		WaitForSingleObject(semaforoThTiro, INFINITE);
		ReleaseSemaphore(semaforoThTiro, 1, NULL);
	}
	return 0;
}

int move(msg data, obj *objectos) {
	
	if (data.aux5 == definicoes.pid1) {
		if (data.aux1 == 1)
			objectos[0].x = objectos[0].x + 10;
		if (data.aux1 == 2)
			objectos[0].x = objectos[0].x - 10;
		SetEvent(mapUpdate);
	}
	if (data.aux5 == definicoes.pid2) {
		if (data.aux1 == 1)
			objectos[1].x = objectos[1].x + 10;
		if (data.aux1 == 2)
			objectos[1].x = objectos[1].x - 10;
		SetEvent(mapUpdate);
	}
	return 0;
}

int CriaNovoJogo(msg data) {
	obj * objectos = mapeamento();
	msg reply;
	int indice = 0;
	reply.aux5 = data.aux5;

	if(wcscmp(definicoes.jogador1, data.aux6)==0){
		definicoes.pid1 = data.aux5;
		swprintf_s(reply.aux6, L"Ola %s, vais ser o jogador 1", data.aux6);
	}
	else if (wcscmp(definicoes.jogador2, data.aux6)==0){
		definicoes.pid2 = data.aux5;
		swprintf_s(reply.aux6, L"Ola %s, vais ser o jogador 2", data.aux6);
	}
	else{
		reply.aux1 = -1;
		swprintf_s(reply.aux6,L"O jogador %s nao foi encontrado!",data.aux6);
		buffercircular2(reply);
		return 0;
	}

	reply.aux1 = 1;
	reply.aux2 = definicoes.maxx;
	reply.aux3 = definicoes.maxy;
	buffercircular2(reply);

	Sleep(500);
	SendDefinitions(data.aux5);

	if (wcslen(definicoes.jogador2) >= 1) {
		if (definicoes.pid1 != NULL && definicoes.pid2 != NULL) {
			criamapa(objectos);
			criajogador1(objectos);
			criajogador2(objectos);
			CreateThread(NULL, 0, thread3, NULL, 0, NULL);
		}
	}
	else {
		if (definicoes.pid1 != NULL && wcslen(definicoes.jogador2) < 1) {
			criamapa(objectos);
			criajogador1(objectos);
			CreateThread(NULL, 0, thread3, NULL, 0, NULL);
		}
	}
	Sleep(200);
	SetEvent(mapUpdate);
	return 0;
}

int criajogador1(obj *objectos) {
	if (objectos[0].tipo == NULL) {
			objectos[0].id = 1;
			objectos[0].tipo = 5;
			objectos[0].tamx = definicoes.tjogador1.tamx;
			objectos[0].tamy = definicoes.tjogador1.tamy;
			objectos[0].x = definicoes.folgahor;
			objectos[0].y = definicoes.maxy - definicoes.tjogador1.tamy - 100;
	}
	return 0;
}

int criajogador2(obj *objectos) {
	if (objectos[1].tipo == NULL) {
		objectos[1].id = 2;
		objectos[1].tipo = 6;
		objectos[1].tamx = definicoes.tjogador2.tamx;
		objectos[1].tamy = definicoes.tjogador2.tamy;
		objectos[1].x = definicoes.maxx - definicoes.folgahor - definicoes.tjogador2.tamx - 50;
		objectos[1].y = definicoes.maxy - definicoes.tjogador1.tamy - 100;
	}
	return 0;
}

int crianave(obj objecto, int indice, obj * objectos) {
	HANDLE semaforoThNave = CreateSemaphore(NULL, 1, 1, TEXT("semaforoThreadNave"));
	objectos[indice].id = objecto.id;
	objectos[indice].x = objecto.x;
	objectos[indice].y = objecto.y;
	objectos[indice].tipo = objecto.tipo;
	objectos[indice].tamx = objecto.tamx;
	objectos[indice].tamy = objecto.tamy;
	WaitForSingleObject(semaforoThNave, INFINITE);
	CreateThread(NULL, 0, thread2, (LPVOID)&objecto, 0, NULL);
	WaitForSingleObject(semaforoThNave, INFINITE);
	ReleaseSemaphore(semaforoThNave, 1, NULL);
	return 0;
}

DWORD WINAPI thread1(LPVOID param) {

	objCounterID = 0;
	buffercircular();

	return 0;
}

DWORD WINAPI thread2(LPVOID param) {
	HANDLE semaforoThNave = CreateSemaphore(NULL, 0, 1, TEXT("semaforoThreadNave"));
	obj * pobj;
	obj esteobjecto;
	pobj = (obj *)param;
	esteobjecto = *pobj;
	obj * mapa = mapeamento();
	ReleaseSemaphore(semaforoThNave, 1, NULL);
	int i,direcaobuff,destroi=1;
	nnaves++;

	while (1) {
		direcaobuff = direcao;
		WaitForSingleObject(clock, INFINITE);
		WaitForSingleObject(mutex, INFINITE);
		for (i = 0;i<300;i++) {
			if (mapa[i].id == esteobjecto.id) {
				destroi = 0;
				//if (mapa[i].tipo == definicoes.naveg.tipo || mapa[i].tipo == definicoes.navep.tipo ) {
					if (direcao == 2)
						descida++;
					if (mapa[i].x + mapa[i].tamx + 50 >= definicoes.maxx && direcao != 2 && direcao == 1) {
						direcao = 2;
					}

					if (mapa[i].x - 50 <= 0 && direcao != 2 && direcao == 3) {
						direcao = 2;
					}
					if (descida / nnaves >= 15 && (direcao == 2 && mapa[i].x - 50 <= 0 || mapa[i].x + mapa[i].tamx + 50 >= definicoes.maxx)) {
						if (mapa[i].x - 50 <= 0) {
							direcao = 1;
						}
						else {
							direcao = 3;
						}
						descida = 0;
					}



					if (direcaobuff == 1) {
						mapa[i].x = mapa[i].x + level;
					}
					if (direcaobuff == 2) {
						mapa[i].y = mapa[i].y + level;
					}
					if (direcaobuff == 3) {
						mapa[i].x = mapa[i].x - level;
					}
					if (direcaobuff == 4) {
						mapa[i].y = mapa[i].y - level;
					}
				}
			//}
		}
		if (destroi == 1){
			nnaves--;
			ReleaseMutex(mutex);
			return 0;
		}
		destroi = 1;
		nnavesprontas++;
		ReleaseMutex(mutex);
		WaitForSingleObject(clock2, INFINITE);
	}
	return 0;
}

DWORD WINAPI thread3(LPVOID param) {
	Sleep(3000);
	while (1) {
		if (nnaves<=0) {
			MessageBox(NULL, L"Game OVer",L"Servidor!", MB_OK | MB_ICONASTERISK);
		}
		Sleep(1);
		SetEvent(clock);
		while (1){
			WaitForSingleObject(mutex, INFINITE);
			if (nnaves <= nnavesprontas) {
				break;
			}
			ReleaseMutex(mutex);
		}
		nnavesprontas = 0;
		ResetEvent(clock);
		SetEvent(mapUpdate);
		WaitForSingleObject(jalitudo, 30);
		ReleaseMutex(mutex);
		SetEvent(clock2);
		//Sleep(1);
		ResetEvent(clock2);
	}
	return 0;
}

DWORD WINAPI thread4(LPVOID param) {
	HANDLE semaforoThTiro = CreateSemaphore(NULL, 1, 1, TEXT("semaforoThtiro"));
	msg * pinfo;
	msg info;
	pinfo = (msg *)param;
	info = *pinfo;
	ReleaseSemaphore(semaforoThTiro, 1, NULL);
	obj * objectos = mapeamento();
	int i,e;
	if (info.aux3 == 1) {//tiro do player 1
		for (i = 0;i < 300;i++) {
			if (objectos[i].tipo == NULL) {
				WaitForSingleObject(mutex, INFINITE);
				objectos[i].id = 500;
				objectos[i].tipo = definicoes.tiro.tipo;
				objectos[i].x = info.aux1;
				objectos[i].y = info.aux2;
				objectos[i].tamx = definicoes.tiro.tamx;
				objectos[i].tamy = definicoes.tiro.tamy;
				ReleaseMutex(mutex);
				while (objectos[i].y > 0){
					WaitForSingleObject(mutex, INFINITE);
					for (e = 0;e < 300;e++) {
						if (objectos[e].tipo !=  NULL) {
							if (objectos[e].x < objectos[i].x && (objectos[e].x + objectos[e].tamx) > objectos[i].x) {
								if (objectos[e].y < objectos[i].y && (objectos[e].y + objectos[e].tamy)> objectos[i].y) {
									objectos[e].id = NULL;
									objectos[e].tipo = NULL;
									objectos[i].id = NULL;
									objectos[i].tipo = NULL;
									return 0;
								}
							}
						}
					}
					objectos[i].y = objectos[i].y -4;
					SetEvent(mapUpdate);
					ReleaseMutex(mutex);
					Sleep(1);
				}
				WaitForSingleObject(mutex, INFINITE);
				objectos[i].id = NULL;
				objectos[i].tipo = NULL;
				ReleaseMutex(mutex);
				return 0;
			}
		}
	}
	else {//tiro do player 2
	}

	return 0;
}

int criamapa(obj * objectos) {
	int i,indice=2;
	obj objec;
	WaitForSingleObject(mutex, INFINITE);
	while (1) {

		for (i = definicoes.folgahor;i < definicoes.maxx;i++) {
			objec.id = indice + 1;
			objec.tipo = 1;
			objec.x = i;
			objec.y = definicoes.folgaver;
			objec.tamx = definicoes.naveg.tamx;
			objec.tamy = definicoes.naveg.tamy;
			crianave(objec, indice, objectos);

			i = i + definicoes.naveg.tamx + definicoes.folgax - 1;
			if (i + definicoes.naveg.tamx + definicoes.folgax + definicoes.folgahor + 1 >= definicoes.maxx)
				i = definicoes.maxx;
			indice++;
		}
		if (objectos[0].y + definicoes.navep.tamy >= (definicoes.maxy *0.25) || indice >= 200) {
			ReleaseMutex(mutex);
			return 0;
		}
		for (i = 0;i <= indice;i++) {
			objectos[i].y = objectos[i].y + definicoes.folgay + definicoes.naveg.tamy;;
		}
		for (i = definicoes.folgahor;i < definicoes.maxx;i++) {
			objec.id = indice + 1;
			objec.tipo = 2;
			objec.x = i;
			objec.y = definicoes.folgaver;
			objec.tamx = definicoes.navep.tamx;
			objec.tamy = definicoes.navep.tamy;
			crianave(objec, indice, objectos);

			i = i + definicoes.navep.tamx + definicoes.folgax - 1;
			if (i + definicoes.navep.tamx + definicoes.folgax + definicoes.folgahor + 1 >= definicoes.maxx)
				i = definicoes.maxx;
			indice++;
		}
		if (objectos[0].y + definicoes.naveg.tamy >= (definicoes.maxy *0.25) || indice >= 200) {
			ReleaseMutex(mutex);
			return 0;
		}
		for (i = 0;i <= indice;i++) {
			objectos[i].y = objectos[i].y + definicoes.folgay + definicoes.naveg.tamy;
		}
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

void SendDefinitions(int pid) {
	msg messag;
	int lag = 25;

	messag.aux5 = pid;


	messag.aux1 = definicoes.naveg.tipo;
	messag.aux2 = definicoes.naveg.tamx;
	messag.aux3 = definicoes.naveg.tamy;
	swprintf_s(messag.aux6,L"%s",definicoes.naveg.bitmap);

	buffercircular2(messag);
	Sleep(lag);

	messag.aux1 = definicoes.navep.tipo;
	messag.aux2 = definicoes.navep.tamx;
	messag.aux3 = definicoes.navep.tamy;
	swprintf_s(messag.aux6, L"%s", definicoes.navep.bitmap);

	buffercircular2(messag);
	Sleep(lag);

	messag.aux1 = definicoes.tiro.tipo;
	messag.aux2 = definicoes.tiro.tamx;
	messag.aux3 = definicoes.tiro.tamy;
	swprintf_s(messag.aux6, L"%s", definicoes.tiro.bitmap);

	buffercircular2(messag);
	Sleep(lag);

	messag.aux1 = definicoes.bomba.tipo;
	messag.aux2 = definicoes.bomba.tamx;
	messag.aux3 = definicoes.bomba.tamy;
	swprintf_s(messag.aux6, L"%s", definicoes.bomba.bitmap);

	buffercircular2(messag);
	Sleep(lag);

	messag.aux1 = definicoes.tjogador1.tipo;
	messag.aux2 = definicoes.tjogador1.tamx;
	messag.aux3 = definicoes.tjogador1.tamy;
	swprintf_s(messag.aux6, L"%s", definicoes.tjogador1.bitmap);

	buffercircular2(messag);
	Sleep(lag);

	messag.aux1 = definicoes.tjogador2.tipo;
	messag.aux2 = definicoes.tjogador2.tamx;
	messag.aux3 = definicoes.tjogador2.tamy;
	swprintf_s(messag.aux6, L"%s", definicoes.tjogador2.bitmap);

	buffercircular2(messag);
	Sleep(lag);

	messag.aux1 = definicoes.power1.tipo;
	messag.aux2 = definicoes.power1.tamx;
	messag.aux3 = definicoes.power1.tamy;
	swprintf_s(messag.aux6, L"%s", definicoes.power1.bitmap);

	buffercircular2(messag);
	Sleep(lag);

	messag.aux1 = definicoes.power2.tipo;
	messag.aux2 = definicoes.power2.tamx;
	messag.aux3 = definicoes.power2.tamy;
	swprintf_s(messag.aux6, L"%s", definicoes.power2.bitmap);

	buffercircular2(messag);
	Sleep(lag);

	messag.aux1 = definicoes.power3.tipo;
	messag.aux2 = definicoes.power3.tamx;
	messag.aux3 = definicoes.power3.tamy;
	swprintf_s(messag.aux6, L"%s", definicoes.power3.bitmap);

	buffercircular2(messag);
	Sleep(lag);

	messag.aux1 = definicoes.power4.tipo;
	messag.aux2 = definicoes.power4.tamx;
	messag.aux3 = definicoes.power4.tamy;
	swprintf_s(messag.aux6, L"%s", definicoes.power4.bitmap);

	buffercircular2(messag);
	Sleep(lag);

	messag.aux1 = definicoes.power5.tipo;
	messag.aux2 = definicoes.power5.tamx;
	messag.aux3 = definicoes.power5.tamy;
	swprintf_s(messag.aux6, L"%s", definicoes.power5.bitmap);

	buffercircular2(messag);
	Sleep(lag);

	messag.aux1 = definicoes.power6.tipo;
	messag.aux2 = definicoes.power6.tamx;
	messag.aux3 = definicoes.power6.tamy;
	swprintf_s(messag.aux6, L"%s", definicoes.power6.bitmap);

	buffercircular2(messag);
	Sleep(lag);

	messag.aux1 = definicoes.power7.tipo;
	messag.aux2 = definicoes.power7.tamx;
	messag.aux3 = definicoes.power7.tamy;
	swprintf_s(messag.aux6, L"%s", definicoes.power7.bitmap);

	buffercircular2(messag);
	Sleep(lag);

	messag.aux1 = definicoes.power8.tipo;
	messag.aux2 = definicoes.power8.tamx;
	messag.aux3 = definicoes.power8.tamy;
	swprintf_s(messag.aux6, L"%s", definicoes.power8.bitmap);

	buffercircular2(messag);
	Sleep(lag);
	//teclas
	if (wcslen(definicoes.jogador2) >= 1) {//Verdade multi player
		messag.aux5 = definicoes.pid1;
		messag.aux1 = definicoes.Tdireita;
		messag.aux2 = definicoes.Tesquerda;
		messag.aux3 = definicoes.Tdisparo;

		buffercircular2(messag);
		Sleep(lag);

		messag.aux5 = definicoes.pid2;
		messag.aux1 = definicoes.Tdireita2;
		messag.aux2 = definicoes.Tesquerda2;
		messag.aux3 = definicoes.Tdisparo2;

		buffercircular2(messag);
	}
	else {//single player
		messag.aux5 = definicoes.pid1;
		messag.aux1 = definicoes.Tdireita;
		messag.aux2 = definicoes.Tesquerda;
		messag.aux3 = definicoes.Tdisparo;

		buffercircular2(messag);
	}
	
}

int apanhatecla() {
	int i;
	for (i = 10;;i++) {
		if (GetAsyncKeyState(i))
			return i;
		if (i > 250)
			i = 10;
	}
	return 0;
}

//** no cliente -> fazer um dialog box para fazer o login
//espera pela conecçao cliente ou clientes
//envia dimensoes do mapa para o cliente atravez do buffer circular 2
//cria mapa com o alguritmos ja pseudofeito no papel