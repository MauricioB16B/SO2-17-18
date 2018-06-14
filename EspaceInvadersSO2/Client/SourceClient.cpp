#include <windows.h>  
#include <stdlib.h>  
#include <string.h>  
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <tchar.h>
#include <WinGdi.h>
#include "resource.h"


// Declaration of Constantes e valores
#define PIPE_NAME TEXT("\\\\.\\pipe\\main")

// Strctures declaration
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

// Global variables
HWND handleWindowMain;
HANDLE hpipe, hpipe2, hpipe3;
HBITMAP bmp;
HDC dc, dc2;
obj mapa[300];
int primeiravez;
// The main window class nome.

static TCHAR szWindowClass[] = _T("win32app");
static TCHAR szTitle[] = _T("Espace invaders SO2 Mauricio");
HINSTANCE hInst;

  
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

DWORD WINAPI thread1(LPVOID param);

DWORD WINAPI thread2(LPVOID param);

DWORD WINAPI thread3(LPVOID param);

void UpdateDc();

BOOL CALLBACK DeleteItemProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);


int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow){
	WNDCLASSEX wcex;
	primeiravez = 0;// apagar isto!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//Definicoes da janelinha
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_WARNING));

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

	HWND hWnd = CreateWindow(szWindowClass,szTitle, WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,CW_USEDEFAULT , CW_USEDEFAULT,1300, 500,NULL,NULL,hInstance,NULL);

	if (!hWnd){
		MessageBox(NULL,_T("Call to CreateWindow failed!"),_T("Win32 Guided Tour"),NULL);
		return 1;
	}

	// The parameters to ShowWindow explained:  
	// hWnd: the value returned from CreateWindow  
	// nCmdShow: the fourth parameter from WinMain  
	ShowWindow(hWnd,nCmdShow);
	UpdateWindow(hWnd);
	handleWindowMain = hWnd;

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
	msg data;
	HDC hdc;
	TCHAR string[] = _T("1 -> Iniciar names pipes");
	WCHAR string2[] = _T("2 -> sair");
	HBITMAP bmp;
	TCHAR string22[1024];
	int i;

	switch (message)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam)){
		case ID_FILE_SAIR40005:
			PostQuitMessage(0);
			break;
		case ID_FILE_SAIR:
			//CreateThread(NULL, 0, thread1, (LPVOID)NULL, 0, NULL);
			data.tipo = 2;
			data.aux1 = 1501;
			data.aux2 = 1502;
			data.aux3 = 1503;
			data.aux4 = 1504;
			data.aux5 = 1505;
			wcscpy_s(data.aux6,L"OLA 1506");
			wcscpy_s(data.aux7, L"OLA 1507");
			wcscpy_s(data.aux8, L"OLA 1508");
			WriteFile(hpipe, &data, sizeof(msg), NULL, NULL);
			break;
		case ID_OPCOES_DEFINICOES: 
			DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, (DLGPROC)DeleteItemProc);
			break;
		case ID_FILE_NOVOJOGO:
			data.tipo = 1;
			data.aux6[0] = '\0';
			data.aux7[0] = '\0';
			data.aux8[0] = '\0';
			if (!WriteFile(hpipe, &data, sizeof(msg), NULL, NULL)) {
				MessageBox(NULL, L"Erro na escrita do pipe!", _T("Janela de testes!! "), NULL);
			}
			else if (primeiravez == 0) {
				CreateThread(NULL, 0, thread3, (LPVOID)&hWnd, 0, NULL);
				primeiravez = 1;
			}
			break;

		default:
			break;
		}
		break;
	case WM_CREATE:
		CreateThread(NULL, 0, thread1, (LPVOID)NULL, 0, NULL);
		//****************************************************************
		dc = GetDC(hWnd);
		dc2 = CreateCompatibleDC(dc);
		bmp = (HBITMAP)LoadImage(NULL, L"img\\ground1.bmp", IMAGE_BITMAP, 1920, 1080, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_CREATEDIBSECTION);
		if (bmp == NULL) {
			swprintf_s(string22, L"ERRO: %d", GetLastError());
			GetLastError();
			MessageBox(NULL, string22, _T("Janela de testes!! "), NULL);
		}
		SelectObject(dc2, bmp);
		//***************************************************************
		break;

	case WM_KEYDOWN:
		if ( wParam == VK_LEFT) {
			mapa[2].x-=3;
			CreateThread(NULL, 0, thread2, (LPVOID)&hWnd, 0, NULL);
		}
		if ( wParam == VK_RIGHT) {
			mapa[2].x+=3;
			CreateThread(NULL, 0, thread2, (LPVOID)&hWnd, 0, NULL);
		}
		if ( wParam == VK_UP) {
			mapa[2].y-=3;
			CreateThread(NULL, 0, thread2, (LPVOID)&hWnd, 0, NULL);
		}
		if ( wParam == VK_DOWN) {
			mapa[2].y+=3;
			CreateThread(NULL, 0, thread2, (LPVOID)&hWnd, 0, NULL);
		}
		if ( wParam == VK_NUMPAD1) {
			data.aux1 = 512;
			WriteFile(hpipe, &data, sizeof(msg), NULL, NULL);
			ReadFile(hpipe2, &data, sizeof(msg), NULL, NULL);
			MessageBox(NULL, data.aux6, _T(" sucess!! "), MB_ICONASTERISK | MB_OK);
		}
		if (wParam == VK_NUMPAD5) {
		}
		if (wParam == VK_NUMPAD6) {
			for (i = 0;i<300;i++) {
				if (mapa[i].id != NULL) {
					MessageBox(NULL, L"mais um!", _T(" sucess!! "), MB_ICONASTERISK | MB_OK);
				}
			}
		}
		break;
	case WM_LBUTTONDOWN:
		break;

	case WM_LBUTTONUP:
		break;

	case WM_MOUSEMOVE:
		break;
	case WM_PAINT:

		hdc = BeginPaint(hWnd, &ps);

		BitBlt(dc, 0, 0, 1920, 1080, dc2, 0, 0, SRCCOPY);

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

DWORD WINAPI thread1(LPVOID param) {
	Sleep(500);
	msg data , dataIn;
	TCHAR string[1024];
	TCHAR NomePipeInMsg[1024];
	TCHAR NomePipeInObj[1024];
	swprintf_s(NomePipeInMsg, TEXT("\\\\.\\pipe\\%d"), GetCurrentProcessId());
	swprintf_s(NomePipeInObj, TEXT("\\\\.\\pipe\\%dobj"), GetCurrentProcessId());

	//TCHAR string[1024];
	if (!WaitNamedPipe(PIPE_NAME, NMPWAIT_WAIT_FOREVER)) {
		MessageBox(NULL, _T("EROO Pipe de comunicaçao nao encontrado!"), _T(" Comunications Error "), MB_ICONERROR | MB_OK);
		return 0;
	}
	hpipe = CreateFile(PIPE_NAME, GENERIC_WRITE, 0, NULL, OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
	if (hpipe==NULL) {
		MessageBox(NULL, _T("CreateFile"), _T("ERRO"), NULL);
		return 0;
	}
	wcscpy_s(data.aux8, NomePipeInMsg);
	if (!WriteFile(hpipe,&data,sizeof(msg),NULL,NULL)) {
		MessageBox(NULL, _T("EROO Nao foi possivel escrever no Pipe"), _T(" Comunications Error "), MB_ICONERROR | MB_OK);
	}

	hpipe2 = CreateNamedPipe(NomePipeInMsg, PIPE_ACCESS_INBOUND, PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, PIPE_UNLIMITED_INSTANCES, sizeof(msg), sizeof(msg), 1000, NULL);
	if (hpipe2 == NULL) {
		MessageBox(NULL, _T("ERRO no pipe de entrada das MSG(CreateNamedPipe)"), _T(" Comunications Error "), MB_ICONERROR | MB_OK);
	}
	
	if (!ConnectNamedPipe(hpipe2, NULL)) {
		MessageBox(NULL, _T("EROO Ligaçao ao Getaway! (msg) (ConnectNamedPipe)"), _T(" Comunications Error "), MB_ICONERROR | MB_OK);
	}

	//*************

	hpipe3 = CreateNamedPipe(NomePipeInObj, PIPE_ACCESS_INBOUND, PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, PIPE_UNLIMITED_INSTANCES, sizeof(obj), sizeof(obj), 1000, NULL);
	if (hpipe3 == NULL) {
		MessageBox(NULL, _T("ERRO no pipe de entrada dos OBJ(CreateNamedPipe)"), _T(" Comunications Error "), MB_ICONERROR | MB_OK);
	}

	if (!ConnectNamedPipe(hpipe3, NULL)) {
		MessageBox(NULL, _T("EROO Ligaçao ao Getaway! (obj)(ConnectNamedPipe)"), _T(" Comunications Error "), MB_ICONERROR | MB_OK);
	}

	MessageBox(NULL, L"Tudo LIGADO!!!", _T(" sucess!! "), MB_ICONASTERISK | MB_OK);

	return 0;
}

DWORD WINAPI thread2(LPVOID param) {
	HWND *hWndp;
	HWND hWnd;
	HBITMAP bmpN,bmpBack;
	HDC dc2N;
	hWndp = (HWND *)param;
	hWnd = *hWndp;

	dc2N = CreateCompatibleDC(dc);
	bmpN = (HBITMAP)LoadImage(NULL, L"img\\teste512.bmp", IMAGE_BITMAP, 60, 55, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_CREATEDIBSECTION);

	if (bmpN == NULL) {
		//swprintf_s(string22, L"ERRO: %d", GetLastError());
		GetLastError();
		MessageBox(NULL, L"erro na imagem", _T("Janela de testes!! "), NULL);
	}
	bmpBack = (HBITMAP)LoadImage(NULL, L"img\\ground1.bmp", IMAGE_BITMAP, 1920, 1080, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_CREATEDIBSECTION);
	SelectObject(dc2N, bmpBack);
	BitBlt(dc2, 0, 0, 1920, 1080, dc2N, 0, 0, SRCCOPY);

	for (int i = 0;i<300;i++) {
		if (mapa[i].tipo != NULL) {
			SelectObject(dc2N, bmpN);
			BitBlt(dc2, mapa[i].x, mapa[i].y, 1920, 1080, dc2N, 0, 0, SRCCOPY);
		}
	}
	InvalidateRect(hWnd, NULL, TRUE);
	return 0;
}

DWORD WINAPI thread3(LPVOID param) {
	DWORD n;
	obj objecto;
	int i;
	while (true) {
		for (i = 0;i < 300;i++) {
			ReadFile(hpipe3, &objecto, sizeof(obj), &n, NULL);
			if (objecto.id == 5000) {
				i = 300;
			}else{
				mapa[i] = objecto;//mutex mapa
			}
		}
		UpdateDc();
	}
	
	return 0;
}

BOOL CALLBACK DeleteItemProc(HWND hwndDlg,UINT message,WPARAM wParam,LPARAM lParam){
	PAINTSTRUCT ps;
	HDC hdc;
	HBITMAP bmp;
	HWND hwinmain=NULL;
	TCHAR string1[1024];
	TCHAR string2[1024];
	switch (message){
	case WM_INITDIALOG:
		bmp = (HBITMAP)LoadImage(NULL, L"img\\teste.bmp", IMAGE_BITMAP, 100, 80, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_CREATEDIBSECTION);
		if (bmp == NULL) {
			MessageBox(NULL, _T("Erro ao carregar imagem"), _T("Janela de testes!! "), NULL);
		}
		SendDlgItemMessage(hwndDlg, IDC_STATIC69, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmp);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwndDlg, &ps);

		EndPaint(hwndDlg, &ps);
		break;
	case WM_COMMAND:
		if(LOWORD(wParam)== IDC_CANCELL)
			EndDialog(hwndDlg, LOWORD(wParam));
		if (LOWORD(wParam) == IDC_GET)
			MessageBox(NULL, _T("Botao GET"), _T("Janela de testes!! "), NULL);
		if (LOWORD(wParam) == IDC_SET) {
			//MessageBox(NULL, _T("Botao SET"), _T("Janela de testes!! "), NULL);
			HWND editcontrolx = GetDlgItem(hwndDlg, IDC_EDIT4);
			HWND editcontroly = GetDlgItem(hwndDlg, IDC_EDIT3);
			GetWindowText(editcontrolx,string1,1024);
			GetWindowText(editcontroly, string2, 1024);
			int x = _wtoi(string1);
			int y = _wtoi(string2);

			hwinmain = GetWindow(hwndDlg, GW_OWNER);
			MoveWindow(hwinmain, 0, 0, x, y, TRUE);
		}
		break;
	case WM_CLOSE:
		EndDialog(hwndDlg, LOWORD(wParam));
		break;
	}return FALSE;		
}

void UpdateDc() {
	HBITMAP bmpN, bmpBack;
	HDC dc2N;

	dc2N = CreateCompatibleDC(dc);
	bmpN = (HBITMAP)LoadImage(NULL, L"img\\teste512.bmp", IMAGE_BITMAP, 45 , 30, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_CREATEDIBSECTION);

	if (bmpN == NULL) {
		//swprintf_s(string22, L"ERRO: %d", GetLastError());
		GetLastError();
		MessageBox(NULL, L"erro na imagem", _T("Janela de testes!! "), NULL);
	}
	bmpBack = (HBITMAP)LoadImage(NULL, L"img\\ground1.bmp", IMAGE_BITMAP, 1920, 1080, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_CREATEDIBSECTION);
	SelectObject(dc2N, bmpBack);
	BitBlt(dc2, 0, 0, 1920, 1080, dc2N, 0, 0, SRCCOPY);

	for (int i = 0;i<300;i++) {
		if (mapa[i].tipo != NULL) {
			SelectObject(dc2N, bmpN);
			BitBlt(dc2, mapa[i].x, mapa[i].y, 500, 500, dc2N, 0, 0, SRCCOPY);
		}
	}
	InvalidateRect(handleWindowMain, NULL, TRUE);
}