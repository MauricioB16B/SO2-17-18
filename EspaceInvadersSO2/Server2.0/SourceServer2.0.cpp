#include <windows.h>  
#include <stdlib.h>  
#include <string.h>  
#include <tchar.h>  
#include "resource.h"
#include <Commctrl.h>
#include <Windowsx.h>

static TCHAR szWindowClass[] = _T("win32app");

//titulo
static TCHAR szTitle[] = _T("Server");
typedef struct definicoes{
	int maxx;
	int maxy;
	TCHAR jogador1[1024];
	TCHAR Jogador2[1024];
}definicoes;

HINSTANCE hInst;
definicoes definicoes;

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
			DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, (DLGPROC)Dialog1Proc);
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
					EnableWindow(GetDlgItem(hwndDlg, IDC_EDIT12), TRUE);
				if (Button_GetCheck(GetDlgItem(hwndDlg, IDC_CHECK1)) == BST_UNCHECKED)
					EnableWindow(GetDlgItem(hwndDlg, IDC_EDIT12), FALSE);
			}	
			break;
		case IDCANCEL:
			EndDialog(hwndDlg, LOWORD(wParam));
			break;
		case IDOK:/*
			GetDlgItemText(hwndDlg,IDC_EDIT10,string1,1024);
			maxx = _wtoi(string1);
			GetDlgItemText(hwndDlg,IDC_EDIT11,string1,1024);
			maxy = _wtoi(string1);
			//GetDlgItem(hwndDlg,);
			*/
			definicoes.maxx = SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER1), (UINT)TBM_GETPOS, (WPARAM)0, (LPARAM)0);
			definicoes.maxy = SendMessage(GetDlgItem(hwndDlg, IDC_SLIDER2), (UINT)TBM_GETPOS, (WPARAM)0, (LPARAM)0);

			swprintf_s(string1,L"x:%d   y:%d",definicoes.maxx,definicoes.maxy);
			MessageBox(NULL,string1, _T("Janela de testes!! "), NULL);
			EndDialog(hwndDlg, LOWORD(wParam));
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