//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#include "wholeinclude.h"
#include "resource.h"
#include "resource1.h"
#include "resource2.h"
#include "helper_functions.h"

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "advapi32.lib")

std::string project_path;
std::string mariaspath = "";
std::string servicename = "MariaDB";
std::string username = "root";
std::string user_password = "toor";
std::string port = "42069";
bool config_did_work = true;
bool choose_new_install = false;


void CheckMariaDB(const std::string cwd = std::filesystem::current_path().string())
{
	{	//CHECK INSTALLTION STATUS OF MARIADB
		//If Install target file "mariadb.exe" isnt found, the silent install with options will then be started
		//At the end the mariadb installer will be deleted
		mariaspath = findMaria();
		//createWindowsMessage("got out of find maria");
		if (choose_new_install || mariaspath == "")
		{
			if (6 != createWindowsChoice("A MariaDB Installation wasn't found! The Program would now try to install this version. If you DONT want that press \"No\" and install this version yourself. For the right values, please take a look in the provided README."))
			{
				exit;
			}
			servicename = "SQL_for_SWE_Project";
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

			std::string thepath = (cwd + "\\mariadb_installer.msi");

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

					// INSTALLDIR=\%ProgramFiles\%\\MariaDB_MehrMarkt DEFAULTUSER=root PASSWORD=toor SERVICENAME=Mehr_Markt_Database-Service PORT=42069 UTF8 
					std::string execute_order = "msiexec /i mariadb_installer.msi INSTALLDIR=\%ProgramFiles\%\\MariaDB_MehrMarkt SERVICENAME=SQL_for_SWE_Project PORT=" + port + " PASSWORD=" + user_password + "UTF8 /qn";
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
		else //When MariaDB is found installed on the machine already
		{
			//std::cout << "MariaDB is already installed.\n";
			if (mariaspath != "" && !config_did_work)
			{
				if (6 != createWindowsChoice("We found a version of MariaDB installed. Do you want to enter the Credentials (Username, Password, Port) for this instance? Or should we install our own version? !!! WARNING !!!: It will override the version 10.10!!!"))
				{
					choose_new_install = true;
					CheckMariaDB();
					return;
				}
				do {
					username = "";
					user_password = "";
					std::wstring wString_username;
					wString_username = SG_InputBox::GetString(L"Mehr Markt Anwendung by SWE B1", L"We found an existing version of MariaDB. In order to use this Software we require the Username, the Password and the Port of your DB. Please enter your MariaDB Username.", L"root");
					username = std::string(wString_username.begin(), wString_username.end());
					std::wstring wString_password;
					wString_password = SG_InputBox::GetString(L"Mehr Markt Anwendung by SWE B1", L"Please enter your MariaDB Password.", L"root");
					user_password = std::string(wString_password.begin(), wString_password.end());
					std::wstring wString_port;
					wString_port = SG_InputBox::GetString(L"Mehr Markt Anwendung by SWE B1", L"Please enter your MariaDB Port.", L"root");
					port = std::string(wString_port.begin(), wString_port.end());
				} while (username == "" && user_password == "" || 6 != createWindowsChoice("Username:" + username + "\nPassword:" + user_password + "\nPort:" + port + "\nIs that correct?"));
			
			}
		}
		



		startSQLService(servicename);


	
	}
}


void CheckXampp(const std::string cwd = std::filesystem::current_path().string())
{
	{	//CHECK INSTALLATION STATUS OF XAMPP
		//If Install target file "xampp-control.exe" isnt found, the silent install with options will then be started
		//At the end the xampp installer will be deleted

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

			if (!success)
			{
				createWindowsMessage("couldn't unpack installer file for xampp, though it's needed to install the xampp php interpreter");
			}

			std::string thepath = (cwd + "\\xampp_installer.exe");
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
		else
		{
			std::cout << "xampp is already there\n";

		}
	}
}

void StartSoftware(const std::string cwd = std::filesystem::current_path().string())
{
	{
		//HRSRC hResource = FindResourceW(NULL, MAKEINTRESOURCE(IDB_PHP), __TEXT("TEXTFILE"));
		//HGLOBAL hGlobal = LoadResource(NULL, hResource);
		//size_t exeSiz = SizeofResource(NULL, hResource);
		//void* exeBuf = LockResource(hGlobal);
		//std::cout << "\n" << cwd << "\n";

		//DWORD at1;
		//HANDLE hFile = CreateFile(L"index.php", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); // Check MSDN for more info
		//BOOL success = WriteFile(hFile, exeBuf, exeSiz, &at1, NULL); // If it fails  -> success = 0

		//CloseHandle(hFile);

		{
			std::cout << "\nSetting Up Path, Starting Localhost and Opening Project for User\n";
			//set PATH=%PATH%;C:\xampp\php\
			//L"C:\\Windows\\system32\\cmd.exe"  && php -S localhost:8000
			// + cwd + "\\index.php"
			std::string thepath = "cmd.exe /c set PATH=\%PATH\%;C:\\xampp\\php\\ && cd "+ project_path + " && start /b php -S localhost:8000 && start /b http://localhost:8000";
			//std::cout << "IST:" << thepath;
			std::wstring thepath_w;

			thepath_w.assign(thepath.begin(), thepath.end());
			LPWSTR thepath_final = &thepath_w[0];

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
				std::cout << "\n Couldnt start Server\n";
		}
	}
}

void ConfigureDatabase()
{
	startSQLService(servicename);
	
	//									 mysql -u root -ptoor -e "source C:\Users\IEUser\Documents\SWE-Software\Datenbank\datenbank-build.sql"

	/*std::cout << "\n\n" <<		 "cmd /c set PATH=\%PATH\%;" + mariaspath + "\\ && mysql -u " + username + " -p" + user_password + " -e \"source " + project_path + "\\Datenbank\\datenbank-build.sql\" -q" << "\n\n";
	std::cin.ignore();*/
							//    cmd /c set PATH=%PATH%;C:\Program Files\MariaDB 10.10\bin\ && mysql -u root -ptoor -e "source C:\Users\IEUser\Documents\SWE-Software\Datenbank\datenbank-build.sql" -q

	//if (!doCmdCommand("cmd /c set PATH=\%PATH\%;" + mariaspath + "\\ && mysql -u " + username + " -p" + user_password + " -e \"source " + project_path + "\\Datenbank\\datenbank-build.sql\" -q"))
	//{
	//	createWindowsMessage("´CONFIGURE DATENBANK FAILED");
	//	config_did_work = false;
	//	CheckMariaDB();
	//	ConfigureDatabase();
	//}

	std::string x = "cmd /c cd " + mariaspath + "\\ && mysql -u " + username + " -p" + user_password + " -e \"source " + project_path + "\\Datenbank\\datenbank-build.sql\" -q";
	std::string y = "cmd /c cd " + mariaspath + "\\ && mysql -u " + username + " -p" + user_password + " -e \"source " + project_path + "\\Datenbank\\datenbank-testdata.sql\" -q";
	const HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	//std::cout << "\n\n" << x << "\n\n";
	//std::cout << "\n\n" << y << "\n\n";

	if (!doCmdCommand(x) || !doCmdCommand(y))
	{
		config_did_work = false;
		createWindowsError("CONFIG DIDNT WORK");
		CheckMariaDB();
	}
	config_did_work = true;
}

int  _tmain(int argc, _TCHAR* argv[])
{
	//to hide console
	/*FreeConsole();*/

	//to hide console include a quick flicker at the start
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);


	//const std::string cwd = std::filesystem::current_path().string();

	killProcessByName(L"php.exe");
	killProcessByName(L"Embedded Installer.exe");




	CheckMariaDB();
	//std::cout << "got out of Maria\n";


	CheckXampp();
	//std::cout << "got out of Xampp\n";


	//std::cout << "Init Project\n";
	project_path = initProjectFiles();
	//std::cout << "got out of Init Project\n";

	//std::cout << "Configuring Database\n";
	ConfigureDatabase();
	//std::cout << "got out of configuring MariaDB\n";


	StartSoftware();
	//std::cout << "got out of software\n";


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