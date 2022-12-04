//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#include "wholeinclude.h"
#include "resource.h"
#include "resource1.h"
#include "resource2.h"
#include "helper_functions.h"
#include "installer.h"

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
bool didloadcredentials = false;


void CheckMariaDB(const std::string cwd = std::filesystem::current_path().string())
{
	{	//CHECK INSTALLTION STATUS OF MARIADB
		//If Install target file "mariadb.exe" isnt found, the silent install with options will then be started
		//At the end the mariadb installer will be deleted

		mariaspath = findMaria();
		//createWindowsMessage("got out of find maria");
		if (mariaspath == "")
		{
			if (6 == createWindowsChoice("MariaDB wasn't found. We will now try to install MariaDB 10.10. If you don't want us to do that press no and install it yourself."))
			{
				install_maria(cwd, username, user_password, port);
				mariaspath = findMaria();
			}
			else
			{
				createWindowsError("Programm exited by the user.");
				exit;
			}
		}
		else //When MariaDB is found installed on the machine already
		{
			//std::cout << "MariaDB is already installed.\n";
			if (config_did_work == false) //when config didn't work.
			{
				if (6 != createWindowsChoice("We found a version of MariaDB installed, but our base credentials arent working :( Please enter the Credentials (Username, Password, Port) for this instance?"))
				{
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
					loadCredentials(project_path, username, user_password, port, didloadcredentials);
				}
				else
				{
					createWindowsError("Programm requires the credentials of the existing MariaDB. Exiting now.");
					exit;
				}
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
			install_xampp(cwd);
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
			std::string thepath = "cmd.exe /c set PATH=\%PATH\%;C:\\xampp\\php\\ && cd " + project_path + " && start /b php -S localhost:8000 && start /b http://localhost:8000";
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
	//::ShowWindow(::GetConsoleWindow(), SW_HIDE);

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);


	//const std::string cwd = std::filesystem::current_path().string();

	killProcessByName(L"php.exe");
	killProcessByName(L"Embedded Installer.exe");


	//std::cout << "Init Project\n";
	project_path = initProjectFiles();
	//std::cout << "got out of Init Project\n";


	CheckMariaDB();
	loadCredentials(project_path, username, user_password, port, didloadcredentials);
	//std::cout << "got out of Maria\n";


	CheckXampp();
	//std::cout << "got out of Xampp\n";




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