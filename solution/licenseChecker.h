#pragma once

#include <curl/curl.h>
#include <Windows.h>

#include <string>
#include <iostream>

int checkLicense(std::string license, std::string email);

void licenseProcess();