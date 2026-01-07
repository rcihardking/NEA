#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <vector>
#include <map>
#include <regex>
#include <iostream>
#include <fstream>
#include <cassert>
#include <stdio.h>

#include <glad/glad.h>
#include <libpng/png.h>

#include "hashtable.h"

namespace file {
	unsigned char* readImage(std::string filepath, int* width, int* height);

	std::vector<float> readMesh(std::string filepath);

	//unsigned int readShaders(std::string vertexShaderfp, std::string fragmentShaderfp);
}