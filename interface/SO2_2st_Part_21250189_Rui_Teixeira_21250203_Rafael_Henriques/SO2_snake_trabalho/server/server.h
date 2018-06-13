#pragma once
#pragma comment(lib, "DLL.lib")
#include "snake_dll.h"

//Server game config info
#define SNAKE_SPEED 3
#define SNAKE_SPEED_TIME 250
#define LOW_EFFECT_TIME 10
#define MEDIUM_EFFECT_TIME 20
#define HIGH_EFFECT_TIME 35
#define LOW_ITEM_FREQUENCY_TIME 15000
#define MEDIUM_ITEM_FREQUENCY_TIME 10000
#define HIGH_ITEM_FREQUENCY_TIME 5000

// Server Handles (Syncs and Threads)
// Server Thread Handles are not needed, but we let them anyway
HANDLE hRecvFromCli;
HANDLE hSendToCli;
HANDLE hNewNPCliThread;
HANDLE hReadCli[MAX_PLAYERS];
HANDLE hSendCli[MAX_PLAYERS];
HANDLE hObjThread[MAX_OBJECTS];
HANDLE hPlayerThread[MAX_PLAYERS];
HANDLE hBotsThread;
HANDLE hSnakeMutex;
HANDLE hMutexObjs;
HANDLE hEventSendInfo;

//Functions 
Config config;
Info info;

int requestKey[MAX_PLAYERS]; // if number is out of range (1-4) player doesn't clicked a new key

DWORD WINAPI recvFromCli(LPVOID *param);
DWORD WINAPI sendToCli(LPVOID *param);
DWORD WINAPI botsIA(LPVOID *param);
BOOL WINAPI ConsoleHandlerRoutine(DWORD dwCtrlType);
DWORD WINAPI objectRespawn(LPVOID *param);
DWORD WINAPI playerSnakeThread(LPVOID *param);
int RangedRandDemo(int range_min, int range_max);
void updateNewObj(int objIndex, int objType);
void checkAndEatObject(int pIndex, int nextX, int nextY, int oldX, int oldY);
/*@return   -1 Username is not valid
*			 1 Updated Info with Success
*			 2 Snake Died, hit another snake
*/
int moveSnake(int key, int pIndex);
void setUpRandomDirAndCoords();
int objInNeighbor(int sIndex);
// Food - raise snake size
void riseSnakeSize(int pIndex, int objIndex, int headX, int headY, int oldX, int oldY);
// Ice - shrink snake size
void obj2Collected(int pIndex, int objIndex);
// Grenade - kill snake
void obj3Collected(int pIndex, int objIndex);
// Vodka - Invert Keys
void obj4Collected(int pIndex, int objIndex);
// Oil - Raise speed
void obj5Collected(int pIndex, int objIndex);
// Glue - Decrease speed
void obj6Collected(int pIndex, int objIndex);
// O-Vodka
void obj7Collected(int pIndex, int objIndex);
// O-oil
void obj8Collected(int pIndex, int objIndex);
// O-Glue
void obj9Collected(int pIndex, int objIndex);
// Snake Cutted in Half - ButcheryKnife
void obj10Collected(int pIndex, int objIndex);
int invertKey(int key);
int setUpSyncs();
void closeSyncs();
int setUpThreads_State3();
void closeThreadHandles_State3();
int gameEnd();
void print(TCHAR *p);
int nearestItem(int x, int y);
// @param 
// in - client input
// @return
//			-1 - if player died or is offline
//			Index value in Info struct 
int SnakeIndex(ClientInput in);
void updateState(int st);
void updateCliOn(int moreNP, int moreSM);

int breakMainCicle;
TCHAR gameCreator[MAX_TAM];



typedef struct
{
	HANDLE hThread;
	int botIndex;
}BotThread;
BotThread botParam[MAX_SNAKE_BOTS];
int objID[MAX_OBJECTS];
int playerIndex[MAX_PLAYERS];





// Regestry
HKEY regKey;
#define AUTHOR1 TEXT("Rui Teixeira")
#define STUDENT_NUM1 TEXT("a21250189")
#define AUTHOR2 TEXT("Rafael Henriques")
#define STUDENT_NUM2 TEXT("a21250203")
#define REG_AUTHOR TEXT("Authors")
#define REG_SCORE TEXT("Score")
#define REG_SCORE_NUM_PLAYER TEXT("RegistedPlayers")
typedef struct
{
	TCHAR name[MAX_TAM];
	TCHAR num[MAX_TAM];
}Author;
typedef struct
{
	TCHAR username[MAX_PLAYERS];
	int score;
}Score;
/* @return  0 success
*		    -1 error
*/			
int SetUpRegistry(Info f);
void updateRegistry(Snake p);
void printRegistry();

