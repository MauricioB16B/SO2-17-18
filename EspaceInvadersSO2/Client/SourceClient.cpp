// GT_HelloWorldWin32.cpp  
// compile with: /D_UNICODE /DUNICODE /DWIN32 /D_WINDOWS /c  

#include <windows.h>  
#include <stdlib.h>  
#include <string.h>  
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <tchar.h>

// Global variables  
RECT rect1;
int retang = 0;
// The main window class name.  
static TCHAR szWindowClass[] = _T("win32app");

// The string that appears in the application's title bar.  
static TCHAR szTitle[] = _T("Espace invaders SO2 Mauricio");

HINSTANCE hInst;

// Forward declarations of functions included in this code module:  
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

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
//  PURPOSE:  Processes messages for the main window.  
//  
//  WM_PAINT    - Paint the main window  
//  WM_DESTROY  - post a quit message and return  
//  
// 
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR string[] = _T("1 -> Iniciar names pipes");
	WCHAR string2[]=_T("2 -> sair");
	//TCHAR string2[1024];
	//DWORD xPos, yPos;

	switch (message)
	{
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