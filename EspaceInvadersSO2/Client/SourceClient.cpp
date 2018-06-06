#include <windows.h>  
#include <stdlib.h>  
#include <string.h>  
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <tchar.h>

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

// Global variables
HANDLE hpipe, hpipe2;
RECT rect1;
int retang = 0;
// The main window class nome.  
static TCHAR szWindowClass[] = _T("win32app");

static TCHAR szTitle[] = _T("Espace invaders SO2 Mauricio");

HINSTANCE hInst;
  
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI thread1(LPVOID param);

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow){
	WNDCLASSEX wcex;

	//Definicoes da janelinha
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WARNING));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
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

	HWND hWnd = CreateWindow(szWindowClass,szTitle,WS_OVERLAPPEDWINDOW,CW_USEDEFAULT, CW_USEDEFAULT,1300, 500,NULL,NULL,hInstance,NULL);

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

//  
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)  
//  
//  PURPOSE:  Processes messages para a main window.  
//  
//  WM_PAINT    - Paint the main window  
//  WM_DESTROY  - Procedimento de quit  
//  
// 
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	PAINTSTRUCT ps;
	msg data;
	HDC hdc;
	TCHAR string[] = _T("1 -> Iniciar names pipes");
	WCHAR string2[] = _T("2 -> sair");
	//TCHAR string2[1024];
	//DWORD xPos, yPos;

	switch (message)
	{
	case WM_CREATE:
		CreateThread(NULL, 0, thread1, (LPVOID)NULL, 0, NULL);
	case WM_KEYDOWN:
		if (wParam == 0x32 || wParam == VK_NUMPAD2) {
			MessageBox(NULL, _T("Tecla 2"), _T("Janela de testes!! "), NULL);
		}

		if (wParam == 0x31 || wParam == VK_NUMPAD1) {
			data.aux1 = 512;
			WriteFile(hpipe, &data, sizeof(msg), NULL, NULL);
			ReadFile(hpipe2, &data, sizeof(msg), NULL, NULL);
			MessageBox(NULL, data.aux6, _T(" sucess!! "), MB_ICONASTERISK | MB_OK);
		}
		break;
	case WM_LBUTTONDOWN:
		//MessageBox(NULL, _T("Cenas maradas"), _T("Janela de testes!! "), NULL);
		rect1.left = LOWORD(lParam);
		rect1.top = HIWORD(lParam);
		retang = 1;
		
		//swprintf_s(string2, 1024,TEXT("x=:%d\ny=:%d"),xPos,yPos);
		//MessageBox(NULL, string2, _T("Janela de testes!! "), NULL);
		break;

	case WM_LBUTTONUP:
		rect1.right = LOWORD(lParam);
		rect1.bottom = HIWORD(lParam);
		retang = 0;
		UpdateWindow(hWnd);
		break;

	case WM_MOUSEMOVE:
		if (retang == 1) {
			hdc=GetDC(hWnd);
			rect1.right = LOWORD(lParam);
			rect1.bottom = HIWORD(lParam);

			InvalidateRect(hWnd, NULL, TRUE);
			//Rectangle(hdc, rect1.left, rect1.top, rect1.right, rect1.bottom);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		//if (retang==1) {
			Rectangle(ps.hdc, rect1.left, rect1.top, rect1.right, rect1.bottom);
		//}
		// Here your application is laid out.  
		// For this introduction, we just print out "Hello, World!"  
		// in the top left corner.  
		TextOut(hdc,45, 20,string, _tcslen(string));
		TextOut(hdc, 45, 40, string2, _tcslen(string2));
		// End application-specific layout section.  

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
	msg data,dataIn;
	DWORD n;
	TCHAR NomePipeInMsg[1024];
	TCHAR NomePipeInObj[1024];
	TCHAR string[1024];
	swprintf_s(NomePipeInMsg, TEXT("\\\\.\\pipe\\%d"), GetCurrentProcessId());
	swprintf_s(NomePipeInObj, TEXT("\\\\.\\pipe\\%dobj"), GetCurrentProcessId());

	//TCHAR string[1024];
	if (!WaitNamedPipe(PIPE_NAME, NMPWAIT_WAIT_FOREVER)) {
		MessageBox(NULL, _T("EROO Pipe de comunica�ao nao encontrado!"), _T(" Comunications Error "), MB_ICONERROR | MB_OK);
		return 0;
	}
	hpipe = CreateFile(PIPE_NAME, GENERIC_WRITE, 0, NULL, OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
	if (hpipe==NULL) {
		MessageBox(NULL, _T("CreateFile"), _T("ERRO"), NULL);
		return 0;
	}
	wcscpy_s(data.aux8, NomePipeInMsg);
	if (!WriteFile(hpipe,&data,sizeof(msg),&n,NULL)) {
		MessageBox(NULL, _T("EROO Nao foi possivel escrever no Pipe"), _T(" Comunications Error "), MB_ICONERROR | MB_OK);
	}

	hpipe2 = CreateNamedPipe(NomePipeInMsg, PIPE_ACCESS_INBOUND, PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, PIPE_UNLIMITED_INSTANCES, sizeof(msg), sizeof(msg), 1000, NULL);
	if (hpipe2 == NULL) {
		MessageBox(NULL, _T("ERRO no pipe de entrada das MSG(CreateNamedPipe)"), _T(" Comunications Error "), MB_ICONERROR | MB_OK);
	}

	if (! ConnectNamedPipe(hpipe2, NULL)) {
		MessageBox(NULL, _T("EROO Liga�ao ao Getaway! (ConnectNamedPipe)"), _T(" Comunications Error "), MB_ICONERROR | MB_OK);
	}

	ReadFile(hpipe2, &dataIn, sizeof(msg), &n, NULL);
	swprintf_s(string, TEXT("%s"), dataIn.aux8);
	MessageBox(NULL, string, _T(" sucess!! "), MB_ICONASTERISK | MB_OK);

	return 0;
}
