#include "server.h"
#include "algorithms.h"

void printConfigStruct()
{
	_tprintf(TEXT("Username1: %s\n"), config.username1);
	if (_tcsncmp(config.username2, TEXT("0"), sizeof(TCHAR) * _tcslen(config.username2)))
		_tprintf(TEXT("Username2: %s\n"), config.username2);
	_tprintf(TEXT("fieldSizeX: %d\n"), config.fieldSizeX);
	_tprintf(TEXT("fieldSizeY: %d\n"), config.fieldSizeY);
	_tprintf(TEXT("snakeInitialSegmentSize: %d\n"), config.snakeInitialSegmentSize);
	_tprintf(TEXT("numSnakeBots: %d\n"), config.numSnakeBots);
	_tprintf(TEXT("numMaxPlayers: %d\n"), config.numMaxPlayers);
	_tprintf(TEXT("itemFrequency: %d\n"), config.itemsFrequency);
	_tprintf(TEXT("numObjects: %d\n"), config.numObjects);
	_tprintf(TEXT("temporaryEffectTime: %d\n"), config.temporaryEffectTime);
	_tprintf(TEXT("withObjects: %d\n"), config.withObjects);
	_tprintf(TEXT("coop: %d\n"), config.coop);
	_tprintf(TEXT("giveup: %d\n\n\n"), config.giveup);
}

int gameConfig()
{
	DWORD timeout;
	TCHAR acceptConfig[MAX_TAM] = TEXT("0");
	int configOK = 0;
	updateState(0);
	_tprintf(TEXT("[AWAIT] Await by someone to configure game... (State: %d)\n"), state.state);
	do
	{
		config = retrieveConfigSM(30000, &timeout);// Read game configuration from named pipe
		if (timeout == 1)
		{
			if (info.serverTurnedOff == 1)
			{
				_tprintf(TEXT("[CONFIG] SERVER TURNED OFF\n"));
				return -1;
			}
			continue;
		}
		else if (config.giveup == 2 && state.state == 0)
		{
			updateState(1);
			_tprintf(TEXT("[CONFIG] Client Connected to Configure...(State: %d)\n"), state.state);
			continue;
		}
		else if (config.giveup == 2 && state.state == 1) { continue; }
		_tprintf(TEXT("[CONFIG] Receiving Client Configuration...(State: %d)\n"), state.state);
		printConfigStruct();// Print Config Struct from Shared Memory
		if (config.giveup == 1 && state.state == 1)
		{
			_tprintf(TEXT("[CONFIG] Client Disconnected from Config...(State: %d)\n"), state.state);
			updateState(0);
			continue;
		}

		if (!_tcsncmp(config.username1, TEXT("0"), _tcslen(config.username1)) || _tcslen(config.username1) < 2)
			_tcsncpy_s(acceptConfig, MAX_TAM, TEXT("Invalid Username."), _TRUNCATE);
		else if (config.fieldSizeX < MIN_FIELD_X || config.fieldSizeX > MAX_FIELD_X)
			_tcsncpy_s(acceptConfig, MAX_TAM, TEXT("Invalid X Field Size."), _TRUNCATE);
		else if (config.fieldSizeY < MIN_FIELD_Y || config.fieldSizeY > MAX_FIELD_Y)
			_tcsncpy_s(acceptConfig, MAX_TAM, TEXT("Invalid Y Field Size."), _TRUNCATE);
		else if (config.snakeInitialSegmentSize < MIN_INITIAL_SNAKE_SEGMENTS ||
			config.snakeInitialSegmentSize > MAX_INITIAL_SNAKE_SEGMENTS)
			_tcsncpy_s(acceptConfig, MAX_TAM, TEXT("Invalid Initial Snake Segments."), _TRUNCATE);
		else if (config.numSnakeBots < MIN_SNAKE_BOTS || config.numSnakeBots > MAX_SNAKE_BOTS)
			_tcsncpy_s(acceptConfig, MAX_TAM, TEXT("Invalid Number of Snake Bots."), _TRUNCATE);
		else if (config.numMaxPlayers < 0 || config.numMaxPlayers > MAX_PLAYERS)
			_tcsncpy_s(acceptConfig, MAX_TAM, TEXT("Invalid Number of Players able to Join game."), _TRUNCATE);
		else if (config.itemsFrequency < MIN_ITEMS_FREQUENCY || config.itemsFrequency > MAX_ITEMS_FREQUENCY)
			_tcsncpy_s(acceptConfig, MAX_TAM, TEXT("Invalid Item Frequency."), _TRUNCATE);
		else if (config.numObjects < MIN_OBJECTS || config.numObjects > MAX_OBJECTS)
			_tcsncpy_s(acceptConfig, MAX_TAM, TEXT("Invalid Number of Objects."), _TRUNCATE);
		else if (config.temporaryEffectTime < MIN_EFFECT_TIME || config.temporaryEffectTime > MAX_EFFECT_TIME)
			_tcsncpy_s(acceptConfig, MAX_TAM, TEXT("Invalid Items Effect Time."), _TRUNCATE);
		else if (config.isOnline < 0 || config.isOnline > 1)
			_tcsncpy_s(acceptConfig, MAX_TAM, TEXT("Invalid Online Var."), _TRUNCATE);
		//else if (config.withObjects < 0 || config.withObjects > 1)
		//	_tcsncpy_s(acceptConfig, MAX_TAM, TEXT("Invalid obstacles Number."), _TRUNCATE);
		else if (config.coop == 1)
		{
			if (!_tcsncmp(config.username2, TEXT("0"), _tcslen(config.username2)) || _tcslen(config.username2) < 2)
				_tcsncpy_s(acceptConfig, MAX_TAM, TEXT("Invalid Username 2."), _TRUNCATE);
			else configOK = 1;
		}
		else configOK = 1;
		if (configOK == 0) updateAcceptConfigSM(acceptConfig);
		_tprintf(TEXT("[CONFIG] Answer Client Configuration... (%s) (State: %d)\n"), acceptConfig, state.state);
	} while (configOK == 0);
	// Copy config values to render
	_tcsncpy_s(info.snake[0].username, MAX_TAM, config.username1, _TRUNCATE);
	(info.numPlayers)++;
	if (config.coop == 1)
	{
		_tcsncpy_s(info.snake[1].username, MAX_TAM, config.username2, _TRUNCATE);
		(info.numPlayers)++;
	}
	info.field.sizeX = config.fieldSizeX;
	info.field.sizeY = config.fieldSizeY;
	info.numBots = config.numSnakeBots;
	info.isOnline = config.isOnline;
	info.clientsOnline++;
	//This fuctions must be called after all players had joined
	// to the game
	if (info.isOnline != 1)
	{
		setUpRandomDirAndCoords();
		updateRenderSM(info);
		_tcsncpy_s(gameCreator, MAX_TAM, config.username1, _TRUNCATE);
	}
	_tcsncpy_s(acceptConfig, MAX_TAM, TEXT("0"), _TRUNCATE);
	updateAcceptConfigSM(acceptConfig);
	_tprintf(TEXT("[CONFIG] Configuration Accept.\n"));
	return 1;
}

void setUpRandomDirAndCoords()
{
	int maxx = info.field.sizeX - 3;
	int minx = 3;
	int maxy = info.field.sizeY - 3;
	int miny = 3;
	int toaddx, toaddy;

	for (int i = 0; i < (info.numPlayers + info.numBots); i++)
	{
		info.snake[i].died = 0;
		info.snake[i].points = 0;
		info.snake[i].currentDirection = 1;
		info.snake[i].glue = 0;
		info.snake[i].oil = 0;
		info.snake[i].invertKeys = 0;
	snakesOverlapped:
		toaddx = RangedRandDemo(minx, maxx);
		toaddy = RangedRandDemo(miny, maxy);
		for (int s = 0; s < config.snakeInitialSegmentSize; s++)
		{
			info.snake[i].coords[s][0] = toaddx;
			info.snake[i].coords[s][1] = toaddy++;
			info.snake[i].snakeSize++;
			// If new snake is overlapping another one
			for (int created = 0; created < i; created++)
			{
				for (int dot = 0; dot < config.snakeInitialSegmentSize; dot++)
					if (info.snake[i].coords[s][0] == info.snake[created].coords[dot][0] &&
						info.snake[i].coords[s][1] == info.snake[created].coords[dot][1])
						goto snakesOverlapped;
			}

		}
		miny = 3 + i + 1;
		minx++;
	}
	for (int i = 0; i < config.numObjects; i++)
	{
		info.field.numObjs++;
		updateNewObj(i, randomObj());
	}

	for (int i = 0; i < MAX_TAM; i++) requestKey[i] = 0; // there is no request keys
}

BOOL WINAPI ConsoleHandlerRoutine(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
	case CTRL_CLOSE_EVENT:
		_tprintf(TEXT("Server turned off...\n"));
		switch (state.state)
		{
		case 0:
			_tprintf(TEXT("Server was in state 0...\n"));
			info.serverTurnedOff = 1;
			break;
		case 1:
			_tprintf(TEXT("Server was in state 1...\n"));
			info.serverTurnedOff = 1;
			updateAcceptConfigSM(TEXT("Close"));
			break;
		case 2:
			_tprintf(TEXT("Server was in state 2...\n"));
			info.serverTurnedOff = 1;
			break;
		case 3:
			_tprintf(TEXT("Server was in state 3...\n"));
			info.serverTurnedOff = 1;
			updateRenderSM(info);
			break;
		}
		closeSM();
		breakMainCicle = 1;
		_gettch();
		//Sleep(1000);
		return TRUE;
	}
	return FALSE;
}

void updateState(int st)
{
	state = getStateServer();
	switch (st)
	{
	case 0:
		state.state = 0;
		state.cliOnThoughtNP = 0;
		state.cliOnThoughtSM = 0;
		break;
	default:
		state.state = st;
		break;
	}

	setStateServer(state);
}

void updateCliOn(int moreNP, int moreSM)
{
	getStateServer();
	state.cliOnThoughtSM += moreSM;
	state.cliOnThoughtNP += moreNP;
}

int playersJoinSM_state2()
{
	DWORD timeout;
	int configOK = 0;
	TCHAR acceptConfig[MAX_TAM] = TEXT("0");
	_tprintf(TEXT("[AWAIT JOIN] Await by someone to configure game... (State: %d)\n"), state.state);
	do
	{
		if (configOK == 1) configOK = 0;
		_tcsncpy_s(acceptConfig, MAX_TAM, TEXT("0"), _TRUNCATE);
		config = retrieveConfigSM(30000, &timeout);// Read game configuration from named pipe
		if (timeout == 1)
		{
			if (info.serverTurnedOff == 1)
			{
				_tprintf(TEXT("[AWAIT JOIN] SERVER TURNED OFF (State: %d)\n"), state.state);
				return -1;
			}
			continue;
		}
		if (config.giveup != 1 && !_tcsncmp(info.snake[0].username, config.username1, _tcslen(config.username1)))
		{
			break;
		}
		// Someone want to disconnect
		else if (config.giveup == 1)
		{
			for (int i = 0; i < info.numPlayers; i++)
			{
				if (!_tcsncmp(info.snake[i].username, config.username1, _tcslen(config.username1)))
				{
					for (int j = (i + 1); j < info.numPlayers; j++)
					{
						if (j < info.numPlayers)
							info.snake[j - 1] = info.snake[j];
					}
					(info.numPlayers)--;
					info.clientsOnline--; // we are sure if player 1 exists, clientsOnline can be decremented
				}
				if (!_tcsncmp(info.snake[i].username, config.username2, _tcslen(config.username2)))
				{
					for (int j = (i + 1); j < info.numPlayers; j++)
					{
						if (j < info.numPlayers)
							info.snake[j - 1] = info.snake[j];
					}
					(info.numPlayers)--;
				}
			}
		}
		else if (config.numMaxPlayers >= info.numPlayers + info.numBots)
		{
			_tcsncpy_s(acceptConfig, MAX_TAM, TEXT("Start"), _TRUNCATE);
			break;
		}
		// Someone wants to connect
		else
		{
			if (!_tcsncmp(config.username1, TEXT("0"), _tcslen(config.username1)) || _tcslen(config.username1) < 2)
				_tcsncpy_s(acceptConfig, MAX_TAM, TEXT("Invalid Username."), _TRUNCATE);
			if (config.coop)
			{
				if (!_tcsncmp(config.username2, TEXT("0"), _tcslen(config.username2)) || _tcslen(config.username2) < 2)
					_tcsncpy_s(acceptConfig, MAX_TAM, TEXT("Invalid Username 2."), _TRUNCATE);
			}
			updateAcceptConfigSM(acceptConfig);
			_tcsncpy_s(info.snake[info.numPlayers].username, MAX_TAM, config.username1, _TRUNCATE);
			(info.numPlayers)++;
			_tprintf(TEXT("[AWAIT JOIN] Player Snake %d has Joined Game %s.\n"), info.numPlayers, config.username1);
			if (config.coop == 1)
			{
				_tcsncpy_s(info.snake[info.numPlayers].username, MAX_TAM, config.username2, _TRUNCATE);
				(info.numPlayers)++;
				_tprintf(TEXT("[AWAIT JOIN] Player Snake %d has Joined Game %s.\n"),
					info.numPlayers, info.snake[info.numPlayers - 1].username);
			}
		}
	} while (configOK == 0);
	updateState(3);
	setUpRandomDirAndCoords();
	updateRenderSM(info);
	_tcsncpy_s(acceptConfig, MAX_TAM, TEXT("0"), _TRUNCATE);
	updateAcceptConfigSM(acceptConfig);
	_tprintf(TEXT("[AWAIT JOIN] Game Started.\n"));
	return 1;
}

int init()
{
	srand((unsigned)time(NULL));
	DWORD recvThreadReturn;


#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif
	_tprintf(TEXT("Server turned on...\n"));
	for (int i = 0; i < MAX_PLAYERS; i++)
		hReadCli[i] = INVALID_HANDLE_VALUE;
	info.numPlayers = 0;
	if (SetUpRegistry(info) == -1) { _gettch(); return 1; }

	// Error setting up Shared Memory
	if (!serverCreateSM())
	{
		_tprintf(TEXT("Error Setting up Shared Memory... (%d)\n"), GetLastError());
		_gettch();
		return 1;
	}

	if (FALSE == SetConsoleCtrlHandler(ConsoleHandlerRoutine, TRUE))
	{
		_tprintf(TEXT("Error Setting up Console Ctrl Routine... (%d)\n"), GetLastError());
		_gettch();
		return 1;
	}
	if (setUpSyncs() != 0) { _gettch(); return 1; }
	breakMainCicle = 0;

	do
	{
		ZeroMemory(&info, sizeof(Info));
		//info = { 0 };
		info.serverTurnedOff = 0;
		info.clientsOnline = 0;
		info.gameEnded = 0;
		gameRunning = 0;
		updateState(0);
		_tprintf(TEXT("[AWAIT] Setting up named pipe...\n"));
		hNewNPCliThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)recvNewCliConnection, NULL, 0, NULL);
		gameConfig();
		if (info.isOnline == 1)
		{
			updateState(2);
			playersJoinSM_state2();
		}
		gameRunning = 1;
		info.gameEnded = 0;
		updateState(3);
		if (setUpSyncs() != 0) { _gettch(); return 1; }
		if (setUpThreads_State3() != 0) { _gettch(); return 1; }

		WaitForSingleObject(hRecvFromCli, INFINITE);
		updateState(0);
		GetExitCodeThread(hRecvFromCli, &recvThreadReturn);
		WaitForSingleObject(hSendCli, INFINITE);
		WaitForMultipleObjects(info.field.numObjs, hObjThread, TRUE, INFINITE);
		WaitForMultipleObjects(info.numPlayers, hPlayerThread, TRUE, INFINITE);
		for (int i = 0; i < info.numBots; i++)
			WaitForSingleObject(botParam[i].hThread, INFINITE);
		closeSM();
		closeThreadHandles_State3();
		closeSyncs();
		if (recvThreadReturn == 2) _tprintf(TEXT("[GAME] All Clients exited game...\n"));
		else _tprintf(TEXT("[GAME] Game finished...\n"));
		ZeroMemory(&config, sizeof(config));
		//config = { 0 };
		for (int i = 0; i < info.numPlayers; i++)
			updateRegistry(info.snake[i]);
		printRegistry();
		if (!serverCreateSM())
		{
			_tprintf(TEXT("Error Setting up Shared Memory... (%d)\n"), GetLastError());
			_gettch();
			return 1;
		}
	} while (breakMainCicle == 0);
	closeSM();
	_tprintf(TEXT("[TURN OFF] All Handles and Syncs Closed...\n"));
	return 0;
}

int setUpThreads_State3()
{
	//DWORD threadID, threadID2;
	_tprintf(TEXT("Setting up bots...\n"));
	for (int i = 0; i < info.numBots; i++)
	{
		botParam[i].botIndex = info.numPlayers + i;
		botParam[i].hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)botsIA, &botParam[i], 0, NULL);
		if (botParam[i].hThread == NULL)
		{
			_tprintf(TEXT("Error Creating Bots Threads (%d)"), GetLastError());
			return 1;
		}
		else CloseHandle(botParam[i].hThread);
	}

	_tprintf(TEXT("Setting Communication Game Threads...\n"));
	hRecvFromCli = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)recvFromCli, NULL, 0, NULL);
	if (hRecvFromCli == NULL)
	{
		_tprintf(TEXT("Error Creating hRecvFromCli Thread (%d)"), GetLastError());
		return 1;
	}

	hSendToCli = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)sendToCli, NULL, 0, NULL);
	if (hSendToCli == NULL)
	{
		_tprintf(TEXT("Error Creating hSendToCli Thread (%d)"), GetLastError());
		return 1;
	}
	//else CloseHandle(hSendToCli);

	_tprintf(TEXT("Setting objects...\n"));
	for (int i = 0; i < info.field.numObjs; i++)
	{
		objID[i] = i;
		hObjThread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)objectRespawn, &objID[i], 0, NULL);
		if (hObjThread[i] == NULL)
		{
			_tprintf(TEXT("Error Creating Objects Threads (%d)"), GetLastError());
			return 1;
		}
		//else CloseHandle(hObjThread[i]);
	}
	for (int i = 0; i < info.numPlayers; i++)
	{
		playerIndex[i] = i;
		hPlayerThread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)playerSnakeThread, &playerIndex[i], 0, NULL);
		if (hPlayerThread[i] == NULL)
		{
			_tprintf(TEXT("Error Creating Player Thread %d (%d)"), i, GetLastError());
			return 1;
		}
		//else CloseHandle(hPlayerThread[i]);
	}
	return 0;
}

void closeThreadHandles_State3()
{
	CloseHandle(hRecvFromCli);
	CloseHandle(hSendCli);
	for (int i = 0; i < info.numPlayers; i++)
		CloseHandle(hPlayerThread[i]);
	for (int i = 0; i < info.numBots; i++)
		CloseHandle(botParam[i].hThread);
}

int setUpSyncs()
{
	hMutexObjs = CreateMutex(NULL, TRUE, NULL);
	if (hMutexObjs == NULL)
	{
		_tprintf(TEXT("Error Creating Mutex Obj (%d)"), GetLastError());
		return 1;
	}
	hSnakeMutex = CreateMutex(NULL, FALSE, NULL);
	if (hSnakeMutex == NULL)
	{
		_tprintf(TEXT("Error Creating Mutex Snakes (%d)"), GetLastError());
		CloseHandle(hMutexObjs);
		return 1;
	}
	hEventSendInfo = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (hEventSendInfo == NULL)
	{
		_tprintf(TEXT("Error Creating Game Event (%d)"), GetLastError());
		CloseHandle(hMutexObjs);
		CloseHandle(hSnakeMutex);
		return 1;
	}
	return 0;
}

void closeSyncs()
{
	CloseHandle(hMutexObjs);
	CloseHandle(hSnakeMutex);
	CloseHandle(hEventSendInfo);
}

DWORD WINAPI botsIA(LPVOID *param)
{
	ClientInput botIn;
	BotThread *bt = (BotThread *)param;
	DWORD timeout;
	int botIndex = bt->botIndex;
	_stprintf_s(info.snake[botIndex].username, _TRUNCATE, TEXT("Bot%d"),
		botIndex + 1 - info.numPlayers);
	_tcsncpy_s(botIn.username, MAX_TAM, info.snake[botIndex].username, _TRUNCATE);
	int key, sum, speed = SNAKE_SPEED_TIME;

	do
	{
		speed = SNAKE_SPEED_TIME;
		if (info.snake[botIndex].glue > 0 && info.snake[0].oil > 0)
		{
			info.snake[botIndex].glue--;
			info.snake[botIndex].oil--;
		}
		else if (info.snake[botIndex].glue > 0)
		{
			speed = SNAKE_SPEED_TIME * 4;
			info.snake[botIndex].glue--;
		}
		else if (info.snake[botIndex].oil > 0)
		{
			speed = SNAKE_SPEED_TIME / 4;
			info.snake[botIndex].oil--;
		}
		Sleep(speed);
		key = avoidAgainstWall(info.snake[botIndex].coords[0][0],
			info.snake[botIndex].coords[0][1], info.snake[botIndex].currentDirection,
			info.field.sizeX, info.field.sizeY);
		if (key == 0)
		{
			key = RangedRandDemo(0, 100);
			if (key < 35)
				key = RangedRandDemo(1, 4);
			else
				key = info.snake[botIndex].currentDirection;
		}
		timeout = WaitForSingleObject(hSnakeMutex, 200);
		if (timeout == WAIT_TIMEOUT) continue;
		moveSnake(key, botIndex);
		SetEvent(hEventSendInfo);
		ReleaseMutex(hSnakeMutex);
	} while (gameRunning == 1 && !info.snake[botIndex].died && info.gameEnded == 0 && info.serverTurnedOff == 0);
	if (info.snake[botIndex].died)
		_tprintf(TEXT("[GAME] Bot %d died Index: %d\n"), botIndex - info.numPlayers, botIndex);
	return 0;
}

DWORD WINAPI playerSnakeThread(LPVOID *param)
{
	int *aux = (int *)param;
	int pIndex = *aux;
	DWORD timeout;
	int speed;
	_tprintf(TEXT("[GAME] Client Snake %d thread Started...\n"), pIndex);
	do
	{
		speed = SNAKE_SPEED_TIME;
		if (info.snake[pIndex].glue > 0 && info.snake[0].oil > 0)
		{
			info.snake[pIndex].glue--;
			info.snake[pIndex].oil--;
		}
		else if (info.snake[pIndex].glue > 0)
		{
			speed = SNAKE_SPEED_TIME * 4;
			info.snake[pIndex].glue--;
		}
		else if (info.snake[pIndex].oil > 0)
		{
			speed = SNAKE_SPEED_TIME / 4;
			info.snake[pIndex].oil--;
		}
		Sleep(speed);
		timeout = WaitForSingleObject(hSnakeMutex, 200);
		if (timeout == WAIT_TIMEOUT) continue;
		if (requestKey[pIndex] < 1 || requestKey[pIndex] > 4) // theres no request or invalid one
		{
			requestKey[pIndex] = 0;
			moveSnake(info.snake[pIndex].currentDirection, pIndex);
		}
		else // new Request
		{
			moveSnake(requestKey[pIndex], pIndex);
			requestKey[pIndex] = 0; // reset request key to this player snake
		}
		SetEvent(hEventSendInfo);
		ReleaseMutex(hSnakeMutex);
	} while (gameRunning == 1 && !info.snake[pIndex].died && info.gameEnded == 0 && info.serverTurnedOff == 0);
	_tprintf(TEXT("[GAME] Client Snake %d thread End...\n"), pIndex);
	ExitThread(0);
}

// Return Direction KEY
int objInNeighbor(int x, int y, int currentDir)
{
	int neighborX[4];
	int neighborY[4];
	get4NeighborCords(neighborX, neighborY, x, y);
	for (int i = 0; i < info.field.numObjs; i++)
		for (int c = 0; c < 4; c++)
			if (neighborX[c] == info.field.object[i][0] &&
				neighborY[c] == info.field.object[i][1])
				return i + 1;
	return 0;
}

DWORD WINAPI objectRespawn(LPVOID *param)
{
	int *objIndex = (int *)param;
	int index = *objIndex;
	DWORD timeout;
	_tprintf(TEXT("[GAME] Object ID: %d\n"), index);
	do
	{
		switch (config.itemsFrequency)
		{
		case 1:
			Sleep(LOW_ITEM_FREQUENCY_TIME);
			break;
		case 2:
			Sleep(MEDIUM_ITEM_FREQUENCY_TIME);
			break;
		case 3:
			Sleep(HIGH_ITEM_FREQUENCY_TIME);
			break;
		}
		//_tprintf(TEXT("[GAME] Object ID: %d Before Mutex\n"), index);
		timeout = WaitForSingleObject(hMutexObjs, 500);
		if (timeout == WAIT_TIMEOUT) { continue; }
		info.field.objType[index] = randomObj();
		SetEvent(hEventSendInfo);
		//_tprintf(TEXT("[GAME] Object ID: %d After Mutex\n"), index);
		ReleaseMutex(hMutexObjs);
	} while (gameRunning == 1 && info.gameEnded == 0 && info.serverTurnedOff == 0);
	_tprintf(TEXT("[GAME] Object ID: %d Thread Closed\n"), index);
	ExitThread(0);

}

void print(TCHAR *p)
{
	_tprintf(TEXT("%s\n"), p);
}

DWORD WINAPI recvFromCli(LPVOID *param)
{
	DWORD timeout;
	ClientInput in;
	int index;
	gameRunning = 1;
	print(TEXT("[GAME] Thread Ready for processing Data From Players... "));
	do
	{
		//timeout = WaitForSingleObject(hSnakeMutex, 200);
		//if (timeout == WAIT_TIMEOUT)
		//{
		//	if (info.gameEnded == 1 || gameRunning == 0)  break;
		//	continue;
		//}
		in = cirBufOutSM(100, &timeout);
		index = SnakeIndex(in);
		if (index < 0) // invalid players 
		{
			//_tprintf(TEXT("[GAME] Invalid username: %s\n"),in.username);
			ReleaseMutex(hSnakeMutex);
			continue;
		}
		// its a valid player
		requestKey[index] = in.key; // set request key to player snake thread
									// timeout
		if (timeout == 1)
		{
			in.key = info.snake[index].currentDirection;
			_tcsncpy_s(in.username, MAX_TAM, info.snake[index].username, _TRUNCATE);
		}
		if (in.exit)
		{
			_tprintf(TEXT("\n[GAME] Client %s left during game.\n"), in.username);
			info.clientsOnline--;
			if (info.clientsOnline == 0)
			{
				state.state = 0;
				break;
			}
		}
		//_tprintf(TEXT("[GAME] Key Pressed: %d by %s Player\n"), in.key, in.username); // Important
	} while (gameRunning == 1 && info.gameEnded == 0 && info.serverTurnedOff == 0);
	_tprintf(TEXT("[GAME] Game Thread recvFromCli Closed.\n"));
	gameRunning = 0;
	info.gameEnded = 0;
	ExitThread(0);
}

int gameEnd()
{
	int snakesAlive;
	snakesAlive = info.numBots + info.numPlayers;
	for (int i = 0; i < (info.numBots + info.numPlayers); i++)
		if (info.snake[i].died == 1) snakesAlive--;
	info.structVal = 4;
	if (snakesAlive == 1 || snakesAlive == 0)
	{
		gameRunning = 0;
		info.gameEnded = 1;
		return 1;
	}
	return 0;
}

DWORD WINAPI sendToCli(LPVOID *param)
{
	DWORD timeout;
	//updateRenderSM(info);
	_tprintf(TEXT("\t[GAME] SendToCli Thread Started.\n"), gameRunning);
	while (gameRunning && !info.gameEnded && !info.serverTurnedOff)
	{
		timeout = WaitForSingleObject(hEventSendInfo, 500);
		if (timeout == WAIT_TIMEOUT) { continue; }
		//_tprintf(TEXT("\t[GAME]SendToCli Cicle\n"));
		gameEnd();
		//_tprintf(TEXT("\t[GAME] Before update Render.\n"));
		updateRenderSM(info);
		//_tprintf(TEXT("\t[GAME] After update Render.\n"));

	}
	_tprintf(TEXT("\t[GAME] SendToCli Thread Closed.\n"));
	return 0;
}

int nearestItem(int x, int y)
{
	int obj = 0, temp;
	for (int i = 0; i < info.field.numObjs; i++)
	{
		if (info.field.objType[i] == 3) continue;
		temp = distanceBetween2Points(x, y, info.field.object[0][0], info.field.object[0][1]);
		if (temp < obj) obj = temp;
	}

	if (x > info.field.object[obj][0] && y != info.field.object[obj][1])
		return KEY_LEFT;
	else if (x < info.field.object[obj][0] && y != info.field.object[obj][1])
		return KEY_RIGHT;
	else if (y > info.field.object[obj][1] && x != info.field.object[obj][0])
		return KEY_UP;
	else if (y < info.field.object[obj][1] && x != info.field.object[obj][0])
		return KEY_DOWN;

	return 0;
}

int SnakeIndex(ClientInput in)
{
	for (int i = 0; i < info.numPlayers; i++)
		if (!_tcsncmp(in.username, info.snake[i].username, sizeof(in.username)))
			return i;
	return -1;
}

int moveSnake(int key, int pIndex)
{
	int x, y, nextX, nextY, oldX, oldY;
	int numAllSnakes = info.numPlayers + info.numBots;
	// 1 -> up | 2 -> down | 3 -> right | 4 -> left | -1 -> no player

	// is under vodka effect - inverted keys
	if (info.snake[pIndex].invertKeys > 0)
	{
		key = invertKey(key);
		info.snake[pIndex].invertKeys--;
	}

	if (pIndex == -1) return -1; // its not a valid player
	else if (key < 1 || key > 4) return -1;
	else if (key == KEY_UP && info.snake[pIndex].currentDirection == KEY_DOWN)
	{
		key = info.snake[pIndex].currentDirection;
	}
	else if (key == KEY_DOWN && info.snake[pIndex].currentDirection == KEY_UP)
	{
		key = info.snake[pIndex].currentDirection;
	}
	else if (key == KEY_LEFT && info.snake[pIndex].currentDirection == KEY_RIGHT)
	{
		key = info.snake[pIndex].currentDirection;
	}
	else if (key == KEY_RIGHT && info.snake[pIndex].currentDirection == KEY_LEFT)
	{
		key = info.snake[pIndex].currentDirection;
	}
	else if (info.snake[pIndex].died) return -1;

	info.snake[pIndex].currentDirection = key;

	// Get current x y head coords
	x = info.snake[pIndex].coords[0][0];
	y = info.snake[pIndex].coords[0][1];

	// Get probable next coordinates
	switch (key)
	{
	case 1: // Up
		nextY = info.snake[pIndex].coords[0][1] - 1;
		nextX = info.snake[pIndex].coords[0][0];
		break;
	case 2: // Down
		nextY = info.snake[pIndex].coords[0][1] + 1;
		nextX = info.snake[pIndex].coords[0][0];
		break;
	case 3: // Right
		nextX = info.snake[pIndex].coords[0][0] + 1;
		nextY = info.snake[pIndex].coords[0][1];
		break;
	case 4: // Left
		nextX = info.snake[pIndex].coords[0][0] - 1;
		nextY = info.snake[pIndex].coords[0][1];
		break;
	default:
		break;
	}
	// Check if next coords isn't out of field range
	// Snake die if touch the border
	if (nextX < 0 || nextX > info.field.sizeX ||
		nextY < 0 || nextY > info.field.sizeY)
	{
		info.snake[pIndex].died = 1;
		return 2;
	}

	// Check if theres a free square to move forward
	for (int i = 0; i < numAllSnakes; i++)
	{
		if (info.snake[i].died == 1)
			continue;
		for (int j = 0; j < info.snake[i].snakeSize; j++)
		{
			// Snake die if touch other snake
			if (nextX == info.snake[i].coords[j][0] && nextY == info.snake[i].coords[j][1])
			{
				info.snake[pIndex].died = 1;
				return 2;
			}
		}
	}
	// move Snake
	oldX = info.snake[pIndex].coords[info.snake[pIndex].snakeSize - 1][0];
	oldY = info.snake[pIndex].coords[info.snake[pIndex].snakeSize - 1][1];
	for (int i = info.snake[pIndex].snakeSize - 1; i >= 0; i--)
	{
		if (i != 0)
		{
			info.snake[pIndex].coords[i][0] = info.snake[pIndex].coords[i - 1][0]; // X
			info.snake[pIndex].coords[i][1] = info.snake[pIndex].coords[i - 1][1]; // Y
		}
		else
		{
			info.snake[pIndex].coords[i][0] = nextX; // Update new X snake head position
			info.snake[pIndex].coords[i][1] = nextY; // Update new Y snake head position
		}
	}
	// Check if theres an Object 
	checkAndEatObject(pIndex, nextX, nextY, oldX, oldY);
	updateRenderSM(info);
	return 1;
}

void checkAndEatObject(int pIndex, int nextX, int nextY, int oldX, int oldY)
{
	// Check if theres an Object 
	for (int i = 0; i < info.field.numObjs; i++)
	{
		// Found an Object
		if (nextX == info.field.object[i][0] && nextY == info.field.object[i][1])
		{
			switch (info.field.objType[i])
			{
			case 1: // Food
				riseSnakeSize(pIndex, i, nextX, nextY, oldX, oldY); // Update Snake Size
				break;
			case 2: // Ice
				obj2Collected(pIndex, i);
				break;
			case 3: // Grenade
				obj3Collected(pIndex, i);
				break;
			case 4: // Vodka
				obj4Collected(pIndex, i);
				break;
			case 5: // Oil
				obj5Collected(pIndex, i);
				break;
			case 6: // Glue
				obj6Collected(pIndex, i);
				break;
			case 7: // O-Vodka
				obj7Collected(pIndex, i);
				break;
			case 8:	// O-Oil
				obj8Collected(pIndex, i);
				break;
			case 9: // O-Glue
				obj9Collected(pIndex, i);
				break;
			case 10: // Snake Cutted in Half - ButcheryKnife
				obj10Collected(pIndex, i);
				break;
			default:
				break;
			}
		}
	}
}

void riseSnakeSize(int pIndex, int objIndex, int headX, int headY, int oldX, int oldY)
{
	info.snake[pIndex].snakeSize++;
	info.snake[pIndex].coords[info.snake[pIndex].snakeSize - 1][0] = oldX;
	info.snake[pIndex].coords[info.snake[pIndex].snakeSize - 1][1] = oldY;

	info.snake[pIndex].points += 2; // Gain pontuation
	updateNewObj(objIndex, randomObj());

}

void obj2Collected(int pIndex, int objIndex)
{
	info.snake[pIndex].snakeSize--;
	// for safety
	info.snake[pIndex].coords[info.snake[pIndex].snakeSize - 1][0] = -1;
	info.snake[pIndex].coords[info.snake[pIndex].snakeSize - 1][1] = -1;

	updateNewObj(objIndex, randomObj());
}

void obj3Collected(int pIndex, int objIndex)
{
	info.snake[pIndex].died = 1;

	updateNewObj(objIndex, randomObj());
}

void obj4Collected(int pIndex, int objIndex)
{
	switch (config.temporaryEffectTime)
	{
	case 1:
		info.snake[pIndex].invertKeys += LOW_EFFECT_TIME;
		break;
	case 2:
		info.snake[pIndex].invertKeys += MEDIUM_EFFECT_TIME;
		break;
	case 3:
		info.snake[pIndex].invertKeys += HIGH_EFFECT_TIME;
		break;
	}

	updateNewObj(objIndex, randomObj());
}

void obj5Collected(int pIndex, int objIndex)
{
	switch (config.temporaryEffectTime)
	{
	case 1:
		info.snake[pIndex].oil += LOW_EFFECT_TIME;
		break;
	case 2:
		info.snake[pIndex].oil += MEDIUM_EFFECT_TIME;
		break;
	case 3:
		info.snake[pIndex].oil += HIGH_EFFECT_TIME;
		break;
	}
	updateNewObj(objIndex, randomObj());
}

void obj6Collected(int pIndex, int objIndex)
{
	switch (config.temporaryEffectTime)
	{
	case 1:
		info.snake[pIndex].glue = LOW_EFFECT_TIME / 2;
		break;
	case 2:
		info.snake[pIndex].glue = MEDIUM_EFFECT_TIME / 2;
		break;
	case 3:
		info.snake[pIndex].glue = HIGH_EFFECT_TIME / 2;
		break;
	}
	updateNewObj(objIndex, randomObj());
}

void obj7Collected(int pIndex, int objIndex)
{
	int r, i = 50;
	do
	{
		r = RangedRandDemo(0, info.numBots + info.numPlayers);
		if (info.snake[r].died == 1)
			continue;
	} while (i--);
	if (i == 0) return;
	obj4Collected(r, objIndex);
}

void obj8Collected(int pIndex, int objIndex)
{
	int r, i = 50;
	do
	{
		r = RangedRandDemo(0, info.numBots + info.numPlayers);
		if (info.snake[r].died == 1)
			continue;
	} while (i--);
	if (i == 0) return;


	obj5Collected(r, objIndex);
}

void obj9Collected(int pIndex, int objIndex)
{
	int r, i = 50;
	do
	{
		r = RangedRandDemo(0, info.numBots + info.numPlayers);
		if (info.snake[r].died == 1)
			continue;
	} while (i--);
	if (i == 0) return;


	obj6Collected(r, objIndex);
}

void obj10Collected(int pIndex, int objIndex)
{
	info.snake[pIndex].snakeSize /= 2;

	info.snake[pIndex].points += 10; // Gain pontuation
	updateNewObj(objIndex, randomObj());
}

int invertKey(int key)
{
	switch (key)
	{
	case KEY_UP:
		return KEY_DOWN;
	case KEY_DOWN:
		return KEY_UP;
	case KEY_RIGHT:
		return KEY_LEFT;
	case KEY_LEFT:
		return KEY_RIGHT;
	}
	return 0;
}

void updateNewObj(int objIndex, int objType)
{
	//int r = RangedRandDemo(0, 100);
	DWORD timeout;
	int newX, newY;

break1:
	newX = RangedRandDemo(0, info.field.sizeX);
	newY = RangedRandDemo(0, info.field.sizeY);
	for (int iSnake = 0; iSnake < (info.numPlayers + info.numBots); iSnake++)
		for (int j = 0; j < info.snake[iSnake].snakeSize; j++)
			if (newX == info.snake[iSnake].coords[j][0] &&
				newY == info.snake[iSnake].coords[j][1])
			{
				goto break1;
			}
	for (int iObj = 0; iObj < info.field.numObjs; iObj++)
	{
		if (newX == info.field.object[iObj][0] &&
			newY == info.field.object[iObj][1])
		{
			goto break1;
		}
	}
	// ---------- Change Item -------------
	timeout = WaitForSingleObject(hMutexObjs, 200);
	if (timeout == WAIT_TIMEOUT) { /*_tprintf(TEXT("Item change mutex timeout\n"));*/ }
	info.field.objType[objIndex] = objType;
	info.field.object[objIndex][0] = newX;
	info.field.object[objIndex][1] = newY;
	ReleaseMutex(hMutexObjs);
}

int SetUpRegistry(Info f)
{
	Author a[2];
	DWORD regFlag;
	Score sc;

	if (RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\SnakeGame"), 0, NULL, REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS, NULL, &regKey, &regFlag) != ERROR_SUCCESS)
	{
		_tprintf(TEXT("[REGISTRY] Error Creating Registry Key (%d)\n"), GetLastError());
		return -1;
	}

	_tprintf(TEXT("[REGISTRY] Registry Created Software\\SnakeGame\n"));
	if (regFlag == REG_CREATED_NEW_KEY)
	{
		_tprintf(TEXT("[REGISTRY] NEW KEY CREATED\n"));
		_stprintf_s(a[0].name, _TRUNCATE, AUTHOR1);
		_stprintf_s(a[0].num, _TRUNCATE, STUDENT_NUM1);
		_stprintf_s(a[1].name, _TRUNCATE, AUTHOR2);
		_stprintf_s(a[1].num, _TRUNCATE, STUDENT_NUM2);

		RegSetValueEx(regKey, REG_AUTHOR, 0, REG_BINARY, (LPBYTE)&a, 2 * sizeof(Author));

		int version = 1;
		RegSetValueEx(regKey, TEXT("Version"), 0, REG_DWORD, (LPBYTE)&version, sizeof(int));

		_stprintf_s(sc.username, _TRUNCATE, TEXT("Admin"));
		sc.score = 0;
		int init = 0;
		RegSetValueEx(regKey, REG_SCORE_NUM_PLAYER, 0, REG_DWORD, (LPBYTE)&init, sizeof(int));
		RegSetValueEx(regKey, REG_SCORE, 0, REG_BINARY, (LPBYTE)&sc, sizeof(Score));
	}
	else if (regFlag == REG_OPENED_EXISTING_KEY)
	{
		_tprintf(TEXT("[REGISTRY] READ REGISTRY\n"));
		printRegistry();
	}
	return 0;
}

void printRegistry()
{

	DWORD size;
	int numPlayers_reg;

	size = sizeof(int); // info.numPlayers

	RegQueryValueEx(regKey, REG_SCORE_NUM_PLAYER, NULL, NULL, (LPBYTE)&numPlayers_reg, &size);

	size = sizeof(Score) * numPlayers_reg;

	Score *sc = (Score *)malloc(size);

	RegQueryValueEx(regKey, REG_SCORE, NULL, NULL, (LPBYTE)sc, &size);
	_tprintf(TEXT("[REGISTRY] NUM RESGISTED PLAYERS:  %d\n"), numPlayers_reg);
	for (int i = 0; i < numPlayers_reg; i++)
		_tprintf(TEXT("[REGISTRY] Player: %s [MAX SCORE: %d]\n"), sc[i].username, sc[i].score);
}

void updateRegistry(Snake p)
{

	DWORD size;
	int numPlayers_in_reg;
	int i;

	size = sizeof(int);

	RegQueryValueEx(regKey, REG_SCORE_NUM_PLAYER, NULL, NULL, (LPBYTE)&numPlayers_in_reg, &size);

	size = sizeof(Score) * numPlayers_in_reg;

	Score *sc = (Score *)malloc(size);

	RegQueryValueEx(regKey, REG_SCORE, NULL, NULL, (LPBYTE)sc, &size);

	for (i = 0; i < numPlayers_in_reg; i++)
		if (!_tcsncmp(sc[i].username, p.username, _tcslen(p.username)))
		{
			if (p.points > sc[i].score)
			{
				sc[i].score = p.points;
				RegSetValueEx(regKey, REG_SCORE, 0, REG_BINARY, (LPBYTE)sc, sizeof(Score) * numPlayers_in_reg);
			}
			//if player is already in registry, fuction returns
			return;
		}

	// Registe new Player in Registry
	numPlayers_in_reg++;
	size = sizeof(Score) * numPlayers_in_reg;
	Score *cs_playerAdded = (Score *)malloc(size);

	//Create new Player score struct and copy data into
	Score newPlayer;
	_stprintf_s(newPlayer.username, _TRUNCATE, p.username);
	newPlayer.score = p.points;

	// go though all old struct copying values to new one
	for (int i = 0; i < numPlayers_in_reg - 1; i++)
		cs_playerAdded[i] = sc[i];
	// last remaining space in struct array to add new player
	cs_playerAdded[i] = newPlayer;


	_tprintf(TEXT("[NEW CLIENT REGISTED IN REGISTRY]ID: %d Player: %s [SCORE: %d]\n"), i,
		cs_playerAdded[i].username, cs_playerAdded[i].score);

	RegSetValueEx(regKey, REG_SCORE_NUM_PLAYER, 0, REG_DWORD, (LPBYTE)&numPlayers_in_reg, sizeof(int));
	RegSetValueEx(regKey, REG_SCORE, 0, REG_BINARY, (LPBYTE)cs_playerAdded, sizeof(Score) * numPlayers_in_reg);
}

void closeRegKey()
{

}