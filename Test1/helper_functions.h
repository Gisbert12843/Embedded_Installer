#pragma once
#include "wholeinclude.h"

//const wchar_t* GetWC(const char* c)
//{
//	const size_t cSize = strlen(c) + 1;
//	wchar_t* wc = new wchar_t[cSize];
//	mbstowcs(wc, c, cSize);
//
//	return wc;
//}




bool doCmdCommand(std::string thepath, std::string running_exe = "")
{
	{
		std::wstring thepath_w;
		thepath_w.assign(thepath.begin(), thepath.end());
		LPWSTR thepath_final = &thepath_w[0];

		std::wstring theexe_w;
		theexe_w.append(running_exe.begin(), running_exe.end());
		LPWSTR theexe_final = &theexe_w[0];
		if (running_exe == "")
			theexe_final == NULL;

		STARTUPINFO strtinfo = { sizeof(strtinfo) };
		PROCESS_INFORMATION processInfo;
		ZeroMemory(&strtinfo, sizeof(strtinfo));


		if (CreateProcess(theexe_final, thepath_final, NULL, NULL, FALSE, 0, NULL, NULL, &strtinfo, &processInfo))
		{
			while (WaitForSingleObject(processInfo.hProcess, INFINITE))
			{
				std::cout << ".";
				Sleep(800);
			}
			CloseHandle(processInfo.hProcess);
			CloseHandle(processInfo.hThread);

		}
		else
			return 0;
	}
}

bool doCmdCommandInNewWindow(std::string command, std::string running_exe = "")
{
	{

		std::wstring thepath_w;
		//thepath_w.append(L"start cmd.exe /c ");
		thepath_w.append(command.begin(), command.end());
		LPWSTR thepath_final = &thepath_w[0];

		std::wstring theexe_w;
		theexe_w.append(running_exe.begin(), running_exe.end());
		LPWSTR theexe_final = &theexe_w[0];
		if (running_exe == "")
			theexe_final == NULL;

		STARTUPINFO strtinfo = { sizeof(strtinfo) };
		PROCESS_INFORMATION processInfo;
		ZeroMemory(&strtinfo, sizeof(strtinfo));

		//CREATE_NO_WINDOW |
		if (CreateProcess(theexe_final,thepath_final, NULL, NULL, FALSE,  NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP, NULL, NULL, &strtinfo, &processInfo))
		{
			//while (WaitForSingleObject(processInfo.hProcess, INFINITE))
			//{
			//	std::cout << ".";
			//	Sleep(800);
			//}
			CloseHandle(processInfo.hProcess);
			CloseHandle(processInfo.hThread);

		}
		else
			return 0;
	}
}

void killProcessByName(const wchar_t* filename)
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	PROCESSENTRY32 pEntry;
	pEntry.dwSize = sizeof(pEntry);
	BOOL hRes = Process32First(hSnapShot, &pEntry);
	while (hRes)
	{
		
		if (lstrcmpW(pEntry.szExeFile, filename) == 0)
		{
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
				(DWORD)pEntry.th32ProcessID);
			if (hProcess != NULL)
			{
				TerminateProcess(hProcess, 9);
				CloseHandle(hProcess);
			}
		}
		hRes = Process32Next(hSnapShot, &pEntry);
	}
	CloseHandle(hSnapShot);
}


HANDLE GetProcessByName(PCSTR name)
{
	DWORD pid = 0;

	// Create toolhelp snapshot.
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 process;
	ZeroMemory(&process, sizeof(process));
	process.dwSize = sizeof(process);

	// Walkthrough all processes.
	if (Process32First(snapshot, &process))
	{
		do
		{
			// Compare process.szExeFile based on format of name, i.e., trim file path
			// trim .exe if necessary, etc.
			if (std::string(std::wstring(process.szExeFile).begin(), std::wstring(process.szExeFile).end()) == std::string(name))
			{
				pid = process.th32ProcessID;
				break;
			}
		} while (Process32Next(snapshot, &process));
	}

	CloseHandle(snapshot);

	if (pid != 0)
	{
		return OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	}

	// Not found
	return NULL;
}

bool isRunning(LPCSTR pName)
{
	HWND hwnd;
	hwnd = FindWindowA(NULL, pName);
	if (hwnd != 0) {
		return true;
	}
	else {
		return false;
	}
}


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
	std::string returnedString = "";
	for (int it = 0; it < theString.length(); it++)
	{
		if (theString.at(it) != '/')
			returnedString += theString.at(it);
		else returnedString += '\\';
	}
	return returnedString;
}



int createWindowsMessage(std::string thestring)
{
	std::wstring message(thestring.begin(), thestring.end());

	return MessageBoxW(NULL, message.c_str(), L"!!HINWEIS!!", MB_OK | MB_ICONQUESTION);
}

int createWindowsChoice(std::string thestring)
{
	std::wstring message(thestring.begin(), thestring.end());

	return MessageBoxW(NULL, message.c_str(), L"!!HINWEIS!!", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);
}

int createWindowsError(std::string thestring)
{
	std::wstring message(thestring.begin(), thestring.end());

	return MessageBoxW(NULL, message.c_str(), L"ERROR", MB_RETRYCANCEL | MB_ICONERROR );
}

void killProgrammMessage()
{
	std::string thestring = "Programm got prematurely exitted by the user.";
	std::wstring message(thestring.begin(), thestring.end());

	MessageBoxW(NULL, message.c_str(), L"ERROR", MB_OK | MB_ICONEXCLAMATION);
	//exit;
}

void startSQLService(std::string servicename)
{
	const char* x ;
	std::string y = "net start ";
	y.append(servicename);
	x = (y.c_str());
	system(x);
}
void stopSQLService(std::string servicename)
{
	const char* x;
	std::string y = "net stop ";
	y.append(servicename);
	x = (y.c_str());
	system(x);
}



std::string initProjectFiles()
{	//powershell.exe Expand-Archive .\project.zip -DestinationPath .\



	 CHAR my_documents[MAX_PATH];
	 HRESULT result = SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);



	 if (std::filesystem::exists(std::string(my_documents) + "\\SWE-Software"))
	 {
		 return (std::string(my_documents) + "\\SWE-Software");
	 }

	HRSRC hResource = FindResource(NULL, MAKEINTRESOURCE(IDB_PHP), __TEXT("BINARY"));
	HGLOBAL hGlobal = LoadResource(NULL, hResource);
	size_t exeSiz = SizeofResource(NULL, hResource);
	void* exeBuf = LockResource(hGlobal);



	DWORD at1;
	HANDLE hFile = CreateFile(L"project.zip", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); // Check MSDN for more info
	BOOL success = WriteFile(hFile, exeBuf, exeSiz, &at1, NULL); // If it fails  -> success = 0


	CloseHandle(hFile);

	if (!success)
	{
		createWindowsError("couldn't unpack project.zip, though its needed for the programm\d\nProgram will now Exit.");
		exit;
	}



	//this f conversion stuff.. string -> LPWSTR
	// powershell.exe "Expand - Archive .\project.zip - DestinationPath C:\Users\IEUser\Documents\SWE-Software"
	std::string my_documents_s = std::string("powershell -command \"Expand-Archive .\\project.zip -DestinationPath " + std::string(my_documents) + "\\SWE-Software\"");
	std::wstring my_documents_ws;
	my_documents_ws.assign(my_documents_s.begin(), my_documents_s.end());
	LPWSTR my_documents_final = &my_documents_ws[0];
	//std::cout << "powershell -command \"Expand-Archive .\\project.zip -DestinationPath " + std::string(my_documents) + "\\SWE-Software\"";
	system(my_documents_s.c_str());
	Sleep(6000);
	DeleteFileA("project.zip");
	return std::string(my_documents) + "\\SWE-Software";
}

std::string findMaria()
{
	TCHAR pf[MAX_PATH];

	SHGetSpecialFolderPath(
		0,
		pf,
		CSIDL_PROGRAM_FILES,
		FALSE);

	auto iter = std::filesystem::recursive_directory_iterator(pf, std::filesystem::directory_options::skip_permission_denied);
	auto end_iter = std::filesystem::end(iter);
	auto ec = std::error_code();
	std::filesystem::directory_entry dirEntry;
	for (; iter != end_iter; iter.increment(ec))
	{
		if (ec)
		{
			continue;
		}
		dirEntry = std::filesystem::directory_entry(iter->path());
		//std::cout << "searching in" << dirEntry.path() << "\n";
	


		if (dirEntry.path().filename() == L"mariadb.exe")
		{
			std::cout << "Found Maria in:" + dirEntry.path().parent_path().string() << "\n";
			return dirEntry.path().parent_path().string();
		}


	}
	return "";
}
