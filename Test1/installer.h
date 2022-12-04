#pragma once
#include "wholeinclude.h"
#include "resource.h"
#include "resource1.h"
#include "resource2.h"
#include "helper_functions.h"

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "advapi32.lib")

void install_maria(std::string sourcepath, std::string &username, std::string &user_password, std::string &port);
void install_xampp(std::string sourcepath);


