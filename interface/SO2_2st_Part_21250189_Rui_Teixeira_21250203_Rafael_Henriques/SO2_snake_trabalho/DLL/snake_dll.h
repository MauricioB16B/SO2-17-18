#pragma once
#include <stdio.h>
#include <io.h>
#include <Windows.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <tchar.h>
#include <conio.h>
#include <stdlib.h>

// String size
#define MAX_TAM 60

// Parameters Config
#define MIN_NAME_SIZE 3
#define MAX_NAME_SIZE 20
#define MIN_FIELD_X 20
#define MAX_FIELD_X 80
#define	MIN_FIELD_Y 20
#define MAX_FIELD_Y 50
#define MIN_INITIAL_SNAKE_SEGMENTS 3
#define MAX_INITIAL_SNAKE_SEGMENTS 10
#define MIN_SNAKE_BOTS 1
#define MAX_SNAKE_BOTS 18
#define MIN_ITEMS_FREQUENCY 1
#define MAX_ITEMS_FREQUENCY 3
#define MIN_OBJECTS 10
#define MAX_OBJECTS 30
#define MIN_EFFECT_TIME 1
#define MAX_EFFECT_TIME 3
#define OBJ_TYPES 10

// more Parameters
#define MAX_SNAKE_SIZE 200
#define MAX_SNAKES 20
#define MAX_PLAYERS 18
#define MAX_MAP_OBSTACLES 800
#define MAX_BUFFS 10
#define MAX_NAME_LENGHT 60
#define MAX_PLAYERS_SAME_CLIENT 2

#define KEY_UP 1
#define KEY_DOWN 2
#define KEY_RIGHT 3
#define KEY_LEFT 4

typedef struct
{
	int structVal; // 1
				   //if username "0" it has no players
	TCHAR username1[MAX_TAM];
	TCHAR username2[MAX_TAM];
	int coop;					// 1 - 0
	int hasConfig;				// Check if there's a config via Shared Memory
	int fieldSizeX;				// 20 - 80
	int fieldSizeY;				// 20 - 80								
	int snakeInitialSegmentSize;// 3 - 8
	int numSnakeBots;			// 1 - 19
	int numMaxPlayers;			// 0 - 18
	int itemsFrequency;			// 1 - 3	
	int numObjects;				// 10 - 30
	int temporaryEffectTime;	// 5 - 20
	int withObjects;			// TRUE - FALSE (1-0)
	int isOnline;				// 1 - remote game
	int giveup;					// 1 - stop config  
}Config;

TCHAR AcceptConfig[MAX_TAM]; // 2

typedef struct
{
	int structVal; // 3
				   //if username "0" it has no players
	TCHAR username[MAX_TAM];
	// 1 -> up | 2 -> down | 3 -> right | 4 -> left | -1 -> no player
	int key;
	// exit = 1 -> exit
	// exit = 0 -> remain  
	int exit;
}ClientInput;

typedef struct
{
	int sizeX, sizeY;						// Map Size
	int numObs;
	int obstacles[MAX_MAP_OBSTACLES][2];	// (x,y) coords
	int numObjs;
	// 1- Food; 2- Ice; 3- Grenade; 4- Vodka; 5- Oil; 6- Glue; 
	// 7- O-Vodka; 8- O-Oil; 9- O-Glue; 10- /////////////////Our Obj
	int objType[MAX_OBJECTS];
	int object[MAX_OBJECTS][2];	// (x,y) coords
}Field;

typedef struct
{
	TCHAR username[MAX_TAM];
	int snakeSize;							// snake size in segments
	int coords[MAX_SNAKE_SIZE][2];			// (x,y) coords
	int invertKeys;							// invert input keys
	int oil;								// raise speed
	int glue;								// decrease speed
	int died;								// 1 - died
	int points;								// Current pontuation
	int currentDirection;					// 1-Up 2-Down 3-Right 4-Left
}Snake;

typedef struct
{
	int structVal; // 4
	int isOnline;							// is an online game
	int serverTurnedOff;					// server turned off
	int gameEnded;							// game ended
	Field field;							// field description to print in GUI (server Data also)
	int numPlayers;							// num Players
	int clientsOnline;
	int numBots;							// num Bot Snakes
	Snake snake[MAX_SNAKES];				// Snakes description to print in GUI (server Data also)
}Info;

typedef struct
{
	int state; // 0 - AwaitSomeone // 1 - Config // 2 - AwaitPlayersConnection // 3 - gameRunning 
	int cliOnThoughtSM; // 1 - SM is beings used by a player // 0 - SM is not being used
	int cliOnThoughtNP; // 0 - 18
	TCHAR cliNamedPipePath[MAX_PLAYERS];
}StateInfo;
StateInfo state;

#define BUF_SIZE sizeof(Info)
// Shared Memory
#define SM_SIZE 200000 // (0,2 Mb)
//#define SM_SIZE (sizeof(Config) + sizeof(Info) + (sizeof(ClientInput) * CLI_IN_BUF_SIZE) + (sizeof(TCHAR) * MAX_TAM) + sizeof(StateInfo) + 100)
#define sharedMemName TEXT("\sm\SharedMemory")
HANDLE hMapFile;
LPCTSTR pBufConfig, pBufAcceptConfig, pBufClientInput, pBufInfo, pBufStateInfo, pBufCircularBuffer;

// Shard Memory Syncronizaton
#define SMClientCanReadName TEXT("\sycn\SM_Client_Can_Read")
#define SMServerCanReadName TEXT("\sync\SM_Server_Can_Read")
#define SMClientCanRetrieveRenderName  TEXT("\sycn\SM_Client_Can_Retrieve_Render")
#define PATH_PIPE_SERVER TEXT("\\\\%s\\pipe\\snakegame")
#define SERVER_IP TEXT(".")
HANDLE hSMClientCanRead, hSMServerCanRead, hSMClientCanRetrieveRender;

// Buffer 
#define CLI_IN_BUF_SIZE 16 //(must be 2^i)
typedef struct
{
	int bufReadIndex;
	int bufWriteIndex;
}CircularBufferInfo;
#define SemaReadCirBufPath TEXT("\sycn\SemaphoreReadCirBuf")
#define SemaWriteCirBufPath TEXT("\sycn\SemaphoreWriteCirBuf")
#define MutexCirBufPath TEXT("\sycn\MutexCirBuf")
HANDLE hSemSpaceCirBuf;
HANDLE hSemCountCirBuf;
HANDLE hMutexCirBuf;

// 1- Food; 2- Ice; 3- Grenade; 4- Vodka; 5- Oil; 6- Glue; 
// 7- O-Vodka; 8- O-Oil; 9- O-Glue; 10- /////////////////Our Obj
// Bitmaps
HBITMAP hBmp[10];

//NamedPipe Communication
int gameRunning;
int serverRunning;
HANDLE hThreadRcvCli;
HANDLE hCliPipes[MAX_PLAYERS];
HANDLE hThListenCli[MAX_PLAYERS];
HANDLE hServerPipe; // Pipe used by client

#ifdef DLL_EXPORTS
#define DLL_IMP_API __declspec(dllexport)
#else
#define DLL_IMP_API __declspec(dllimport)
#endif

extern "C"
{
	//Variável global da DLL
	//extern DLL_IMP_API int nDLL;

	//Funções a serem exportadas/importadas
	// Server ones
	DLL_IMP_API int serverCreateSM();
	DLL_IMP_API void cleanUp(PSID pEveryoneSID, PSID pAdminSID, PACL pACL, PSECURITY_DESCRIPTOR pSD);
	DLL_IMP_API	void closeSM();
	DLL_IMP_API void setStateServer(StateInfo state);
	DLL_IMP_API StateInfo getStateServer();
	DLL_IMP_API int setNoCliInSharedMemState();
	DLL_IMP_API int setCliInSharedMemState();
	DLL_IMP_API int getMyIP(TCHAR *ip);
	/* @return 0  - return with success
	*		   -1 - error setting up sa
	*/
	DLL_IMP_API DWORD WINAPI recvNewCliConnection(LPVOID *param);
	DLL_IMP_API DWORD WINAPI recvCliInfo(LPVOID *param);
	/*Has 20 in lenght*/
	DLL_IMP_API	Config retrieveConfigSM(int fireTimeoutTime, DWORD *timeout);
	DLL_IMP_API void updateAcceptConfigSM(TCHAR acceptConfig[]);
	DLL_IMP_API void retrieveClientInputSM(int fireTimeoutTime, DWORD *timeout, ClientInput cliIn[]);
	DLL_IMP_API void updateRenderSM(Info info);
	//Client Ones
	/* @return	0 - Could not open shared memory (server offline)
	*		   -1 - This was not supposed to happed
	*			1 - Connected to server
	*/
	DLL_IMP_API	int clientOpenAccessSM(int *state);
	/* @return	return state index in success
	*		   -1 - In case of error check errorLog
	*/
	DLL_IMP_API	int clientOpenAccessNP(TCHAR *ip, TCHAR *username, TCHAR *password, TCHAR *errorLog);
	/*  @return	 state
	*
	*/
	DLL_IMP_API int getStateSM();
	/* @return	 0 - succeeded
	*			-1 - its not in state config
	*			-2 - error receiving message to server
	*/
	DLL_IMP_API int getAcceptConfigNP(TCHAR returnLog[]);
	/* @return	 1 - succeeded (next state)
	*			 0 - read returnLog - some refused info in config info
	*			-1 - timeout
	*/
	DLL_IMP_API int getAcceptConfigSM(int fireTimeoutTime, DWORD *timeout, TCHAR returnLog[]);
	DLL_IMP_API void clientSetConfigSM(Config config);
	DLL_IMP_API int onePlayerConnectSM();
	DLL_IMP_API int newClientJoinSM(TCHAR username[], TCHAR username2[], int coop);
	/* @return	 0 - succeeded
	*			-1 - its not in state waiting players
	*			-2 - error sending message to server
	*/
	DLL_IMP_API int newClientJoinNP(TCHAR username[], TCHAR username2[], int coop);
	DLL_IMP_API int newClientJoinSM(TCHAR username[], TCHAR username2[], int coop);
	DLL_IMP_API void clientInputGameSM(int fireTimeoutTime, DWORD *timeout, TCHAR username[], int key, int exit);
	/* @return	0 - succeeded
	*		   -1 - its not in state game running
	*		   -2 - error sending message to server
	*/
	DLL_IMP_API int clientInputNP(TCHAR username[], int key, int exit);
	DLL_IMP_API Info getRenderSM(int fireTimeoutTime, DWORD *timeout);
	/* @return	0 - succeeded
	*		   -1 - its not in state game running
	*		   -2 - error receiving error info
	*/
	DLL_IMP_API int getRenderNP(Info *info);
	DLL_IMP_API void loadCliBitmaps(HINSTANCE hInst, HBITMAP *b);
	//DLL_IMP_API BOOL WINAPI DllMain(HANDLE hInstance, DWORD razao, LPVOID tipo_declaracao);
	//CircularBuffer
	DLL_IMP_API void cirBufInSM(int fireTimeoutTime, DWORD *timeout, TCHAR username[], int key, int exit); // Client
	DLL_IMP_API ClientInput cirBufOutSM(int fireTimeoutTime, DWORD *timeout); // Server
}