#include "installer.h"

void install_maria(std::string sourcepath, std::string &username, std::string &user_password, std::string &port)
{

	std::cout << "\nSTARTED MARIADB INSTALLATION\n";

	//this loads the installer from the embedded resources
	HRSRC hResource = FindResource(NULL, MAKEINTRESOURCE(IDB_EMBEDEXE1), __TEXT("BINARY"));
	HGLOBAL hGlobal = LoadResource(NULL, hResource);
	size_t exeSiz = SizeofResource(NULL, hResource);
	void* exeBuf = LockResource(hGlobal);

	DWORD at1;
	HANDLE hFile = CreateFile(L"mariadb_installer.msi", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); // Check MSDN for more info
	BOOL success = WriteFile(hFile, exeBuf, exeSiz, &at1, NULL); // If it fails  -> success = 0


	CloseHandle(hFile);

	if (!success)
	{
		createWindowsError("couldn't unpack installer file for mariadb, though it's needed to install mariadb\d\nProgram will now Exit.");
		exit;
	}

	std::string thepath = (sourcepath + "\\mariadb_installer.msi");

	//std::cout << thepath << "\n";
	do {
		if (std::filesystem::exists(thepath))
		{
			//createWindowsMessage("Starting MariaDB Setup.\nProject will open automatically.");
			std::cout << "Starting MariaDB Setup\n";

			STARTUPINFO strtinfo = { sizeof(strtinfo) };
			PROCESS_INFORMATION processInfo;
			ZeroMemory(&strtinfo, sizeof(strtinfo));

			//mysql -h localhost -u root -p toor
			// 
			// 
			//msiexec /i mariadb_installer.msi SERVICENAME=MariaDB PORT=42069 PASSWORD=toor /qn
			// 
			// 
			// INSTALLDIR=\%ProgramFiles\%\\MariaDB_MehrMarkt DEFAULTUSER=root PASSWORD=toor SERVICENAME=Mehr_Markt_Database-Service PORT=42069 UTF8 
			std::string execute_order = "msiexec /i mariadb_installer.msi SERVICENAME=MariaDB PORT=" + port + " PASSWORD=" + user_password + " UTF8=1 /qn";
			std::wstring execute_order_ws;
			execute_order_ws.assign(execute_order.begin(), execute_order.end());
			LPWSTR pwst = &execute_order_ws[0];

			if (CreateProcessW(NULL, pwst, NULL, NULL, FALSE, 0, NULL, NULL, &strtinfo, &processInfo))
			{
				std::cout << "Installing MariaDB.\n";
				createWindowsMessage("Starting MariaDB Setup. Please be patient. This will take a few minutes. \nProject will open automatically.");

				while (WaitForSingleObject(processInfo.hProcess, INFINITE))
				{
					std::cout << ".";
					Sleep(800);
				}
				CloseHandle(processInfo.hProcess);
				CloseHandle(processInfo.hThread);;
			}
			else
			{
				//std::cout << "couldnt start Maria Setup!\n";
				createWindowsMessage("Couldn't start MariaDB-Setup, though its needed for the Software to run! Try starting the Software from a different location. Programm will exit now.");
				exit;
			}


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
	std::filesystem::remove(thepath);

}



void install_xampp(std::string projectpath)
{
	HRSRC hResource = FindResource(NULL, MAKEINTRESOURCE(IDB_EMBEDEXE2), __TEXT("BINARY"));
	HGLOBAL hGlobal = LoadResource(NULL, hResource);
	size_t exeSiz = SizeofResource(NULL, hResource);
	void* exeBuf = LockResource(hGlobal);

	DWORD at1;
	HANDLE hFile = CreateFile(L"xampp_installer.exe", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); // Check MSDN for more info
	BOOL success = WriteFile(hFile, exeBuf, exeSiz, &at1, NULL); // If it fails  -> success = 0

	CloseHandle(hFile);

	if (!success)
	{
		createWindowsMessage("couldn't unpack installer file for xampp, though it's needed to install the xampp php interpreter");
	}

	std::string thepath = (projectpath + "\\xampp_installer.exe");
	do {
		if (success && std::filesystem::exists(thepath))
		{
			std::cout << "Starting xampp Setup\n";
			createWindowsMessage("Starting XAMPP Setup. Please be patient. This will take a few minutes. \nProject will open automatically.");

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
				std::cout << "Waiting for XAMPP Setup to finnish.\n";

				while (WaitForSingleObject(processInfo.hProcess, INFINITE))
				{
					std::cout << ".";
					Sleep(800);
				}
				CloseHandle(processInfo.hProcess);
				CloseHandle(processInfo.hThread);
			}
			else
				std::cout << "couldnt start xampp setup!\n";

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
	std::filesystem::remove(thepath);
}