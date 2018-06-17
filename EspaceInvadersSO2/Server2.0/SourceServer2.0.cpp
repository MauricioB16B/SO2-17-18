#include <windows.h>  
#include <stdlib.h>  
#include <string.h>  
#include <tchar.h>  
#include "resource.h"
#include <Commctrl.h>
#include <Windowsx.h>

#define BufferSize 100
#define Buffers 10

typedef struct definicoes{
	int maxx;
	int maxy;
	int nnaves;
	CHAR Tdireita;
	CHAR Tesquerda;
	CHAR Tdisparo;
	CHAR Tdireita2;
	CHAR Tesquerda2;
	CHAR Tdisparo2;
	TCHAR jogador1[1024];
	TCHAR jogador2[1024];
}def;
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
	int id;
}tipo;

static TCHAR szWindowClass[] = _T("win32app");
static TCHAR szTitle[] = _T("Server");

HINSTANCE hInst;
def definicoes;
int objCounterID;
HANDLE mapUpdate;

int tratamsg(msg data);
obj * mapeamento();
int CriaNovoJogo(msg data);
int buffercircular();
DWORD WINAPI thread1(LPVOID param);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK Dialog1Proc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

int CALLBACK WinMain(_In_ HINSTANCE hInstance,_In_ HINSTANCE hPrevInstance,_In_ LPSTR     lpCmdLine,_In_ int       nCmdShow){
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
		SetDlgItemText(hwndDlg, IDC_EDIT3, L"a");
		SetDlgItemText(hwndDlg, IDC_EDIT4, L"d");
		SetDlgItemText(hwndDlg, IDC_EDIT5, L"k");
		SetDlgItemText(hwndDlg, IDC_EDIT6, L"4");
		SetDlgItemText(hwndDlg, IDC_EDIT7, L"6");
		SetDlgItemText(hwndDlg, IDC_EDIT8, L"0");
		break;
	case WM_CLOSE:
		EndDialog(hwndDlg, LOWORD(wParam));
		break;
	}
	return FALSE;
}

DWORD WINAPI thread1(LPVOID param) {
	mapUpdate = CreateEvent(NULL, FALSE, FALSE, TEXT("MapUpdate"));

	objCounterID = 0;
	while (1) {
		buffercircular();
	}

	return 0;
}

int buffercircular() {

	TCHAR NomeMemoria[] = TEXT("Nome da Mem�ria Partilhada");
	TCHAR NomeSemaforoPodeEscrever[] = TEXT("Sem�foro Pode Escrever");
	TCHAR NomeSemaforoPodeLer[] = TEXT("Sem�foro Pode Ler");

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
			_tprintf(TEXT("[Erro]Cria��o de objectos do Windows(%d)\n"), GetLastError());
			return -1;
		}

		shm = (bufferinfo*)MapViewOfFile(hMemoria, FILE_MAP_WRITE, 0, 0, sizeof(bufferinfo));
		if (shm == NULL) {
			_tprintf(TEXT("[Erro]Mapeamento da mem�ria partilhada(%d)\n"), GetLastError());
			return -1;
		}

		for (int i = 0;; ++i) {
			WaitForSingleObject(PodeLer, INFINITE);

			WaitForSingleObject(mutex, INFINITE);
			//ler IN par aa var local POS
			pos = shm->iLeitura;
			shm->iLeitura = (shm->iLeitura + 1) % Buffers;//adiciona ate que � == a Buffers
														  //Incrementar valor de IN
			ReleaseMutex(mutex);

			tratamsg(shm->dados[pos]);// copia data do buffer para variavel da funcao e liberta o buffer
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
		ReleaseSemaphore(semaforo1, 1, NULL);
		break;
	case 3:
		ReleaseSemaphore(semaforo1, 1, NULL);
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
	WaitForSingleObject(semaforo1, INFINITE);
	ReleaseSemaphore(semaforo1, 1, NULL);
	return 0;
}

int CriaNovoJogo(msg data) {
	obj * objectos = mapeamento();

	//** no cliente -> fazer um dialog box para fazer o login
	//espera pela conec�ao cliente ou clientes
	//envia dimensoes do mapa para o cliente atravez do buffer circular 2
	//cria mapa com o alguritmos ja pseudofeito no papel

	objectos[0].id = 1;
	objectos[0].tipo = 1;
	objectos[0].x = 100;
	objectos[0].y = 150;
	objectos[0].tamx = 10;
	objectos[0].tamy = 10;

	SetEvent(mapUpdate);
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