// DesinstalaSrv.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <tchar.h>

#define NOME_DO_SERVICO TEXT("SServNT")

int main(int argc, TCHAR *argv[])
{
	SC_HANDLE schSCManager, schService;
	schSCManager = OpenSCManager(
		NULL,                    // local machine 
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 

	if (schSCManager == NULL)
		_tprintf(TEXT("Erro: OpenSCManager (%d)\n"),GetLastError());

	schService = OpenService(
		schSCManager,       // SCManager database 
		NOME_DO_SERVICO,       // name of service 
		DELETE);            // only need DELETE access 

	if (schService == NULL)
		_tprintf(TEXT("Error: OpenService (%d)\n"),GetLastError());

	if (!DeleteService(schService))
		_tprintf(TEXT("Error: DeleteService (%d)\n"),GetLastError());
	else
		_tprintf(TEXT("DeleteService SUCCESS\n"));

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);

	_gettch();
	return 0;
}

