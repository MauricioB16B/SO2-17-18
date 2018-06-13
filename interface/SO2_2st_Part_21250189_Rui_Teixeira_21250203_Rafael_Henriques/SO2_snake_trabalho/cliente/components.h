#pragma once

#define TAM 100

// Define window characteristics
ATOM ClassRegister(HINSTANCE hInst, TCHAR*szProgName);

//Create Windows 
HWND createDialogRun(HINSTANCE hInst); // state 0
HWND createDialogConfig(HINSTANCE hInst); // state 1
HWND createDiagonJoin(HINSTANCE hInst); // state 2
HWND createDialogAWaitPlayers(HINSTANCE hInst); // state 2
HWND createMainWindow(HINSTANCE hInst, TCHAR *szProgName, int locx, int locy, int sizex, int sizey); // state 3
HWND createChangePicture(HINSTANCE hInst);
// Show Window and Update
void showWindow(HWND hWnd, int nCmdShow);

LRESULT CALLBACK ActionListener(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ConfigListener(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK RunListener(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK JoinListener(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ChangePictureListener(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK AwaitPlayersListener(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void centerWindow(HWND hWnd);

void LoadBitMapsInClient(HINSTANCE hInst, HBITMAP* hBmp);
void editResourceOnPaint(int resource, HWND hWnd);