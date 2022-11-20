//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#include <windows.h>
#include <iostream>
#include <cstdio>
#include "resource.h"
#include "resource1.h"
#include "resource2.h"
#include <fstream>
#include <cstdint>
#include <shlwapi.h>
#include <filesystem>
#include <stdlib.h>     //for using the function sleep
#include <tchar.h>
#include <winuser.h>
#include <string>
#include <processthreadsapi.h>



#pragma comment(lib, "Shlwapi.lib")


void LoadFileInResource(int name, int type, DWORD& size, const char*& data)
{
	HMODULE handle = ::GetModuleHandle(NULL);
	HRSRC rc = ::FindResource(handle, MAKEINTRESOURCE(name),
		MAKEINTRESOURCE(type));
	HGLOBAL rcData = ::LoadResource(handle, rc);
	size = ::SizeofResource(handle, rc);
	data = static_cast<const char*>(::LockResource(rcData));
}

std::string fixPath(std::string theString)
{
	std::string returnedString="";
	for (int it=0; it<theString.length(); it++)
	{
		if (theString.at(it) != '/')
		returnedString += theString.at(it);
		else returnedString += '\\';
	}
	return returnedString;
}


void createWindowsMessage(std::string thestring)
{
	std::wstring message (thestring.begin(), thestring.end());
	
	MessageBoxW(NULL, message.c_str(), L"HINWEIS", MB_OK | MB_ICONQUESTION);
}



int  _tmain(int argc, _TCHAR* argv[])
{
	FreeConsole();

	//to hide console include a quick flicker at the start
	//::ShowWindow(::GetConsoleWindow(), SW_HIDE);

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);


	const std::string cwd = std::filesystem::current_path().string();

	std::cout << cwd << "\n";

	{//CHECK INSTALLTION STATUS OF MARIADB

		if (!std::filesystem::exists("\%ProgramFiles\%\\MariaDB_MehrMarkt\\bin\\mariadb.exe"))
		{
			HRSRC hResource = FindResource(NULL, MAKEINTRESOURCE(IDB_EMBEDEXE1), __TEXT("BINARY"));
			HGLOBAL hGlobal = LoadResource(NULL, hResource);
			size_t exeSiz = SizeofResource(NULL, hResource);
			void* exeBuf = LockResource(hGlobal);

			DWORD at1;
			HANDLE hFile = CreateFile(L"mariadb_installer.msi", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); // Check MSDN for more info
			BOOL success = WriteFile(hFile, exeBuf, exeSiz, &at1, NULL); // If it fails  -> success = 0

			CloseHandle(hFile);

			std::string thepath = (cwd + "\\mariadb_installer.msi");

			std::cout << thepath << "\n";
			do {
				if (success && std::filesystem::exists(thepath))
				{
					createWindowsMessage("Starting MariaDB Setup.\nProject will open automatically.");
					std::cout << "Starting MariaDB Setup\n";


					//HERE COMES THE EXECUTION CODE


					// xampp_installer.exe --unattendedmodeui none --disable-components xampp_filezilla xampp_mercury xampp_tomcat xampp_perl xampp_webalizer xampp_sendmail
					//auto result = system("MsiExec.exe /i mariadb_installer.msi INSTALLDIR=\%ProgramFiles\%\\MariaDB_MehrMarkt DEFAULTUSER=root PASSWORD=toor SERVICENAME=Mehr_Markt_Database-Service PORT=42069 UTF8 /qn");
					break;
				}

				std::cout << "Waiting for MariaDB-Setup to be ready.";

				while (success == 0 || !std::filesystem::exists(thepath))
				{
					std::cout << ".";
					Sleep(800);
				}

			} while (!std::filesystem::exists(thepath));
		}
		else
		{
			std::cout << "maria is there\n";
		}
	}

	{//CHECK INSTALLATION STATUS OF XAMPP		

		if (!std::filesystem::exists("C:\\xampp\\xampp-control.exe"))
		{
			HRSRC hResource = FindResource(NULL, MAKEINTRESOURCE(IDB_EMBEDEXE2), __TEXT("BINARY"));
			HGLOBAL hGlobal = LoadResource(NULL, hResource);
			size_t exeSiz = SizeofResource(NULL, hResource);
			void* exeBuf = LockResource(hGlobal);

			DWORD at1;
			HANDLE hFile = CreateFile(L"xampp_installer.exe", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); // Check MSDN for more info
			BOOL success = WriteFile(hFile, exeBuf, exeSiz, &at1, NULL); // If it fails  -> success = 0

			CloseHandle(hFile);


			std::string thepath = (cwd + "\\xampp_installer.exe");
			do {
				if (success && std::filesystem::exists(thepath))
				{
					std::cout << "Starting xampp Setup\n";



					STARTUPINFO strtinfo = { sizeof(strtinfo) };
					PROCESS_INFORMATION processInfo;
					ZeroMemory(&strtinfo, sizeof(strtinfo));

					// --unattendedmodeui none --disable-components xampp_filezilla,xampp_mercury,xampp_tomcat,xampp_perl,xampp_webalizer,xampp_sendmail
					std::string execute_order = thepath + " --mode unattended --disable-components xampp_filezilla,xampp_mercury,xampp_tomcat,xampp_perl,xampp_webalizer,xampp_sendmail";
					std::wstring execute_order_ws;
					execute_order_ws.assign(execute_order.begin(), execute_order.end());
					LPWSTR pwst = &execute_order_ws[0];

					if (CreateProcessW(NULL, pwst, NULL, NULL, FALSE, 0, NULL, NULL, &strtinfo, &processInfo))
					{
						std::cout << "gotinside!\n";
						std::cout << "Waiting for XAMPP Setup to finnish.\n";

						while (WaitForSingleObject(processInfo.hProcess, INFINITE))
						{
							std::cout << ".";
							Sleep(800);
						}
						CloseHandle(processInfo.hProcess);
						CloseHandle(processInfo.hThread);

						

						std::cout << "\nYUHU!\n";

					}
					else
						std::cout << "didnt getinside!\n";

					//ShellExecuteA(NULL, "runas", "xampp_installer.exe --unattendedmodeui none --disable-components xampp_filezilla xampp_mercury xampp_tomcat xampp_perl xampp_webalizer xampp_sendmail", NULL, NULL, 0);
					//HEAR COMES EXECUTION CODE FOR XAMPP SETUP

					break;
				}

				std::cout << "Waiting for XAMPP-Setup to be ready.\n";

				while (!success || !std::filesystem::exists(thepath))
				{
					std::cout << ".";
					Sleep(800);
				}
				
			} while (!std::filesystem::exists(thepath));
		}
		else
		{
			std::cout << "xampp is there\n";

		}
	}/*
	{
		std::cout << "Updating Path-Variables!\n";
		std::cout << "\n" << cwd << "\n";
		std::string env = "cmd /c set PATH=\"\%PATH\%;C:\\xampp\\php\"";
		std::wstring env_w;

		env_w.assign(env.begin(), env.end());
		LPWSTR env_final = &env_w[0];



		STARTUPINFO strtinfo = { sizeof(strtinfo) };
		PROCESS_INFORMATION processInfo;
		ZeroMemory(&strtinfo, sizeof(strtinfo));
		//L"C:\\Windows\\system32\\cmd.exe"
		if (CreateProcessW(NULL, env_final, NULL, NULL, FALSE, 0, NULL, NULL, &strtinfo, &processInfo))
		{
			while (WaitForSingleObject(processInfo.hProcess, INFINITE))
			{
				std::cout << ".";
				Sleep(800);
			}

			//TerminateProcess(&processInfo, NULL);

			CloseHandle(processInfo.hProcess);
			CloseHandle(processInfo.hThread);
			std::cout << "\nUpdated Path\n";

		}
		else
			std::cout << "\nCouldnt update Path\n";
	}*/
	
	{
		HRSRC hResource = FindResource(NULL, MAKEINTRESOURCE(IDB_PHP), __TEXT("BINARY"));
		HGLOBAL hGlobal = LoadResource(NULL, hResource);
		size_t exeSiz = SizeofResource(NULL, hResource);
		void* exeBuf = LockResource(hGlobal);
		std::cout << "\n" << cwd << "\n";

		DWORD at1;
		HANDLE hFile = CreateFile(L"index.php", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); // Check MSDN for more info
		BOOL success = WriteFile(hFile, exeBuf, exeSiz, &at1, NULL); // If it fails  -> success = 0

		CloseHandle(hFile);

		{
			std::cout << "\nSetting Up Path and Starting Localhost\n";
			//set PATH=%PATH%;C:\xampp\php\
			//L"C:\\Windows\\system32\\cmd.exe"  && php -S localhost:8000
			// + cwd + "\\index.php"
			std::string thepath = "cmd.exe /c set PATH=\%PATH\%;C:\\xampp\\php\\ && start /b php -S localhost:8000 && start /b http://localhost:8000";
			std::cout << "IST:" << thepath;
			std::wstring thepath_w;

			thepath_w.assign(thepath.begin(), thepath.end());
			LPWSTR thepath_final = &thepath_w[0];

			std::cout << "\n2\n";
			STARTUPINFO strtinfo = { sizeof(strtinfo) };
			PROCESS_INFORMATION processInfo;
			ZeroMemory(&strtinfo, sizeof(strtinfo));


			if (CreateProcess(NULL, thepath_final, NULL, NULL, FALSE, 0, NULL, NULL, &strtinfo, &processInfo))
			{
				while (WaitForSingleObject(processInfo.hProcess, INFINITE))
				{
					std::cout << ".";
					Sleep(800);
				}
				CloseHandle(processInfo.hProcess);
				CloseHandle(processInfo.hThread);
				std::cout << "\nStarted Server\n";

			}
			else
				std::cout << "\nDidnt start Server\n";

			std::cout << "\n3\n";
		}
	}

	std::cout << "\n4\n";

	std::cin.ignore();

	return 0;
}


















//DWORD size = 0;
//const char* data = NULL;
//const int BUFFERSIZE = 4096;


//LoadFileInResource(IDR_MYTEXTFILE1, TEXTFILE1, size, data);

//std::ofstream newExe1;
//newExe1.open("mariadb_installer.msi", std::ios::app | std::ios::binary);
//char* buffer = new char[size + 1];
//::memcpy(buffer, data, size);
//buffer[size] = 0; // NULL terminator
//newExe1 << buffer;
//delete[] buffer;
//newExe1.close();

	//LoadFileInResource(IDR_MYTEXTFILE2, TEXTFILE2, size, data);

//std::ofstream newTxt2 ("xampp_install.exe");
//char* buffer2 = new char[size + 1];
//::memcpy(buffer2, data, size);
//buffer2[size] = 0; // NULL terminator
//newTxt2 << buffer2;
//delete[] buffer2;
//newTxt2.close();