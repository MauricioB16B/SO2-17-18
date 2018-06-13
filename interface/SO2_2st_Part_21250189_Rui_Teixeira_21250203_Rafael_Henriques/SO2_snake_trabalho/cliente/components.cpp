#include <stdio.h>
#include <io.h>
#include <Windows.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <tchar.h>
#include <conio.h>

#include "resource.h"
#include "components.h"

ATOM ClassRegister(HINSTANCE hInst, TCHAR*szProgName)
{
	WNDCLASSEX wcApp;	// WNDCLASSEX é uma estrutura cujos membros servem para 
	// ============================================================================
	// 1. Definição das características da janela "wcApp" 
	//    (Valores dos elementos da estrutura "wcApp" do tipo WNDCLASSEX)
	// ============================================================================
	wcApp.cbSize = sizeof(WNDCLASSEX);	// Tamanho da estrutura WNDCLASSEX
	wcApp.hInstance = hInst;			// Instância da janela actualmente exibida 
										// ("hInst" é parâmetro de WinMain e vem 
										// inicializada daí)
	wcApp.lpszClassName = szProgName;	// Nome da janela (neste caso = nome do programa)
	wcApp.lpfnWndProc = ActionListener;	// Endereço da função de processamento da janela 	// ("TrataEventos" foi declarada no início e                 // encontra-se mais abaixo)
	wcApp.style = CS_HREDRAW | CS_VREDRAW;// Estilo da janela: Fazer o redraw se for      // modificada horizontal ou verticalmente
	wcApp.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON_SNAKE));// "hIcon" = handler do ícon normal
															  //"NULL" = Icon definido no Windows
															  // "IDI_AP..." Ícone "aplicação"
	wcApp.hIconSm = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON_SNAKE));// "hIconSm" = handler do ícon pequeno
																//"NULL" = Icon definido no Windows
																// "IDI_INF..." Ícon de informação
	wcApp.hCursor = LoadCursor(NULL, IDC_ARROW);	// "hCursor" = handler do cursor (rato) 
													// "NULL" = Forma definida no Windows
													// "IDC_ARROW" Aspecto "seta" 
	wcApp.lpszMenuName = NULL;			// Classe do menu que a janela pode ter
										// (NULL = não tem menu)
	wcApp.cbClsExtra = 0;				// Livre, para uso particular
	wcApp.cbWndExtra = 0;				// Livre, para uso particular
	wcApp.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	// "hbrBackground" = handler para "brush" de pintura do fundo da janela. Devolvido por  // "GetStockObject".Neste caso o fundo será branco
	return RegisterClassEx(&wcApp);
}

HWND createMainWindow(HINSTANCE hInst,TCHAR *szProgName, int locx, int locy, int sizex, int sizey)
{
	// ============================================================================
	// 3. Criar a janela
	// ============================================================================
	//hWnd = CreateWindow(
	//	szProgName,			// Nome da janela (programa) definido acima
	//	TEXT("Snake Game"),// Texto que figura na barra do título
	//	WS_OVERLAPPEDWINDOW,	// Estilo da janela (WS_OVERLAPPED= normal)
	//	MAIN_WND_X,		// Posição x pixels (default=à direita da última)
	//	MAIN_WND_Y,		// Posição y pixels (default=abaixo da última)
	//	MAIN_WND_X,		// Largura da janela (em pixels)
	//	MAIN_WND_Y,		// Altura da janela (em pixels)
	//(HWND)HWND_DESKTOP,	// handle da janela pai (se se criar uma a partir de
	// outra) ou HWND_DESKTOP se a janela for a primeira, 
	// criada a partir do "desktop"
	//	(HMENU)NULL,			// handle do menu da janela (se tiver menu)
	//HINSTANCE)hInst,		// handle da instância do programa actual ("hInst" é 
	// passado num dos parâmetros de WinMain()
	//0);				// Não há parâmetros adicionais para a janela
	HWND hWnd;
	//============================================================================
	// 3. Criar a janela
	//============================================================================
	hWnd = CreateWindow(
		szProgName,			// Nome da janela (programa) definido acima
		TEXT("Snake Game"),// Texto que figura na barra do título
						   //WS_OVERLAPPEDWINDOW,	// Estilo da janela (WS_OVERLAPPED= normal)
		WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_BORDER | WS_MINIMIZEBOX,
		//WS_OVERLAPPED,
		locx,		// Posição x pixels (default=à direita da última)
		locy,		// Posição y pixels (default=abaixo da última)
		sizex + 20,		// Largura da janela (em pixels)
		sizey + 40,		// Altura da janela (em pixels)
		(HWND)HWND_DESKTOP,	// handle da janela pai (se se criar uma a partir de
							// outra) ou HWND_DESKTOP se a janela for a primeira, 
							// criada a partir do "desktop"
		(HMENU)NULL,			// handle do menu da janela (se tiver menu)
		(HINSTANCE)hInst,		// handle da instância do programa actual ("hInst" é 
								// passado num dos parâmetros de WinMain()
		0);	// Não há parâmetros adicionais para a janela

	centerWindow(hWnd);
	return hWnd;
}

HWND createDialogConfig(HINSTANCE hInst)
{
	HWND hWnd;
	hWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG_CONFIG), (HWND)HWND_DESKTOP, (DLGPROC)ConfigListener);
	centerWindow(hWnd);
	return hWnd;
}

HWND createDiagonJoin(HINSTANCE hInst)
{
	HWND hWnd;
	hWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG_JOIN), (HWND)HWND_DESKTOP, (DLGPROC)NULL);
	centerWindow(hWnd);
	return hWnd;
}

HWND createDialogAWaitPlayers(HINSTANCE hInst)
{
	HWND hWnd;
	hWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG_AWAITPLAYERS), (HWND)HWND_DESKTOP, (DLGPROC)AwaitPlayersListener);
	centerWindow(hWnd);
	return hWnd;
}

HWND createDialogRun(HINSTANCE hInst)
{
	HWND hWnd;
	hWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG_RUN), (HWND)HWND_DESKTOP, (DLGPROC)RunListener);
	centerWindow(hWnd);
	return hWnd;
}
HWND createChangePicture(HINSTANCE hInst)
{
	HWND hWnd;
	hWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG_BMP_EDITOR), (HWND)HWND_DESKTOP, (DLGPROC)ChangePictureListener);
	centerWindow(hWnd);
	return hWnd;
}


void centerWindow(HWND hWnd)
{
	RECT rc;
	GetWindowRect(hWnd, &rc);

	int xPos = (GetSystemMetrics(SM_CXSCREEN) - rc.right) / 2;
	int yPos = (GetSystemMetrics(SM_CYSCREEN) - rc.bottom) / 2;

	SetWindowPos(hWnd, 0, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}

void showWindow(HWND hWnd, int nCmdShow)
{
	ShowWindow(hWnd, nCmdShow);	// "hWnd"= handler da janela, devolvido por 
								// "CreateWindow"; "nCmdShow"= modo de exibição (p.e. 
								// normal/modal); é passado como parâmetro de WinMain()
	UpdateWindow(hWnd);			// Refrescar a janela (Windows envia à janela uma 

								// mensagem para pintar, mostrar dados, (refrescar)… 
								// ============================================================================
								// 5. Loop de Mensagens
								// ============================================================================
								// O Windows envia mensagens às janelas (programas). Estas mensagens ficam numa fila de
								// espera até que GetMessage(...) possa ler "a mensagem seguinte"	
								// Parâmetros de "getMessage":
								// 1)"&lpMsg"=Endereço de uma estrutura do tipo MSG ("MSG lpMsg" ja foi declarada no  
								//   início de WinMain()):
								//			HWND hwnd		handler da janela a que se destina a mensagem
								//			UINT message		Identificador da mensagem
								//			WPARAM wParam		Parâmetro, p.e. código da tecla premida
								//			LPARAM lParam		Parâmetro, p.e. se ALT também estava premida
								//			DWORD time		Hora a que a mensagem foi enviada pelo Windows
								//			POINT pt		Localização do mouse (x, y) 
								// 2)handle da window para a qual se pretendem receber mensagens (=NULL se se pretendem //   receber as mensagens para todas as janelas pertencentes à thread actual)
								// 3)Código limite inferior das mensagens que se pretendem receber
								// 4)Código limite superior das mensagens que se pretendem receber

								// NOTA: GetMessage() devolve 0 quando for recebida a mensagem de fecho da janela,
								// 	  terminando então o loop de recepção de mensagens, e o programa 
}

void LoadBitMapsInClient(HINSTANCE hInst,HBITMAP* hBmp)
{
	hBmp[0] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_OBJ1));
	hBmp[1] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_OBJ2));
	hBmp[2] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_OBJ3));
	hBmp[3] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_OBJ4));
	hBmp[4] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_OBJ5));
	hBmp[5] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_OBJ6));
	hBmp[6] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_OBJ7));
	hBmp[7] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_OBJ8));
	hBmp[8] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_OBJ9));
	hBmp[9] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_OBJ10));
}

void editResourceOnPaint(int resource,HWND hWnd) 
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	TCHAR filePath[TAM], aux[TAM];

	switch (resource) 
	{
	case 0:
		_tcsncpy_s(filePath, TAM, TEXT(" pictures\\apple.bmp"), _TRUNCATE);
		break;
	case 1:
		_tcsncpy_s(filePath,TAM, TEXT(" pictures\\ice.bmp"), _TRUNCATE);
		break;
	case 2:
		_tcsncpy_s(filePath,TAM, TEXT(" pictures\\grenade.bmp"), _TRUNCATE);
		break;
	case 3:
		_tcsncpy_s(filePath, TAM, TEXT(" pictures\\vodka.bmp"), _TRUNCATE);
		break;
	case 4:
		_tcsncpy_s(filePath, TAM, TEXT(" pictures\\oil.bmp"), _TRUNCATE);
		break;
	case 5:
		_tcsncpy_s(filePath, TAM, TEXT(" pictures\\glue.bmp"), _TRUNCATE);
		break;
	case 6:
		_tcsncpy_s(filePath, TAM, TEXT(" pictures\\o_vodka.bmp"), _TRUNCATE);
		break;
	case 7:
		_tcsncpy_s(filePath, TAM, TEXT(" pictures\\o_oil.bmp"), _TRUNCATE);
		break;
	case 8:
		_tcsncpy_s(filePath, TAM, TEXT(" pictures\\o_glue.bmp"), _TRUNCATE);
		break;
	case 9:
		_tcsncpy_s(filePath, TAM, TEXT(" pictures\\hoe.bmp"), _TRUNCATE);
		break;
	default:
		break;
	}

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);

	//GetModuleFileName(TEXT("mspaint.exe"), executavel, 256);

	// Start the child process. 
	if (CreateProcess(
			TEXT("C:\\WINDOWS\\system32\\mspaint.exe"),		// No module name (use command line)
			filePath,										// Command line
			NULL,											// Process handle not inheritable
			NULL,											// Thread handle not inheritable
			0,												// Set handle inheritance to FALSE
			0,												// No creation flags
			NULL,											// Use parent's environment block
			NULL,											// Use parent's starting directory 
			&si,											// Pointer to STARTUPINFO structure
			&pi))											// Pointer to PROCESS_INFORMATION structure
	{
		// Wait until child process exits.
		WaitForSingleObject(pi.hProcess, INFINITE);

		// Close process and thread handles. 
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else
	{
		_stprintf_s(aux,TAM, TEXT("Error launching paint process (%d)\nPicture Path %s ID: %d"),
			GetLastError(),filePath, resource);
		MessageBox(hWnd, aux, TEXT("Error"), MB_ICONEXCLAMATION);
	}
}
