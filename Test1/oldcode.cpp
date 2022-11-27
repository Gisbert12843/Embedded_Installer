
/*

VOID __stdcall StartServiceIfPossible(std::string servicename)
{
	SERVICE_STATUS_PROCESS ssStatus;
	DWORD dwOldCheckPoint;
	DWORD dwStartTickCount;
	DWORD dwWaitTime;
	DWORD dwBytesNeeded;
	LPCWSTR szSvcName = std::wstring(servicename.begin(), servicename.end()).c_str();

	// Get a handle to the SCM database.
	SC_HANDLE schSCManager;
	schSCManager = OpenSCManagerW(
		NULL,                    // local computer
		NULL,                    // servicesActive database
		SC_MANAGER_ALL_ACCESS);  // full access rights

	if (NULL == schSCManager)
	{
		printf("OpenSCManager failed (%d)\n", GetLastError());
		return;
	}

	// Get a handle to the service.
	SC_HANDLE schService;

	schService = OpenServiceW(
		schSCManager,         // SCM database
		szSvcName,            // name of service
		SERVICE_ALL_ACCESS);  // full access

	if (schService == NULL)
	{
		printf("OpenService failed (%d)\n", GetLastError());
		CloseServiceHandle(schSCManager);
		return;
	}

	// Check the status in case the service is not stopped.

	if (!QueryServiceStatusEx(
		schService,                     // handle to service
		SC_STATUS_PROCESS_INFO,         // information level
		(LPBYTE)&ssStatus,             // address of structure
		sizeof(SERVICE_STATUS_PROCESS), // size of structure
		&dwBytesNeeded))              // size needed if buffer is too small
	{
		printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
		CloseServiceHandle(schService);
		CloseServiceHandle(schSCManager);
		return;
	}

	// Check if the service is already running. It would be possible
	// to stop the service here, but for simplicity this example just returns.

	if (ssStatus.dwCurrentState != SERVICE_STOPPED && ssStatus.dwCurrentState != SERVICE_STOP_PENDING)
	{
		printf("Cannot start the service because it is already running\n");
		CloseServiceHandle(schService);
		CloseServiceHandle(schSCManager);
		return;
	}

	// Save the tick count and initial checkpoint.

	dwStartTickCount = GetTickCount();
	dwOldCheckPoint = ssStatus.dwCheckPoint;

	// Wait for the service to stop before attempting to start it.

	while (ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
	{
		// Do not wait longer than the wait hint. A good interval is
		// one-tenth of the wait hint but not less than 1 second
		// and not more than 10 seconds.

		dwWaitTime = ssStatus.dwWaitHint / 10;

		if (dwWaitTime < 1000)
			dwWaitTime = 1000;
		else if (dwWaitTime > 10000)
			dwWaitTime = 10000;

		Sleep(dwWaitTime);

		// Check the status until the service is no longer stop pending.

		if (!QueryServiceStatusEx(
			schService,                     // handle to service
			SC_STATUS_PROCESS_INFO,         // information level
			(LPBYTE)&ssStatus,             // address of structure
			sizeof(SERVICE_STATUS_PROCESS), // size of structure
			&dwBytesNeeded))              // size needed if buffer is too small
		{
			printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
			CloseServiceHandle(schService);
			CloseServiceHandle(schSCManager);
			return;
		}

		if (ssStatus.dwCheckPoint > dwOldCheckPoint)
		{
			// Continue to wait and check.

			dwStartTickCount = GetTickCount();
			dwOldCheckPoint = ssStatus.dwCheckPoint;
		}
		else
		{
			if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint)
			{
				printf("Timeout waiting for service to stop\n");
				CloseServiceHandle(schService);
				CloseServiceHandle(schSCManager);
				return;
			}
		}
	}

	// Attempt to start the service.

	if (!StartService(
		schService,  // handle to service
		0,           // number of arguments
		NULL))      // no arguments
	{
		printf("StartService failed (%d)\n", GetLastError());
		CloseServiceHandle(schService);
		CloseServiceHandle(schSCManager);
		return;
	}
	else printf("Service start pending...\n");

	// Check the status until the service is no longer start pending.

	if (!QueryServiceStatusEx(
		schService,                     // handle to service
		SC_STATUS_PROCESS_INFO,         // info level
		(LPBYTE)&ssStatus,             // address of structure
		sizeof(SERVICE_STATUS_PROCESS), // size of structure
		&dwBytesNeeded))              // if buffer too small
	{
		printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
		CloseServiceHandle(schService);
		CloseServiceHandle(schSCManager);
		return;
	}

	// Save the tick count and initial checkpoint.

	dwStartTickCount = GetTickCount();
	dwOldCheckPoint = ssStatus.dwCheckPoint;

	while (ssStatus.dwCurrentState == SERVICE_START_PENDING)
	{
		// Do not wait longer than the wait hint. A good interval is
		// one-tenth the wait hint, but no less than 1 second and no
		// more than 10 seconds.

		dwWaitTime = ssStatus.dwWaitHint / 10;

		if (dwWaitTime < 1000)
			dwWaitTime = 1000;
		else if (dwWaitTime > 10000)
			dwWaitTime = 10000;

		Sleep(dwWaitTime);

		// Check the status again.

		if (!QueryServiceStatusEx(
			schService,             // handle to service
			SC_STATUS_PROCESS_INFO, // info level
			(LPBYTE)&ssStatus,             // address of structure
			sizeof(SERVICE_STATUS_PROCESS), // size of structure
			&dwBytesNeeded))              // if buffer too small
		{
			printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
			break;
		}

		if (ssStatus.dwCheckPoint > dwOldCheckPoint)
		{
			// Continue to wait and check.

			dwStartTickCount = GetTickCount();
			dwOldCheckPoint = ssStatus.dwCheckPoint;
		}
		else
		{
			if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint)
			{
				// No progress made within the wait hint.
				break;
			}
		}
	}

	// Determine whether the service is running.

	if (ssStatus.dwCurrentState == SERVICE_RUNNING)
	{
		printf("Service started successfully.\n");
	}
	else
	{
		printf("Service not started. \n");
		printf("  Current State: %d\n", ssStatus.dwCurrentState);
		printf("  Exit Code: %d\n", ssStatus.dwWin32ExitCode);
		printf("  Check Point: %d\n", ssStatus.dwCheckPoint);
		printf("  Wait Hint: %d\n", ssStatus.dwWaitHint);
	}

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
}

*/






























//
//
//{
//	//starts mysqld.exe if its not active atm
//	if ((isRunning("mysqld.exe")) == false)
//	{
//
//		std::cout << "mysqld.exe isnt running. starting it now.\n";
//		std::cin.
// ();
//
//		STARTUPINFO strtinfo = { sizeof(strtinfo) };
//		PROCESS_INFORMATION processInfo;
//		ZeroMemory(&strtinfo, sizeof(strtinfo));
//		//C:\Program Files\MariaDB 10.10\bin\mysqld.exe
//		std::string execute_order = "\"c:\\Program Files\\MariaDB 10.10\\bin\\mysqld.exe\"";
//		std::wstring execute_order_ws;
//		execute_order_ws.assign(execute_order.begin(), execute_order.end());
//		LPWSTR pwst = &execute_order_ws[0];
//
//		if (CreateProcessW(NULL, pwst, NULL, NULL, FALSE, 0, NULL, NULL, &strtinfo, &processInfo))
//		{
//			std::cout << "Starting MariaDB SQL Service EXE.\n";
//			std::cin.ignore();
//			while (WaitForSingleObject(processInfo.hProcess, INFINITE))
//			{
//				std::cout << ".";
//				Sleep(800);
//			}
//			CloseHandle(processInfo.hProcess);
//			CloseHandle(processInfo.hThread);
//			std::cout << "Closed Handle for SQL Service EXE.\n";
//			std::cin.ignore();
//
//		}
//		else
//		{
//			std::cout << "Couldnt Start mysqld.exe.\n";
//			std::cin.ignore();
//
//			if (IDCANCEL == createWindowsError("Couldnt Start mysqld.exe."))
//			{
//				killProgrammMessage();
//			}
//			else
//			{
//				createWindowsMessage("OK lets try this again then.");
//				CheckMariaDB();
//			}
//		}
//	}
//		}
