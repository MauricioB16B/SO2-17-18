// InstalaSrv.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <tchar.h>

#define NOME_DO_SERVICO TEXT("SServNT")
#define CAMINHO TEXT("C:\\SnakeGameNT.exe")

int main(int argc, TCHAR *argv[])
{
	SC_HANDLE schSCManager, schService;
	schSCManager = OpenSCManager(
		NULL,                    // local machine 
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 

	if (schSCManager == NULL)
		_tprintf(TEXT("Erro: OpenSCManager (%d)\n"),GetLastError());


	TCHAR *lpszBinaryPathName = CAMINHO;

	schService = CreateService(
		schSCManager,              // SCManager database 
		NOME_DO_SERVICO,               // name of service 
		TEXT("aa SnakeGameISEC"),           // service name to display 
		SERVICE_ALL_ACCESS,        // desired access 
		SERVICE_WIN32_OWN_PROCESS, // service type 
		SERVICE_DEMAND_START,      // start type 
		SERVICE_ERROR_NORMAL,      // error control type 
		lpszBinaryPathName,        // service's binary 
		NULL,                      // no load ordering group 
		NULL,                      // no tag identifier 
		NULL,                      // no dependencies 
		NULL,                      // LocalSystem account 
		NULL);                     // no password 

	if (schService == NULL)
		_tprintf(TEXT("Error: CreateService (%d)\n"),GetLastError());
	else
		_tprintf(TEXT("CreateService SUCCESS.\n"));

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);

	_gettch();
	return 0;
}

