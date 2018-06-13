#pragma comment(lib, "DLL.lib")
#include "cliente.h"
#include <CommCtrl.h>

#include "components.h"
#include "processinfo.h"
int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPTSTR lpCmdLine, int nCmdShow)
{
	TCHAR aux[200];
	HWND hWnd,hWndRun,hWndJoinGame, hWndConfig, hWndAwaitPlayers,hWndChangePicture;		
	MSG lpMsg;		
	BOOL ret;		
	DWORD timeout;

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif
	// ============================================================================
	// 2. Registar a classe "wcApp" no Windows
	// ============================================================================
	if (!ClassRegister(hInst, szProgName))
		return(0);
	kindConnection = 0;
	turnOffCli = 0;
	canLaunchRun = 0;
	canLaunchConfig = 0;
	canLaunchJoinGame = 0;
	canLaunchAwaitPlayers = 0;
	canLaunchChangePicture = 0;
	canLaunchGame = 0;
	hWndRun = createDialogRun(hInst);
	showWindow(hWndRun, nCmdShow);
	DefaultKeys();
	//loadCliBitmaps(hInst,hBmp);
		//if (hBmp[0] == NULL)
			//return 3;
	LoadBitMapsInClient(hInst,hBmp);

	//hMutex = CreateMutex(NULL, TRUE, 0);
	//if (hMutex == NULL) { _tprintf(TEXT("\nError Creating a Mutex (%d)\n"), GetLastError()); return 1; }
	
	
	while ((ret = GetMessage(&lpMsg, NULL, 0, 0)) != 0)
	{

		if (ret != -1)
		{
			TranslateMessage(&lpMsg);	// Pré-processamento da mensagem (p.e. obter código 
										// ASCII da tecla premida)
			DispatchMessage(&lpMsg);	// Enviar a mensagem traduzida de volta ao Windows, que
										// aguarda até que a possa reenviar à função de 
										// tratamento da janela, CALLBACK TrataEventos (abaixo)
		}
		if (canLaunchRun == 1)
		{
			canLaunchRun = 0;
			hWndRun = createDialogRun(hInst);
			showWindow(hWndRun, nCmdShow);
		}
		else if (canLaunchConfig == 1)
		{
			config.giveup = 2;
			clientSetConfigSM(config);
			canLaunchConfig = 0;
			hWndConfig = createDialogConfig(hInst);
			showWindow(hWndConfig, nCmdShow);
		}
		else if (canLaunchJoinGame == 1)
		{
			canLaunchJoinGame = 0;
			hWndJoinGame = createDiagonJoin(hInst);
			showWindow(hWndJoinGame, nCmdShow);
		}
		else if (canLaunchAwaitPlayers == 1)
		{
			canLaunchAwaitPlayers = 0;
			hWndAwaitPlayers = createDialogAWaitPlayers(hInst);
			showWindow(hWndAwaitPlayers, nCmdShow);
		}
		else if (canLaunchGame == 1)
		{
			canLaunchGame = 0;
			hRecv = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)recvDataSM, &hWnd, 0, &threadID);
			
			hWnd = createMainWindow(hInst,szProgName,MAIN_WND_START_X,MAIN_WND_START_Y,
				config.fieldSizeX * SQUARE_SIZE_X + SQUARE_SIZE_X,config.fieldSizeY * SQUARE_SIZE_Y + SQUARE_SIZE_Y);
			hMain = hWnd;
			if (hRecv == NULL) 
			{ 
				_stprintf_s(aux, _TRUNCATE, TEXT("Error Lauching a Thread (%d)"), GetLastError());
				MessageBox(hWnd,aux,TEXT("Error Launching Thread"), MB_OK); break;
				CloseWindow(hWnd);
				clientInputGameSM(10000, &timeout, config.username1, 1, 1);
				return -1;
			}
			//else CloseHandle(hRecv);
			showWindow(hWnd, nCmdShow);
		}
		else if (canLaunchChangePicture == 1)
		{
			canLaunchChangePicture = 0;
			hWndChangePicture = createChangePicture(hInst);
			showWindow(hWndChangePicture, nCmdShow);
		}
	}
	//CreateConfigLabels(hWnd);
	// ============================================================================
	// 6. Fim do programa
	// ============================================================================
	WaitForSingleObject(hRecv,INFINITE);
	if(kindConnection == 1)closeSM();
	return((int)lpMsg.wParam);	// Retorna sempre o parâmetro wParam da estrutura lpMsg
}

void DefaultKeys()
{
	cliKeys[0].keyUp = 'w';
	cliKeys[0].keyDown = 's';
	cliKeys[0].keyRight = 'd';
	cliKeys[0].keyLeft = 'a';
	cliKeys[1].keyUp = 'i';
	cliKeys[1].keyDown = 'k';
	cliKeys[1].keyRight = 'l';
	cliKeys[1].keyLeft = 'j';
}

DWORD WINAPI recvDataSM(LPVOID *param)
{
	DWORD timeout;
	//HANDLE hMutex = (HANDLE *)*param;
	render = getRenderSM(30000, &timeout);
	for (int i = 0; i < render.numPlayers; i++)
		if (!_tcsncmp(render.snake[i].username, config.username1, sizeof(TCHAR) * MAX_TAM))
			clientIndexRender[0] = i;
		else if (coop && !_tcsncmp(render.snake[i].username, config.username2, sizeof(TCHAR) * MAX_TAM))
			clientIndexRender[1] = i;
	do
	{
		render = getRenderSM(30000, &timeout);
		if (timeout == 1) {}
		InvalidateRect(hMain,NULL,TRUE);
		UpdateWindow(hMain);
		Sleep(100);
	} while (turnOffCli == 0 && render.serverTurnedOff == 0 && render.gameEnded == 0);
	PostMessage(hMain, WM_CLOSE, 0, 0);
	return 0;
}

LRESULT CALLBACK ActionListener(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
	TCHAR aux[MAX_TAM], score[300];
	HDC device;
	HPEN hPen;
	PAINTSTRUCT pt;
	DWORD timeout;
	static TCHAR key = 'A';
	switch (messg)
	{
	case WM_CREATE:
		cirBufInSM(1500, &timeout, cliKeys[0].username, KEY_UP, 0);
		break;
	case WM_CHAR:
		if (render.serverTurnedOff == 1)
		{
			MessageBox(hWnd, TEXT("Server was turned off\nYou will be turned off"), TEXT("Server Information"), MB_OK | MB_ICONINFORMATION);
			PostQuitMessage(1);
			break;
		}
		key = wParam; // get key input

		if (key == cliKeys[0].keyUp)
			cirBufInSM(3000, &timeout, cliKeys[0].username, KEY_UP, 0);
		else if (key == cliKeys[0].keyDown)
			cirBufInSM(3000, &timeout, cliKeys[0].username, KEY_DOWN, 0);
		else if(key == cliKeys[0].keyRight)
			cirBufInSM(3000, &timeout, cliKeys[0].username, KEY_RIGHT, 0);
		else if(key == cliKeys[0].keyLeft)
			cirBufInSM(3000, &timeout, cliKeys[0].username, KEY_LEFT, 0);
		else if (coop)
		{
			if (key == cliKeys[1].keyUp)
				cirBufInSM(3000, &timeout, cliKeys[1].username, KEY_UP, 0);
			else if (key == cliKeys[1].keyDown)
				cirBufInSM(3000, &timeout, cliKeys[1].username, KEY_DOWN, 0);
			else if (key == cliKeys[1].keyRight)
				cirBufInSM(3000, &timeout, cliKeys[1].username, KEY_RIGHT, 0);
			else if (key == cliKeys[1].keyLeft)
				cirBufInSM(3000, &timeout, cliKeys[1].username, KEY_LEFT, 0);
		}
		
		InvalidateRect(hWnd, NULL, TRUE); // Force WM_PAINT
		UpdateWindow(hWnd);
	case WM_PAINT:
		device = BeginPaint(hWnd, &pt);
		hPen = CreatePen(PS_SOLID, 1, RGB(20, 20, 20));
		SelectObject(device, hPen);
		renderGame(device,render);
		DeleteObject(hPen);
		EndPaint(hWnd, &pt);
		break;
	case WM_LBUTTONUP:
		//_stprintf_s(str, _TRUNCATE, TEXT("Botão esquerdo largado: %d, %d\nRender(%d,%d)%d|%d"),
		//	LOWORD(lParam), HIWORD(lParam),render.field.object[0][0], render.field.object[0][1],
		//	render.field.objType[0],render.field.numObjs);
		//MessageBox(hWnd, str, TEXT("Evento do Ratro"), MB_OK);
		break;
	case WM_CLOSE:	// Destruir a janela e terminar o programa 
					// "PostQuitMessage(Exit Status)"		
		if (render.gameEnded)
		{
			_stprintf_s(score, _TRUNCATE, TEXT("%s \t->\t %d points\n"), render.snake[0].username, render.snake[0].points);
			for (int i = 1; i < render.numBots + render.numPlayers; i++)
			{
				_stprintf_s(aux, _TRUNCATE, TEXT("%s \t->\t %d points\n"), render.snake[i].username, render.snake[i].points);
				_tcsncat_s(score, _TRUNCATE, aux, sizeof(TCHAR)*_tcslen(aux));
			}
			MessageBox(hMain, score, TEXT("Score"), MB_OK);
			//canLaunchRun = 1;
			PostQuitMessage(0);
		}
		else if (render.serverTurnedOff)
		{
			_stprintf_s(aux, _TRUNCATE, TEXT("Client will Shut Down."));
			MessageBox(hMain, aux, TEXT("Server Turned Off"), MB_OK);
			PostQuitMessage(0);
		}
		if (MessageBox(hWnd, TEXT("Do you really want to exit game?"),
			TEXT("Warning"), MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
		{
			cirBufInSM(30000, &timeout, cliKeys[0].username, key, 1);
			if (coop)
				cirBufInSM(30000, &timeout, cliKeys[1].username, key, 1);
			turnOffCli = 1;
			PostQuitMessage(0);
		}
		break;
	default:
		return(DefWindowProc(hWnd, messg, wParam, lParam));
	}
	return(0);
}

void renderGame(HDC d, Info r)
{

	for (int i = 0; i < r.field.numObjs; i++)
	{
		drawImage(d, i, r);
	}

	for (int i = r.numPlayers; i < r.numBots + r.numPlayers; i++)
		drawSnake(d, r.snake[i], 255, 222, 173, 244, 164, 96);

	for (int i = 0; i < r.numPlayers; i++)
	{
		if (!_tcsncmp(r.snake[i].username, config.username1, sizeof(TCHAR) * MAX_TAM))
		{
			drawSnake(d, r.snake[i],0,191,255,0,0,205);
		}
		else if (!_tcsncmp(r.snake[i].username, config.username2, sizeof(TCHAR) * MAX_TAM))
		{
			drawSnake(d, r.snake[i],124,252,0,0,100,0);
		}
		else
			drawSnake(d, r.snake[i], 255, 215, 0, 255, 140, 0);
	}
	// if snake is under inverted keys, glue or oil
	for (int i = 0; i < r.numBots + r.numPlayers; i++)
		if(r.snake[i].invertKeys || r.snake[i].glue|| r.snake[i].oil)
			drawSnake(d, r.snake[i], 250, 128, 114, 178, 34, 34);
}

void drawImage(HDC d, int objIndex, Info r)
{
	HDC hdcMem;
	hdcMem = CreateCompatibleDC(d);
	SelectObject(hdcMem, hBmp[r.field.objType[objIndex] - 1]);
	BitBlt(d, r.field.object[objIndex][0] * SQUARE_SIZE_X,
		r.field.object[objIndex][1] * SQUARE_SIZE_Y, 
		SQUARE_SIZE_X, SQUARE_SIZE_Y, hdcMem, 0, 0, SRCCOPY);
	DeleteDC(hdcMem);
	
	//Rectangle(d, render.field.object[objIndex][0] * SQUARE_SIZE_X,
	//	render.field.object[objIndex][1] * SQUARE_SIZE_Y,
	//	(d, render.field.object[objIndex][0] + 1) * SQUARE_SIZE_X,
	//	(render.field.object[objIndex][1] + 1) * SQUARE_SIZE_Y);
}

void drawSnake(HDC d,Snake s,int r,int g, int b,int headr, int headg, int headb)
{
	if (s.died)
		return;
	HBRUSH hHead = CreateSolidBrush(RGB(r, g, b));
	HBRUSH hBody = CreateSolidBrush(RGB(headr, headg, headb));
	for (int i = 0; i < s.snakeSize; i++)
	{
		if (i)
			SelectObject(d, hHead);
		else
			SelectObject(d, hBody);

		Rectangle(d, s.coords[i][0] * SQUARE_SIZE_X,
			s.coords[i][1] * SQUARE_SIZE_Y,
			(d, s.coords[i][0] + 1) * SQUARE_SIZE_X,
			(s.coords[i][1] + 1) * SQUARE_SIZE_Y);
	}
	DeleteObject(hHead);
	DeleteObject(hBody);
}

LRESULT CALLBACK ConfigListener(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
	//HWND cboRespawn = NULL;
	const TCHAR *respawn[] = { TEXT("Low"), TEXT("Medium"), TEXT("High") };
	TCHAR aux[MAX_TAM];
	DWORD timeout;
	BOOL checked;
	int ret;
	switch (messg) 
	{
		/**	x
		*	-> WM_CLOSE   	// Windows will close
		*	-> WM_DESTROY	// Windows will be erased
		*	-> WM_QUIT	// App will close
		*
		*	@return IDCANCEL, IDOK, IDYES, IDNO, ...
		*
		*	MB_OKCANCEL, MB_YESNO, MB_YESNOCANCEL, MB_OK
		****/
		//TCHAR str[100];
		//HDC device;
		//PAINTSTRUCT ps;
	case WM_INITDIALOG:
		//cboRespawn = GetDlgItem(hWnd,IDC_RESPAWN_TIME);
		for (int i = 0; i < 3; i++)
		{
			SendDlgItemMessage(hWnd, IDC_BUFF_TIME, CB_ADDSTRING, i, (LPARAM)respawn[i]);
			SendDlgItemMessage(hWnd, IDC_RESPAWN_TIME, CB_ADDSTRING, i, (LPARAM)respawn[i]);
			if (i == 1)
			{
				SendDlgItemMessage(hWnd, IDC_BUFF_TIME,CB_SETCURSEL, i, (LPARAM)respawn[i]);
				SendDlgItemMessage(hWnd, IDC_RESPAWN_TIME,CB_SETCURSEL, i, (LPARAM)respawn[i]);
			}
		}
		for (int i = MIN_OBJECTS; i <= MAX_OBJECTS; i++)
		{
			_sntprintf_s(aux, _TRUNCATE, TEXT("%d"), i);
			SendDlgItemMessage(hWnd, IDC_N_ITEMS,CB_ADDSTRING,(WPARAM) i, (LPARAM)aux);
			if (i == 15)		
				SendDlgItemMessage(hWnd, IDC_N_ITEMS, CB_SELECTSTRING,(WPARAM) i, (LPARAM)aux);
		}
		for (int i = MIN_INITIAL_SNAKE_SEGMENTS; i <= MAX_INITIAL_SNAKE_SEGMENTS; i++)
		{
			_sntprintf_s(aux, _TRUNCATE,TEXT("%d"),i);
			SendDlgItemMessage(hWnd, IDC_INIT_SNAKE, CB_ADDSTRING, i, (LPARAM)aux);
			if(i == MIN_INITIAL_SNAKE_SEGMENTS)
				SendDlgItemMessage(hWnd, IDC_INIT_SNAKE, CB_SELECTSTRING, i, (LPARAM)aux);
		}
		for (int i = MIN_SNAKE_BOTS; i <= MAX_SNAKE_BOTS; i++)
		{
			_sntprintf_s(aux, _TRUNCATE, TEXT("%d"), i);
			SendDlgItemMessage(hWnd, IDC_N_BOTS, CB_ADDSTRING, i, (LPARAM)aux);
			SendDlgItemMessage(hWnd, IDC_N_PLAYERS, CB_ADDSTRING, i, (LPARAM)aux);
			if (i == 10)
			{
				SendDlgItemMessage(hWnd, IDC_N_BOTS, CB_SELECTSTRING, i, (LPARAM)aux);
			}
		}
		//SendDlgItemMessage(hWnd, IDC_INIT_SNAKE, CB_ADDSTRING, 0, (LPARAM)TEXT("10"));

		//Sliders
		SendDlgItemMessage(hWnd, IDC_MAP_X, TBM_SETRANGE,(WPARAM)1, (LPARAM)MAKELONG(MIN_FIELD_X, MAX_FIELD_X));
		SendDlgItemMessage(hWnd, IDC_MAP_Y, TBM_SETRANGE,(WPARAM)1, (LPARAM)MAKELONG(MIN_FIELD_Y, MAX_FIELD_Y));
		//SendDlgItemMessage(hWnd, IDC_MAP_X, TBM_SETRANGE, (WPARAM)1, (LPARAM)40);
		SendMessage(GetDlgItem(hWnd, IDC_MAP_X), TBM_SETPOS, (WPARAM)1, (LPARAM)60);
		SendMessage(GetDlgItem(hWnd, IDC_MAP_Y), TBM_SETPOS, (WPARAM)1, (LPARAM)40);
		return TRUE;
	case WM_CREATE:
		break;
	case WM_CLOSE:
		//DestroyWindow(hWnd);
		if (MessageBox(hWnd, TEXT("Do you want to exit from Configuration?"), TEXT("Warning"), MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
		{
			config.giveup = 1;
			if (kindConnection == 1)
				clientSetConfigSM(config);
			//else

			// NAMED PIPE 
			PostQuitMessage(0);
		}
		break;
	case WM_PAINT:
		//CreateConfigLabels(hWnd);
		//SetBkColor((HDC)hWnd, TRANSPARENT);
		break;
	case WM_LBUTTONUP:
		//_stprintf_s(str, 100, TEXT("Botão esquerdo largado: %d, %d"), LOWORD(lParam), HIWORD(lParam));
		//MessageBox(hWnd, str, TEXT("Evento do Ratro"), MB_OK);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == ID_COMMIT_CONFIG)
			if (HIWORD(wParam) == BN_CLICKED)
			{
				config.giveup = 0;
				coop = 0;
				config.coop = 0;
				GetDlgItemText(hWnd, IDC_PLAYER_NAME,config.username1,MAX_TAM);
				if (_tcslen(config.username1) <= 2)
					_tcsncpy_s(config.username1, MAX_TAM, TEXT("0"), _TRUNCATE);
				config.numObjects = SendMessage(GetDlgItem(hWnd, IDC_N_ITEMS), (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
				checked = IsDlgButtonChecked(hWnd, IDC_COOP);
				if (checked)
				{
					coop = 1;
					config.coop = 1;
					GetDlgItemText(hWnd, IDC_PLAYER_2_NAME, config.username2, MAX_TAM);
					if (_tcslen(config.username2) <= 2)
						_tcsncpy_s(config.username2,MAX_TAM,TEXT("0"), _TRUNCATE);
				}
				config.numObjects += MIN_OBJECTS;
				config.temporaryEffectTime = SendMessage(GetDlgItem(hWnd, IDC_BUFF_TIME), (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0) + 1;
				config.itemsFrequency = SendMessage(GetDlgItem(hWnd, IDC_RESPAWN_TIME), (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0) + 1;
				config.snakeInitialSegmentSize = SendMessage(GetDlgItem(hWnd, IDC_INIT_SNAKE), (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
				config.snakeInitialSegmentSize += MIN_INITIAL_SNAKE_SEGMENTS;
				config.numSnakeBots = SendMessage(GetDlgItem(hWnd, IDC_N_BOTS), (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
				config.numSnakeBots += MIN_SNAKE_BOTS;
				config.fieldSizeX = SendMessage(GetDlgItem(hWnd, IDC_MAP_X), (UINT)TBM_GETPOS, (WPARAM)0, (LPARAM)0);
				config.fieldSizeY = SendMessage(GetDlgItem(hWnd, IDC_MAP_Y), (UINT)TBM_GETPOS, (WPARAM)0, (LPARAM)0);
				config.giveup = 0;
				checked = IsDlgButtonChecked(hWnd, IDC_ONLINE);
				
				if (checked)
				{
					config.isOnline = 1;
					config.numMaxPlayers = 1 + SendMessage(GetDlgItem(hWnd, IDC_N_PLAYERS), (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
				}
				else
				{
					config.isOnline = 0;
					config.numMaxPlayers = 0;
				}

				checked = IsDlgButtonChecked(hWnd, IDC_WITH_OBST);
				if (checked)
					config.withObjects = 1;
				else
					config.withObjects = 0;

				checked = IsDlgButtonChecked(hWnd, IDC_CHANGE_KEYS);
				if (checked)
				{
					cliKeys[0].keyUp = (WORD)SendDlgItemMessage(hWnd, IDC_P1_KEY1, HKM_GETHOTKEY, 0, 0);
					cliKeys[0].keyDown = (WORD)SendDlgItemMessage(hWnd, IDC_P1_KEY2, HKM_GETHOTKEY, 0, 0);
					cliKeys[0].keyRight = (WORD)SendDlgItemMessage(hWnd, IDC_P1_KEY3, HKM_GETHOTKEY, 0, 0);
					cliKeys[0].keyLeft = (WORD)SendDlgItemMessage(hWnd, IDC_P1_KEY4, HKM_GETHOTKEY, 0, 0);
					if (coop)
					{
						cliKeys[1].keyUp = (WORD)SendDlgItemMessage(hWnd, IDC_P2_KEY1, HKM_GETHOTKEY, 0, 0);
						cliKeys[1].keyDown = (WORD)SendDlgItemMessage(hWnd, IDC_P2_KEY2, HKM_GETHOTKEY, 0, 0);
						cliKeys[1].keyRight = (WORD)SendDlgItemMessage(hWnd, IDC_P2_KEY3, HKM_GETHOTKEY, 0, 0);
						cliKeys[1].keyLeft = (WORD)SendDlgItemMessage(hWnd, IDC_P2_KEY4, HKM_GETHOTKEY, 0, 0);
					}
				}
				//_sntprintf_s(debbug, _TRUNCATE, TEXT("Name: %s, Num Obj: %d, Buff Time: %d\n Map Size:(%d,%d)"),
				//	config.username1, config.numObjects, config.temporaryEffectTime,config.fieldSizeX,config.fieldSizeY);
				//MessageBox(hWnd, debbug,TEXT("Debbug"), MB_OK | MB_ICONINFORMATION);
				clientSetConfigSM(config);
				ret = getAcceptConfigSM(10000, &timeout, aux);
				if (timeout == 1) 
				{
					MessageBox(hWnd, TEXT("Server doesn't answered\n"), TEXT("Disconnected"), MB_OK | MB_ICONINFORMATION);
					PostQuitMessage(0);
				}
				if (ret == -2)
				{
					MessageBox(hWnd, TEXT("Server Turned Off\n"), TEXT("Disconnected"), MB_OK | MB_ICONINFORMATION);
					PostQuitMessage(0);
				}
				else if (ret == 0)
					MessageBox(hWnd,aux, TEXT("Invalid Configuration"), MB_OK | MB_ICONINFORMATION);
				else if (config.isOnline)
				{

				}
				else
				{
					_tcsncpy_s(cliKeys[0].username, MAX_TAM,config.username1, _TRUNCATE);
					_tcsncpy_s(cliKeys[1].username, MAX_TAM,config.username2, _TRUNCATE);
					canLaunchGame = 1;
					EndDialog(hWnd, wParam);
				}
			}
		break;
	}
	return(0);
}

LRESULT CALLBACK RunListener(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int retval,st;
	//BOOL checked;
	const int SIZE = 100;
	TCHAR ip[SIZE], username[SIZE], password[SIZE],errorLog[SIZE];

	switch (msg)
	{

	case WM_CREATE:

		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == ID_BT_LOCAL)
		{
			retval = clientOpenAccessSM(&st);
			//_stprintf_s(errorLog, _TRUNCATE, TEXT("State: %d"), state);
			//MessageBox(hWnd, errorLog, TEXT("STATE"), MB_OK | MB_ICONEXCLAMATION);
			switch (retval)
			{
			case 1:
				if (st == 0)
				{
					canLaunchConfig = 1; // state 1
					kindConnection = 1; // SM
					EndDialog(hWnd, wParam);
				}
				else if (st == 2)
				{
					canLaunchAwaitPlayers = 1; // state 2
					kindConnection = 1; // SM
					EndDialog(hWnd, wParam);
				}
				break;
			case 0:
			case -1:
				MessageBox(hWnd, TEXT("Connection Failed to Local Server.\nServer is not Online."),
					TEXT("Server Offline"), MB_OK | MB_ICONEXCLAMATION);
				break;
			case -2:
				MessageBox(hWnd, TEXT("Other Player Already Connected through Shared Memory."),
					TEXT("Server Online"), MB_OK | MB_ICONEXCLAMATION);
				break;
			case -3:
				MessageBox(hWnd, TEXT("Other Player is configuring game."),
					TEXT("Server Online"), MB_OK | MB_ICONEXCLAMATION);
				break;
			case -4:
				MessageBox(hWnd, TEXT("Game is already running"),
					TEXT("Server Online"), MB_OK | MB_ICONEXCLAMATION);
				break;
			}				
		}
		else if (LOWORD(wParam) == ID_BT_REMOTE)
		{
			
			GetDlgItemText(hWnd, IDC_IP, ip, SIZE);
			GetDlgItemText(hWnd, IDC_LOG_ON_USER, username, SIZE);
			GetDlgItemText(hWnd, IDC_LOG_ON_PASS, password, SIZE);
			retval = clientOpenAccessNP(ip,username,password,errorLog);
			
			// DEBBUG
			//retval = clientOpenAccessNP(TEXT("192.168.43.207"),TEXT("rafa"),TEXT("++132Rafa19956++"), errorLog);
			switch (retval)
			{
			case 1:
				canLaunchConfig = 1; // state 1
				kindConnection = 2; // NP
				EndDialog(hWnd, wParam);
				MessageBox(hWnd, errorLog, TEXT("State"), MB_OK | MB_ICONEXCLAMATION);
				break;
			case 2:
				canLaunchAwaitPlayers = 1; // state 2
				kindConnection = 2; // NP
				EndDialog(hWnd, wParam);
				MessageBox(hWnd, errorLog, TEXT("State"), MB_OK | MB_ICONEXCLAMATION);
				break;
			default:
				MessageBox(hWnd, errorLog, TEXT("Connection Failed"), MB_OK | MB_ICONEXCLAMATION);
				break;
			}
		}
		else if (LOWORD(wParam) == ID_BT_CHANGE_BMPS)
		{
			canLaunchChangePicture = 1;
			EndDialog(hWnd, wParam);
		}
		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	}
	return(0);
}

LRESULT CALLBACK JoinListener(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	DWORD timeout;
	BOOL checked;
	int ret;
	TCHAR aux[MAX_TAM];
	switch (msg)
	{

	case WM_CREATE:
		coop = 0;
		config.coop = 0;
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == ID_JOIN_GAME)
		{
			GetDlgItemText(hWnd, IDC_P1_JOIN_NAME, config.username1, MAX_TAM);
			if (_tcslen(config.username1) <= 2)
				_tcsncpy_s(config.username1, MAX_TAM, TEXT("0"), _TRUNCATE);
			checked = IsDlgButtonChecked(hWnd, IDC_COOP);
			if (checked)
			{
				coop = 1;
				config.coop = 1;
				GetDlgItemText(hWnd, IDC_PLAYER_2_NAME, config.username2, MAX_TAM);
				if (_tcslen(config.username2) <= 2)
					_tcsncpy_s(config.username2, MAX_TAM, TEXT("0"), _TRUNCATE);
			}
			if (kindConnection == 1)
			{
				clientSetConfigSM(config);
				ret = getAcceptConfigSM(10000, &timeout, aux);
				if (ret == -2)
				{
					MessageBox(hWnd, TEXT("Server will disconnect you."),
						TEXT("Information"), MB_OK);
					PostQuitMessage(2);
				}
				else if (ret == 1)
				{
					MessageBox(hWnd,aux, TEXT("Invalid Configuration"), MB_OK);
					break;
				}
				canLaunchGame = 1;
				EndDialog(hWnd, wParam);
			}
		}
		break;
	case WM_CLOSE:
		if (MessageBox(hWnd, TEXT("Do you really want to exit game?"),
			TEXT("Warning"), MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
		{
			cirBufInSM(30000, &timeout, cliKeys[0].username, 1, 1);
			if (coop)
				cirBufInSM(30000, &timeout, cliKeys[1].username, 1, 1);
			turnOffCli = 1;
			PostQuitMessage(0);
		}
	}
	return(0);
}

LRESULT CALLBACK AwaitPlayersListener(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//TCHAR aux[MAX_TAM];
	switch (msg)
	{

	case WM_CREATE:

		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == ID_START_ONLINE)
		{
			if (kindConnection == 1)
			{
				config.giveup = 0;
				clientSetConfigSM(config);
				canLaunchGame = 1;
				EndDialog(hWnd, wParam);
			}
		}
		break;
	case WM_CLOSE:
		if (MessageBox(hWnd, TEXT("Do you really want to exit game?"),
			TEXT("Warning"), MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
		{
			config.giveup = 1;
			clientSetConfigSM(config);
			turnOffCli = 1;
			PostQuitMessage(0);
		}
	}
	return(0);
}

LRESULT CALLBACK ChangePictureListener(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC device;
	PAINTSTRUCT pt;
	int curPicIndex = 0;
	const int numPic = 10;
	const TCHAR *pictures[] = { TEXT("Food"),TEXT("Ice"),TEXT("Grenade"),TEXT("Vodka"),TEXT("Oil"),
		TEXT("Glue"),TEXT("O-Vodka"),TEXT("O-Oil"),TEXT("O-Glue"), TEXT("Hoe") };
	switch (msg)
	{
	case WM_INITDIALOG:
		for(int i = 0; i < numPic; i++)
			SendDlgItemMessage(hWnd,IDC_BMP_OPTIONS, CB_ADDSTRING, i, (LPARAM)pictures[i]);
		SendDlgItemMessage(hWnd, IDC_BMP_OPTIONS, CB_SETCURSEL, 0, (LPARAM)pictures[0]);
		break;
	case WM_PAINT:
		device = BeginPaint(hWnd, &pt);
		drawImageInBitmapEditor(device, 20, 40, curPicIndex);
		EndPaint(hWnd, &pt);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == ID_BACK)
		{
			canLaunchRun = 1;
			EndDialog(hWnd, wParam);
		}
		else if (LOWORD(wParam) == ID_EDIT_BMP)
		{
			curPicIndex = SendMessage(GetDlgItem(hWnd, IDC_BMP_OPTIONS), (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
			editResourceOnPaint(curPicIndex,hWnd);
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_CLOSE:
		if (MessageBox(hWnd, TEXT("Do you really want to exit bitmap editing?"),
			TEXT("Warning"), MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
		{
			PostQuitMessage(0);
		}
	}
	//InvalidateRect(hWnd, NULL, TRUE);
	return(0);
}

void drawImageInBitmapEditor(HDC d,int locx, int locy, int index)
{
	HDC hdcMem;
	hdcMem = CreateCompatibleDC(d);
	SelectObject(hdcMem, hBmp[0]);
	BitBlt(d, locx, locy, SQUARE_SIZE_X, SQUARE_SIZE_Y, hdcMem, 0, 0, SRCCOPY);
	DeleteDC(hdcMem);
}