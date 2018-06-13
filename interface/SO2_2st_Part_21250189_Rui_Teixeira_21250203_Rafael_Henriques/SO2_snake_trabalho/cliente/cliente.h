#pragma once
#pragma once

#include "snake_dll.h"
#include "resource.h"

//ClientGUI
#define MAIN_WND_X 700 
#define MAIN_WND_Y 500
#define MAIN_WND_START_X 40
#define MAIN_WND_START_Y 40
#define SQUARE_SIZE_X 15
#define SQUARE_SIZE_Y 15
DWORD WINAPI recvDataSM(LPVOID *param);
LRESULT CALLBACK ActionListener(HWND, UINT, WPARAM, LPARAM); // state 3
LRESULT CALLBACK RunListener(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam); // state 0
LRESULT CALLBACK ConfigListener(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam); // state 1
LRESULT CALLBACK JoinListener(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);// state 2
LRESULT CALLBACK ChangePictureListener(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void renderGame(HDC d, Info r);
void drawImage(HDC d, int objIndex, Info r);
void drawSnake(HDC d, Snake s, int r, int g, int b, int headr, int headg, int headb);
void drawImageInBitmapEditor(HDC d, int locx, int locy, int index);

// Nome da classe da janela (para programas de uma só janela, normalmente este nome é 
// igual ao do próprio programa) "szprogName" é usado mais abaixo na definição das 
// propriedades do objecto janela
TCHAR *szProgName = TEXT("Base");





typedef struct {
	TCHAR username[MAX_TAM];
	WORD keyUp;	// -> 1 
	WORD keyDown;	// -> 2
	WORD keyRight;	// -> 3
	WORD keyLeft;	// -> 4
}ClientInfo;

int canLaunchRun;
int canLaunchConfig;
int canLaunchJoinGame;
int canLaunchAwaitPlayers;
int canLaunchGame;
int canLaunchChangePicture;
int coop;
int turnOffCli;
int clientIndexRender[MAX_PLAYERS_SAME_CLIENT];
ClientInfo cliKeys[MAX_PLAYERS_SAME_CLIENT];
Config config;
Info render;
HANDLE hEvent;
HANDLE hMutex;
DWORD threadID;
HANDLE hRecv;
HWND hMain;

//frases erro
TCHAR name1small[] = TEXT("Player 1's name is too small.");
TCHAR name1big[] = TEXT("Player 1's name is too big.");

typedef struct
{
	HWND hWnd;
	HANDLE hMutex;
}RecvThreadParam;

int kindConnection; // 1 - SM // 2 - NP