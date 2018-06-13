#define _WIN32_WINNT 0x0500 // sa
#include "snake_dll.h"
#include "resource.h"

#include <aclapi.h>
#include <strsafe.h>
#include <sddl.h> // sa - this library must be included after Window.h

/* Call it once
* @return 1 if it works properly*/
int serverCreateSM()
{
	CircularBufferInfo cb;
	hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
		0, SM_SIZE, sharedMemName);
	if (hMapFile == NULL)
	{
		_tprintf(TEXT("Could not create file mapping object (%d).\n"),
			GetLastError());
		return 0;
	}

	pBufConfig = (LPTSTR)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (pBufConfig == NULL)
	{
		CloseHandle(hMapFile);
		_tprintf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());
		return 0;
	}
	pBufAcceptConfig = pBufConfig + sizeof(Config);
	pBufClientInput = pBufAcceptConfig + sizeof(TCHAR)*MAX_TAM;
	pBufInfo = pBufClientInput + sizeof(ClientInput) * CLI_IN_BUF_SIZE;
	pBufStateInfo = pBufInfo + sizeof(Info);
	pBufCircularBuffer = pBufStateInfo + sizeof(StateInfo);

	// Set username to "0"
	Config tempConfig = {0};
	//CopyMemory(&tempConfig, (PVOID)pBufConfig, sizeof(Config));
	_tcsncpy_s(tempConfig.username1, MAX_TAM, TEXT("0"), _TRUNCATE);
	_tcsncpy_s(tempConfig.username2, MAX_TAM, TEXT("0"), _TRUNCATE);
	tempConfig.hasConfig = 0;
	CopyMemory((PVOID)pBufConfig, &tempConfig, sizeof(Config));

	// Set username to "0"
	ClientInput temp[CLI_IN_BUF_SIZE] = {0};
	for(int i = 0; i < CLI_IN_BUF_SIZE; i++)
		_tcsncpy_s(temp[i].username, MAX_TAM, TEXT("0"), _TRUNCATE);
	CopyMemory((PVOID)pBufClientInput, &temp, sizeof(ClientInput) * CLI_IN_BUF_SIZE);

	state.state = 0;
	state.cliOnThoughtNP = 0;
	state.cliOnThoughtSM = 0;
	setStateServer(state);

	//Circular Buffer Set Up
	cb.bufReadIndex = 0;
	cb.bufWriteIndex = 0;
	CopyMemory((PVOID)pBufCircularBuffer, &cb, sizeof(CircularBufferInfo));

	hSMServerCanRead = CreateEvent(NULL, TRUE, FALSE, SMServerCanReadName);
	if (hSMServerCanRead == NULL)
	{
		UnmapViewOfFile(pBufConfig);
		CloseHandle(hMapFile);
		_tprintf(TEXT("Could not create a IO Shared Memory Event 1- (%d).\n"),
			GetLastError());
		return 0;
	}
	hSMClientCanRead = CreateEvent(NULL, TRUE, FALSE, SMClientCanReadName);
	if (hSMClientCanRead == NULL)
	{
		UnmapViewOfFile(pBufConfig);
		CloseHandle(hMapFile);
		CloseHandle(hSMServerCanRead);
		_tprintf(TEXT("Could not create a IO Shared Memory Event 2- (%d).\n"),
			GetLastError());
		return 0;
	}
	hSMClientCanRetrieveRender = CreateEvent(NULL, TRUE, TRUE, SMClientCanRetrieveRenderName);
	if (hSMClientCanRetrieveRender == NULL)
	{
		UnmapViewOfFile(pBufConfig);
		CloseHandle(hMapFile);
		CloseHandle(hSMServerCanRead);
		CloseHandle(hSMClientCanRead);
		_tprintf(TEXT("Could not create a IO Shared Memory Event 3- (%d).\n"),
			GetLastError());
		return 0;
	}
	// Intial free space in buffer
	hSemSpaceCirBuf = CreateSemaphore(NULL, CLI_IN_BUF_SIZE, CLI_IN_BUF_SIZE, SemaReadCirBufPath);
	if(hSemSpaceCirBuf == NULL)
	{
		UnmapViewOfFile(pBufConfig);
		CloseHandle(hMapFile);
		CloseHandle(hSMServerCanRead);
		CloseHandle(hSMClientCanRead);
		CloseHandle(hSMClientCanRetrieveRender);
		_tprintf(TEXT("Could not create a IO Shared Memory Event 4- (%d).\n"),
			GetLastError());
		return 0;
	}
	hSemCountCirBuf = CreateSemaphore(NULL, 0, CLI_IN_BUF_SIZE, SemaWriteCirBufPath);
	if (hSemCountCirBuf == NULL)
	{
		UnmapViewOfFile(pBufConfig);
		CloseHandle(hMapFile);
		CloseHandle(hSMServerCanRead);
		CloseHandle(hSMClientCanRead);
		CloseHandle(hSMClientCanRetrieveRender);
		CloseHandle(hSemSpaceCirBuf);
		_tprintf(TEXT("Could not create a IO Shared Memory Event 5- (%d).\n"),
			GetLastError());
		return 0;
	}
	hMutexCirBuf = CreateMutex(NULL, FALSE, MutexCirBufPath);
	if (hMutexCirBuf == NULL)
	{
		UnmapViewOfFile(pBufConfig);
		CloseHandle(hMapFile);
		CloseHandle(hSMServerCanRead);
		CloseHandle(hSMClientCanRead);
		CloseHandle(hSMClientCanRetrieveRender);
		CloseHandle(hSemSpaceCirBuf);
		CloseHandle(hSemCountCirBuf);
		_tprintf(TEXT("Could not create a IO Shared Memory Event 6- (%d).\n"),
			GetLastError());
		return 0;
	}
	return 1;
}

void setStateServer(StateInfo state)
{
	CopyMemory((PVOID)pBufStateInfo, &state, sizeof(StateInfo));
}
StateInfo getStateServer()
{
	StateInfo st;
	CopyMemory(&st, (PVOID)pBufStateInfo, sizeof(StateInfo));
	return st;
}
int getStateSM()
{
	StateInfo st;
	CopyMemory(&st, (PVOID)pBufStateInfo, sizeof(StateInfo));
	return st.state;
}
int setNoCliInSharedMemState()
{
	state = getStateServer();
	if (state.cliOnThoughtSM == 0)
		return 0;

	state.cliOnThoughtSM = 0;
	setStateServer(state);
	return 1;
}
int setCliInSharedMemState()
{
	getStateServer();
	if (state.cliOnThoughtSM == 1)
		return 0;

	state.cliOnThoughtSM = 1;
	setStateServer(state);
	return 1;
}
void cleanUp(PSID pEveryoneSID, PSID pAdminSID, PACL pACL, PSECURITY_DESCRIPTOR pSD)
{
	if (pEveryoneSID)
		FreeSid(pEveryoneSID);
	if (pAdminSID)
		FreeSid(pAdminSID);
	if (pACL)
		LocalFree(pACL);
	if (pSD)
		LocalFree(pSD);
}

DWORD WINAPI recvNewCliConnection(LPVOID *param)
{
	//---------------------------------------------------------------------------------------------
	// SECURITY_ATTRIBUTES open access for everyone read and write
	SECURITY_ATTRIBUTES sa;
	TCHAR *szSD = TEXT("D:")
		TEXT("(A;OICI;GA;;;BG)")
		TEXT("(A;OICI;GA;;;AN)")
		TEXT("(A;OICI;GA;;;AU)")
		TEXT("(A;OICI;GA;;;BA)")
		TEXT("(A;OICI;GA;;;WD)");
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = FALSE;
	PULONG nSize = 0;
	if (!ConvertStringSecurityDescriptorToSecurityDescriptor(szSD, SDDL_REVISION_1, &(sa.lpSecurityDescriptor), NULL))
	{
		_tprintf(TEXT("Error converting sd (%d)\n"), GetLastError());
		return -1;
	}
	//-------------------------------------------------------------------------------------------------------
	HANDLE hPipe;
	TCHAR pathPipe[100];
	HANDLE hThread;
	DWORD dwThreadID;
	int pipeIndex;
	for (int i = 0; i < MAX_PLAYERS; i++)
		hCliPipes[i] = INVALID_HANDLE_VALUE;
	_stprintf_s(pathPipe, _TRUNCATE,PATH_PIPE_SERVER,SERVER_IP);
	state.state = 0;
	setStateServer(state);
	while (1)
	{
		if (state.cliOnThoughtNP < MAX_PLAYERS && (state.state == 0 || state.state == 2)) // Wait by player to config or await player to join
		{
			hPipe = CreateNamedPipe(pathPipe, PIPE_ACCESS_DUPLEX, PIPE_WAIT | PIPE_TYPE_MESSAGE
				| PIPE_READMODE_MESSAGE, MAX_PLAYERS, sizeof(Info), sizeof(Info), 1000, &sa);

			if (hPipe == INVALID_HANDLE_VALUE)
			{
				_tprintf(TEXT("Error Creating Namedpipe."));
				continue;
			}
			// Fica á espera mesmo que fim seja igual a true
			if (!ConnectNamedPipe(hPipe, NULL))
			{
				_tprintf(TEXT("Client could not connect"));
				CloseHandle(hPipe);
				continue;
			}
			else
			{
				// Search for empty spaces
				for (pipeIndex = 0; pipeIndex < MAX_PLAYERS; pipeIndex++)
					if (hCliPipes[pipeIndex] == INVALID_HANDLE_VALUE)
					{
						hCliPipes[pipeIndex] = hPipe;
						state.cliOnThoughtNP++;
						break;
					}
				hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)recvCliInfo,&pipeIndex, 0, &dwThreadID);
				if (hThread == NULL)
				{
					_tprintf(TEXT("Error creating Thread. (%d)"), GetLastError());
					hCliPipes[pipeIndex] = INVALID_HANDLE_VALUE;
					state.cliOnThoughtNP--;
					continue;
				}
				else
					CloseHandle(hThread);
			}
			
		}
		else
		{
			Sleep(1000);
		}
	}

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		DisconnectNamedPipe(hCliPipes[i]);
		CloseHandle(hCliPipes[i]);	
	}
	return 0;
}

DWORD WINAPI recvCliInfo(LPVOID *param)
{
	int pipeIndex = (int&) *param;
	Config c = {0};
	ClientInput in = {0};
	DWORD n, timeout = 0;
	BOOL fSuccess = FALSE;
	TCHAR username[MAX_TAM];
	_tcsncpy_s(username, MAX_TAM,TEXT("0"), _TRUNCATE);
	_gettch();
	getStateServer();
	_tprintf(TEXT("Named pipe index: %d State: %d"), pipeIndex, state);
	switch (state.state)
	{
	case 0: // new player 
		state.state = 1;
		setStateServer(state);
		fSuccess = WriteFile(hCliPipes[pipeIndex], &(state.state), sizeof(int), &n, NULL);
		_tprintf(TEXT("S1\n"), pipeIndex, state);
		break;
	case 2: // await players
		setStateServer(state);
		fSuccess = WriteFile(hCliPipes[pipeIndex], &(state.state), sizeof(int), &n, NULL);
		_tprintf(TEXT("S2\n"), pipeIndex, state);
	case 1: // config
	case 3: // game running
		_tprintf(TEXT("In this state should not launch cli thread. (%d)"), GetLastError());
		ExitThread(1);

	}
	if (!fSuccess || n == 0)
	{
		if (GetLastError() == ERROR_BROKEN_PIPE)
		{
			if (!_tcsncmp(username, TEXT("0"), _tcslen(username)))
				_tprintf(TEXT("Client Turned Off. (%d)"), GetLastError());
			else
				_tprintf(TEXT("Client %s Turned Off. (%d)"), username, GetLastError());
			ExitThread(2);
		}
		else
		{
			if (!_tcsncmp(username, TEXT("0"), _tcslen(username)))
				_tprintf(TEXT("ReadFile Failed. (%d)"), GetLastError());
			else
				_tprintf(TEXT("ReadFile Failed from %s. (%d)"), username, GetLastError());
			ExitThread(3);
		}
	}
	while (1)
	{
		switch (state.state)
		{
		case 0:
		case 1:
			fSuccess = ReadFile(hCliPipes[pipeIndex],&c, sizeof(Config), &n, NULL);
			break;
		case 2: // AwaitPlayersConnection
			fSuccess = ReadFile(hCliPipes[pipeIndex], &in, sizeof(ClientInput), &n, NULL);
			break;
		case 3:
			fSuccess = ReadFile(hCliPipes[pipeIndex], &in, sizeof(ClientInput), &n, NULL);
			break;
		}
		if (!fSuccess || n == 0)
		{
			if (GetLastError() == ERROR_BROKEN_PIPE)
			{
				if (!_tcsncmp(username, TEXT("0"), _tcslen(username)))
					_tprintf(TEXT("Client Turned Off. (%d)"), GetLastError());
				else
					_tprintf(TEXT("Client %s Turned Off. (%d)"),username, GetLastError());
				break;
			}
			else
			{
				if (!_tcsncmp(username, TEXT("0"), _tcslen(username)))
					_tprintf(TEXT("ReadFile Failed. (%d)"), GetLastError());
				else
					_tprintf(TEXT("ReadFile Failed from %s. (%d)"), username, GetLastError());
			}
		}
		switch (state.state)
		{
		case 0:
		case 1:
			clientSetConfigSM(c);
			break;
		case 2: // AwaitPlayersConnection
				//fSuccess = ReadFile(hCliPipes[pipeIndex], &c, sizeof(ClientInput), &n, NULL);
			break;
		case 3:
			clientInputGameSM(1000, &timeout, in.username, in.key, in.exit);
			break;
		}
		if(timeout == 1) {}
	}
	hCliPipes[pipeIndex] = INVALID_HANDLE_VALUE;
	return 0;
}

void cleanup(PSID pEveryoneSID, PSID pAdminSID, PACL pACL, PSECURITY_DESCRIPTOR pSD)
{
	if (pEveryoneSID)
		FreeSid(pEveryoneSID);
	if (pAdminSID)
		FreeSid(pAdminSID);
	if (pACL)
		LocalFree(pACL);
	if (pSD)
		LocalFree(pSD);
}

int getMyIP(TCHAR *ip)
{
	return 0;
}

void closeSM()
{
	state = getStateServer();
	if (state.state != 0)
	{
		state.state = 0;
		state.cliOnThoughtSM = 0;
		setStateServer(state);
	}
	UnmapViewOfFile(pBufConfig);
	CloseHandle(hMapFile);
	CloseHandle(hSMServerCanRead);
	CloseHandle(hSMClientCanRead);
	CloseHandle(hSMClientCanRetrieveRender);
	CloseHandle(hSemSpaceCirBuf);
	CloseHandle(hSemCountCirBuf);
	CloseHandle(hMutexCirBuf);
}

Config retrieveConfigSM(int fireTimeoutTime, DWORD *timeout)
{
	Config temp = {0};
	*timeout = WaitForSingleObject(hSMServerCanRead, fireTimeoutTime);
	ResetEvent(hSMServerCanRead);
	if (*timeout == WAIT_TIMEOUT)
	{
		*timeout = 1;
		return temp;
	}

	*timeout = 0;
	CopyMemory(&temp, (PVOID)pBufConfig, sizeof(Config));
	return temp;
}

void updateAcceptConfigSM(TCHAR acceptConfig[])
{
	CopyMemory((PVOID)pBufAcceptConfig, acceptConfig, sizeof(AcceptConfig));
	SetEvent(hSMClientCanRead);
}

void retrieveClientInputSM(int fireTimeoutTime, DWORD *timeout, ClientInput cliIn[])
{

	*timeout = WaitForSingleObject(hSMServerCanRead, fireTimeoutTime);
	ResetEvent(hSMServerCanRead);
	if (*timeout == WAIT_TIMEOUT)
	{
		*timeout = 1;
		return;
	}

	*timeout = 0;
	CopyMemory(cliIn, (PVOID)pBufClientInput, sizeof(ClientInput) * MAX_PLAYERS_SAME_CLIENT);
	//SetEvent(hSMClientCanRead);
}

void updateRenderSM(Info info)
{
	// Clients are not able to read 
	ResetEvent(hSMClientCanRetrieveRender);
	CopyMemory((PVOID)pBufInfo, &info, sizeof(info));
	// Clients are able to read again
	SetEvent(hSMClientCanRetrieveRender);
}

// Client Methods
// Start Here

/* Call it once
* @return	1  - if it works properly
*			-1 - error
*			-2 - a player already connect through shared memory
*			-3 - other player is configuring game
*			-4 - a game is already running
*/
int clientOpenAccessSM(int *state)
{
	hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, sharedMemName);
	if (hMapFile == NULL)
	{
		_tprintf(TEXT("Could not create file mapping object (%d).\n"), GetLastError());
		return 0;
	}

	pBufConfig = (LPTSTR)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (pBufConfig == NULL)
	{
		CloseHandle(hMapFile);
		_tprintf(TEXT("Could not map view of file (%d).\n"), GetLastError());
		return -1;
	}

	pBufAcceptConfig = pBufConfig + sizeof(Config);
	pBufClientInput = pBufAcceptConfig + sizeof(TCHAR)*MAX_TAM;
	pBufInfo = pBufClientInput + sizeof(ClientInput) * CLI_IN_BUF_SIZE;
	pBufStateInfo = pBufInfo + sizeof(Info);
	pBufCircularBuffer = pBufStateInfo + sizeof(StateInfo);


	*state = getStateSM();
	switch (*state)
	{
	case 1:
		UnmapViewOfFile(pBufConfig);
		CloseHandle(hMapFile);
		return -3;
	case 3:
		UnmapViewOfFile(pBufConfig);
		CloseHandle(hMapFile);
		return -4;
	}
	hSMServerCanRead = OpenEvent(EVENT_ALL_ACCESS, NULL, SMServerCanReadName);
	if (hSMServerCanRead == NULL)
	{
		UnmapViewOfFile(pBufConfig);
		CloseHandle(hMapFile);
		_tprintf(TEXT("Could not create a 1st IO Shared Memory Event (%d).\n"), GetLastError());
		return -1;
	}
	hSMClientCanRead = OpenEvent(EVENT_ALL_ACCESS, NULL, SMClientCanReadName);
	if (hSMClientCanRead == NULL)
	{
		UnmapViewOfFile(pBufConfig);
		CloseHandle(hMapFile);
		CloseHandle(hSMServerCanRead);
		_tprintf(TEXT("Could not create a 2nd IO Shared Memory Event (%d).\n"), GetLastError());
		return -1;
	}
	hSMClientCanRetrieveRender = OpenEvent(EVENT_ALL_ACCESS, NULL, SMClientCanRetrieveRenderName);
	if (hSMClientCanRetrieveRender == NULL)
	{
		UnmapViewOfFile(pBufConfig);
		CloseHandle(hMapFile);
		CloseHandle(hSMServerCanRead);
		CloseHandle(hSMClientCanRead);
		_tprintf(TEXT("Could not create a Render IO Shared Memory Event (%d).\n"), GetLastError());
		return -1;
	}
	// Intial free space in buffer
	hSemSpaceCirBuf = OpenSemaphore(SEMAPHORE_ALL_ACCESS,NULL,SemaReadCirBufPath);
	if (hSemSpaceCirBuf == NULL)
	{
		UnmapViewOfFile(pBufConfig);
		CloseHandle(hMapFile);
		CloseHandle(hSMServerCanRead);
		CloseHandle(hSMClientCanRead);
		CloseHandle(hSMClientCanRetrieveRender);
		return -1;
	}
	hSemCountCirBuf = OpenSemaphore(SEMAPHORE_ALL_ACCESS, NULL, SemaWriteCirBufPath);
	if (hSemCountCirBuf == NULL)
	{
		UnmapViewOfFile(pBufConfig);
		CloseHandle(hMapFile);
		CloseHandle(hSMServerCanRead);
		CloseHandle(hSMClientCanRead);
		CloseHandle(hSMClientCanRetrieveRender);
		CloseHandle(hSemSpaceCirBuf);
		return -1;
	}
	hMutexCirBuf = OpenMutex(MUTEX_ALL_ACCESS,NULL,MutexCirBufPath);
	if (hMutexCirBuf == NULL)
	{
		UnmapViewOfFile(pBufConfig);
		CloseHandle(hMapFile);
		CloseHandle(hSMServerCanRead);
		CloseHandle(hSMClientCanRead);
		CloseHandle(hSMClientCanRetrieveRender);
		CloseHandle(hSemSpaceCirBuf);
		CloseHandle(hSemCountCirBuf);
		return -1;
	}


	return 1;
}

int onePlayerConnectSM()
{
	CopyMemory(&state, (PVOID)pBufStateInfo, sizeof(StateInfo));
	if (state.cliOnThoughtSM == 1)
		return 1;

	return 0;
}

int clientOpenAccessNP(TCHAR *ip,TCHAR *username,TCHAR *password, TCHAR *errorLog)
{
	// Remote connect client to namedpipe server
	HANDLE hUserToken;

	if (!LogonUser(username, ip, password, LOGON32_LOGON_NEW_CREDENTIALS, LOGON32_PROVIDER_DEFAULT, &hUserToken))
	{
		_stprintf_s(errorLog, _TRUNCATE, TEXT("Error Loggin On to remote Server\n(%d)"), GetLastError());
		return -1;
	}
	if (!ImpersonateLoggedOnUser(hUserToken))
	{
		_stprintf_s(errorLog, _TRUNCATE, TEXT("Error Loggin On to remote Server\n(%d)"), GetLastError());
		return -1;
	}
	//--------------------------------------------------------------------------------------
	BOOL fSuccess = FALSE;
	DWORD dwMode, n = 0;
	TCHAR pipeRemotePath[100];
	hServerPipe = INVALID_HANDLE_VALUE;
	_stprintf_s(pipeRemotePath, _TRUNCATE, PATH_PIPE_SERVER, ip);
	while (1)
	{
		hServerPipe = CreateFile(pipeRemotePath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0/*FILE_ATTRIBUTE_NORMAL*/,
			NULL);

		if (hServerPipe != INVALID_HANDLE_VALUE)
		{
			//_stprintf_s(errorLog, _TRUNCATE, TEXT("Error Loggin On to remote Server\n"));
			break;
		}
		else if (GetLastError() != ERROR_PIPE_BUSY)
		{
			_stprintf_s(errorLog, _TRUNCATE,
				TEXT("Server is online.\nCould not establish connection to Server.\n"));
			return -1;
		}
		if (!WaitNamedPipe(pipeRemotePath, 10000)) 
		{
			_stprintf_s(errorLog, _TRUNCATE, TEXT("Server is full, try to connect later...\n"));
			return -1;
		}
	}
	dwMode = PIPE_READMODE_MESSAGE;
	fSuccess = SetNamedPipeHandleState(hServerPipe, &dwMode, NULL, NULL);
	if (!fSuccess)
	{
		_stprintf_s(errorLog, _TRUNCATE, TEXT("Error changing named pipe to Message mode\n(%d)"),GetLastError());
		return -1;
	}
	//--------------------------------------------------------------------------------
	int state = -1;
	fSuccess = ReadFile(hServerPipe,&state, sizeof(int), &n, NULL);
	if (!fSuccess || n != sizeof(int))
	{
		_stprintf_s(errorLog, _TRUNCATE, TEXT("Error receiving current state message\n(%d)"), GetLastError());
		return -2;
	}
	_stprintf_s(errorLog, _TRUNCATE, TEXT("State Received %d\n"),state);
	return state;
}

void clientSetConfigSM(Config config)
{
	CopyMemory((PVOID)pBufConfig, &config, sizeof(Config));
	SetEvent(hSMServerCanRead);
}

int clientSetConfigNP(Config config)
{
	config.structVal = 1;
	DWORD n, fSuccess = FALSE;

	fSuccess = WriteFile(hServerPipe,&config, sizeof(Config), &n, NULL);
		if(!fSuccess || sizeof(Config) != n)
			return -2;

	CopyMemory((PVOID)pBufConfig, &config, sizeof(Config));
	return 0;
}

/*@return 1 if config is right*/
int getAcceptConfigSM(int fireTimeoutTime, DWORD *timeout, TCHAR returnLog[])
{
	*timeout = WaitForSingleObject(hSMClientCanRead, fireTimeoutTime);
	if (*timeout == WAIT_TIMEOUT)
	{
		*timeout = 1;
		return -1;
	}

	*timeout = 0;
	CopyMemory(returnLog, (PVOID)pBufAcceptConfig, sizeof(TCHAR) * MAX_TAM);

	if (!_tcsncmp(returnLog, TEXT("Close"), _tcslen(TEXT("Close"))))
		return -2;

	if (!_tcsncmp(returnLog, TEXT("0"), _tcslen(TEXT("0"))))
		return 1;
	SetEvent(hSMClientCanRead);
	return 0;
}
int getAcceptConfigNP(TCHAR returnLog[])
{
	Info info = { 0 };
	DWORD n, fSuccess;

	fSuccess = ReadFile(hServerPipe, &returnLog, sizeof(AcceptConfig), &n, NULL);
	if (!fSuccess || n != sizeof(AcceptConfig))
		return -2;
	
	return 0;
}

int newClientJoinSM(TCHAR username[],TCHAR username2[],int coop)
{
	Config c;
	_tcsncpy_s(c.username1, MAX_TAM, username, _TRUNCATE);
	if (coop)
	{
		_tcsncpy_s(c.username2, MAX_TAM, username2, _TRUNCATE);
		c.coop = 1;
	}
	else
		_tcsncpy_s(c.username1, MAX_TAM, TEXT("0"), _TRUNCATE);

	if (getStateSM() != 2) // Await Players
		return -1;

	CopyMemory((PVOID)pBufConfig, &c, sizeof(Config));
	return 0;
}

int newClientJoinNP(TCHAR username[],TCHAR username2[],int coop)
{
	Config c;
	DWORD n, fSuccess;
	_tcsncpy_s(c.username1, MAX_TAM, username, _TRUNCATE);
	if(coop)
	{
		_tcsncpy_s(c.username2, MAX_TAM, username2, _TRUNCATE);
		c.coop = 1;
	}
	else
		_tcsncpy_s(c.username1, MAX_TAM, TEXT("0"), _TRUNCATE);

	fSuccess = WriteFile(hServerPipe, &c, sizeof(Config), &n, NULL);
	if (!fSuccess || n != sizeof(Config))
		return -2;
	return 0;
}

int clientInputNP(TCHAR username[], int key, int exit)
{
	ClientInput in;
	DWORD n, fSuccess;
	_tcsncpy_s(in.username, MAX_TAM, username, _TRUNCATE);

	fSuccess = WriteFile(hServerPipe, &in, sizeof(ClientInput), &n, NULL);
	if (!fSuccess || n != sizeof(ClientInput))
		return -2;

	return 0;
}

void clientInputGameSM(int fireTimeoutTime, DWORD *timeout, TCHAR username[], int key, int exit)
{
	//*timeout = WaitForSingleObject(hSMClientCanRead, fireTimeoutTime);
	//ResetEvent(hSMClientCanRead);
	if (*timeout == WAIT_TIMEOUT)
	{
		*timeout = 1;
		return;
	}
	*timeout = 0;
	ClientInput temp;
	_tcsncpy_s(temp.username, MAX_TAM, username, _TRUNCATE);
	temp.key = key;
	temp.exit = exit;

	CopyMemory((PVOID)pBufClientInput, &temp, sizeof(ClientInput));
	SetEvent(hSMServerCanRead);
}

Info getRenderSM(int fireTimeoutTime, DWORD *timeout)
{
	Info info = {0};
	*timeout = WaitForSingleObject(hSMClientCanRetrieveRender, fireTimeoutTime);
	if (*timeout == WAIT_TIMEOUT)
	{
		*timeout = 1;
		return info;
	}
	*timeout = 0;

	CopyMemory(&info, (PVOID)pBufInfo, sizeof(Info));
	return info;
}

int getRenderNP(Info *info)
{
	info = { 0 };
	DWORD n, fSuccess;

	fSuccess = ReadFile(hServerPipe, info, sizeof(Info), &n, NULL);
	if (!fSuccess || n != sizeof(ClientInput))
		return -2;

	return 0;
}

//Bitmap
void loadCliBitmaps(HINSTANCE hInst, HBITMAP *b)
{

	//b[0] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
	//b[1] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_OBJ2));
	//b[2] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_OBJ3));
	//b[3] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_OBJ4));
	//b[4] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_OBJ5));
	//b[5] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_OBJ6));
	//b[6] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_OBJ7));
	//b[7] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_OBJ8));
	//b[8] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_OBJ9));
	//b[9] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_OBJ10));
}

//Circular Buffer Methods

/* CircularBuffer SetValue
*
*/
void cirBufInSM(int fireTimeoutTime, DWORD *timeout, TCHAR username[], int key, int exit)
{
	CircularBufferInfo cb = { 0 };
	ClientInput in = { 0 }, cirBuffer[CLI_IN_BUF_SIZE] = { 0 };
	*timeout = 0;
	//Input
	_tcsncpy_s(in.username, MAX_TAM, username, _TRUNCATE);
	in.key = key;
	in.exit = exit;

	// wait if there's no space left
	WaitForSingleObject(hSemSpaceCirBuf,fireTimeoutTime);
	if (*timeout == WAIT_TIMEOUT) { *timeout = 1; return; }
	//-----------------------------------------------------------------------
	*timeout = WaitForSingleObject(hMutexCirBuf, fireTimeoutTime);
	if (*timeout == WAIT_TIMEOUT) { *timeout = 2; return; }

	//Read CirBuf Index
	CopyMemory(&cb, (PVOID)pBufCircularBuffer, sizeof(CircularBufferInfo));

	//Read all buffer
	CopyMemory(&cirBuffer, (PVOID)pBufClientInput, sizeof(ClientInput) * CLI_IN_BUF_SIZE);
	cirBuffer[(cb.bufWriteIndex++) & (CLI_IN_BUF_SIZE - 1)] = in;

	if (cb.bufWriteIndex == CLI_IN_BUF_SIZE) cb.bufWriteIndex = 0;
	//Set value in buffer
	CopyMemory((PVOID)pBufClientInput, &cirBuffer, sizeof(ClientInput) * CLI_IN_BUF_SIZE);

	//Update CirBuf Index
	CopyMemory((PVOID)pBufCircularBuffer, &cb, sizeof(CircularBufferInfo));

	ReleaseMutex(hMutexCirBuf);
	//-------------------------------------------------------------------------
	// increment the count of the number of items
	ReleaseSemaphore(hSemCountCirBuf,1,NULL);
	Sleep(100);
}

/* CircularBuffer GetValue
*
*/
ClientInput cirBufOutSM(int fireTimeoutTime, DWORD *timeout)
{
	CircularBufferInfo cb = { 0 };
	ClientInput cirBuffer[CLI_IN_BUF_SIZE] = { 0 };
	ClientInput out = {0};
	*timeout = 0;
	// if there's nothing in the buffer
	*timeout = WaitForSingleObject(hSemCountCirBuf, fireTimeoutTime);
	if (*timeout == WAIT_TIMEOUT) { *timeout = 1; /*_tprintf(TEXT("TIMOUT 1\n\n\n"));*/ return out; }
	//-------------------------------------------------------------------------
	*timeout = WaitForSingleObject(hMutexCirBuf, fireTimeoutTime);
	if (*timeout == WAIT_TIMEOUT) { *timeout = 1; /*_tprintf(TEXT("TIMOUT 2\n\n\n"));*/ return out; }

	//Read CirBuf Index
	CopyMemory(&cb, (PVOID)pBufCircularBuffer, sizeof(CircularBufferInfo));

	//Read all buffer
	CopyMemory(&cirBuffer, (PVOID)pBufClientInput, sizeof(ClientInput) * CLI_IN_BUF_SIZE);
	out = cirBuffer[(cb.bufReadIndex++) & (CLI_IN_BUF_SIZE - 1)];
	if (cb.bufReadIndex == CLI_IN_BUF_SIZE) cb.bufReadIndex= 0;
	//Set value in buffer
	CopyMemory((PVOID)pBufClientInput, &cirBuffer, sizeof(ClientInput) * CLI_IN_BUF_SIZE);

	//Update CirBuf Index
	CopyMemory((PVOID)pBufCircularBuffer, &cb, sizeof(CircularBufferInfo));

	ReleaseMutex(hMutexCirBuf);
	//-------------------------------------------------------------------------
	// increment the count of the number of items
	ReleaseSemaphore(hSemSpaceCirBuf, 1, NULL);
	return out;
}
