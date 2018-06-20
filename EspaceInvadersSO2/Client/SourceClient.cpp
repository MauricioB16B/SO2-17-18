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
typedef struct {
	int tamx;
	int tamy;
	int tipo;
	TCHAR bitmap[1024];
	HBITMAP himg;
}tipo;

// Global variables
HWND handleWindowMain;
HANDLE hpipe, hpipe2, hpipe3;
HBITMAP bmp;
HBITMAP bmpBack;
HDC dc, dc2, dc2N;
obj mapa[300];
int primeiravez,velocidadeDir,velocidadeEsq;
tipo tipos[20];
// The main window class nome.

static TCHAR szWindowClass[] = _T("win32app");
static TCHAR szTitle[] = _T("Espace invaders SO2 Mauricio");
HINSTANCE hInst;

  
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

DWORD WINAPI thread1(LPVOID param);

DWORD WINAPI thread2(LPVOID param);

DWORD WINAPI thread3(LPVOID param);

DWORD WINAPI thread4(LPVOID param);

void UpdateDc();

int loadimg();

int loaddefinicoes();

BOOL CALLBACK DeleteItemProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

BOOL CALLBACK loginProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);


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
			DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG3), hWnd, (DLGPROC)loginProc);
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

	case WM_KEYDOWN:/*
		if ( wParam == VK_LEFT) {
			data.tipo = 2;
			data.aux1 = 2;
			data.aux5 = GetCurrentProcessId();
			WriteFile(hpipe, &data, sizeof(msg), NULL, NULL);
		}
		if ( wParam == VK_RIGHT) {
			data.tipo = 2;
			data.aux1 = 1;
			data.aux5 = GetCurrentProcessId();
			WriteFile(hpipe, &data, sizeof(msg), NULL, NULL);
		}
		if ( wParam == VK_UP) {
			data.tipo = 3;
			WriteFile(hpipe, &data, sizeof(msg), NULL, NULL);
		}*/
		if ( wParam == VK_DOWN) {
		}
		if ( wParam == VK_NUMPAD1) {
			
		}
		if (wParam == VK_NUMPAD5) {
		}
		if (wParam == VK_NUMPAD6) {
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

DWORD WINAPI thread4(LPVOID param) {
	msg data;
	data.aux5 = GetCurrentProcessId();
	int a = 0;
	while (true){
		if (GetAsyncKeyState(VK_LEFT)) {
			a++;
			data.tipo = 2;
			data.aux1 = 2;
			WriteFile(hpipe, &data, sizeof(msg), NULL, NULL);
			if (a < 20) {
				Sleep(20);
			}else{
				Sleep(5);
			}
		}
		else if (GetAsyncKeyState(VK_RIGHT)) {
			a++;
			data.tipo = 2;
			data.aux1 = 1;
			WriteFile(hpipe, &data, sizeof(msg), NULL, NULL);
			if (a < 20) {
				Sleep(20);
			}
			else {
				Sleep(5);
			}
		}
		else{
			a = 0;
		}
	}
	return 0;
}

DWORD WINAPI thread1(LPVOID param) {
	Sleep(500);
	msg data;
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

BOOL CALLBACK loginProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	msg data, data2;
	switch (message){
	case WM_COMMAND:
		switch (LOWORD(wParam)){
		case IDLOGIN:
			GetDlgItemText(hwndDlg, IDC_EDIT1, data.aux6, 1024);
			data.tipo = 1;
			data.aux5 = GetCurrentProcessId();
			data.aux7[0] = '\0';
			data.aux8[0] = '\0';
			if (!WriteFile(hpipe, &data, sizeof(msg), NULL, NULL)) {
				MessageBox(NULL, L"Erro na escrita do pipe!", _T("Janela de testes!! "), NULL);
			}
			ReadFile(hpipe2, &data2, sizeof(msg), NULL, NULL);
			if (data2.aux1 == -1) {
				MessageBox(NULL, data2.aux6, _T("Erro"), NULL);
			}else{
				loaddefinicoes();
				loadimg();
				CreateThread(NULL, 0, thread3, NULL, 0, NULL);
				MessageBox(NULL, data2.aux6, _T("LogIN"), NULL);
				CreateThread(NULL, 0, thread4, (LPVOID)NULL, 0, NULL);
				MoveWindow(handleWindowMain, 0, 0, data2.aux2, data2.aux3, TRUE);

				EndDialog(hwndDlg, LOWORD(wParam));
			}
			break;
		case IDCANCEL:
			EndDialog(hwndDlg, LOWORD(wParam));
			break;
		default:
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hwndDlg, LOWORD(wParam));
		break;
	default:
		break;
	}
	return 0;
}

void UpdateDc() {
	int i;
	if(dc2N == NULL)
		dc2N = CreateCompatibleDC(dc);


	SelectObject(dc2N, bmpBack);
	BitBlt(dc2, 0, 0, 1920, 1080, dc2N, 0, 0, SRCCOPY);

	for (i = 0;i<300;i++) {
		if (mapa[i].tipo != NULL) {
			SelectObject(dc2N, tipos[mapa[i].tipo - 1].himg);
			BitBlt(dc2, mapa[i].x, mapa[i].y, mapa[i].tamx, mapa[i].tamy, dc2N, 0, 0, SRCCOPY);
		}
	}
	BitBlt(dc, 0, 0, 1920, 1080, dc2, 0, 0, SRCCOPY);
	//InvalidateRect(handleWindowMain, NULL, TRUE);
}

int loaddefinicoes() {
	msg messag;
	int i;
	
	for (i = 0;i <= 13 ;i++) {
		ReadFile(hpipe2, &messag, sizeof(msg), NULL, NULL);
		tipos[i].tipo = messag.aux1;
		tipos[i].tamx = messag.aux2;;
		tipos[i].tamy = messag.aux3;
		swprintf_s(tipos[i].bitmap, L"%s", messag.aux6);
	}
	return 0;
}

int loadimg() {
	TCHAR str1[1024];
	int i;
	bmpBack = (HBITMAP)LoadImage(NULL, L"img\\ground1.bmp", IMAGE_BITMAP, 1920, 1080, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_CREATEDIBSECTION);
	for (i = 0;i <= 13;i++) {
		tipos[i].himg = (HBITMAP)LoadImage(NULL, tipos[i].bitmap, IMAGE_BITMAP, tipos[i].tamx, tipos[i].tamy, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_CREATEDIBSECTION);
		if (tipos[i].himg == NULL) {
			swprintf_s(str1, L"%s", tipos[i].bitmap);
			MessageBox(NULL, str1, L"Erro imagem not found", NULL);
		}
	}
	return 0;
}