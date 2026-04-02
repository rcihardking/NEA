#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <array>
#include <regex>
#include <initializer_list>
#include <fstream>
#include <sstream>
#include <cmath>
#include <glad/glad.h>
#include <libpng/png.h>

#include "hashtable.h"


//defining structs used by functions
//images
struct image {
	unsigned int width;
	unsigned int height;
	unsigned char* data;
};

struct texture {
	unsigned int textureID;
	//~texture();	//deconstructor for automatic cleanup
};

//meshes
struct mesh {
	unsigned int vbo;
	unsigned int vao;
	unsigned int size;
	//~mesh();	//deconstructor for automatic cleanup
};

//shaders
struct shader {
	unsigned int shaderID;
	//~shader();	//deconstructor for automatic cleanup
};



//defining our read functions
//images
image readPNG(std::string filepath);

//meshes
std::vector<float> readOBJ(std::string filepath);
std::vector<float> readOBJ_old(std::string filepath);



//defining default dictionaries
typedef std::vector<float>(*readMeshPtr)(std::string);
typedef image(*readImgPtr)(std::string);

//defining file loading classes
struct formatLoaders {
	hashtable<readImgPtr, 10>* imgFormats;
	hashtable<readMeshPtr, 10>* meshFormats;
};


class fileLoader {
public:
	fileLoader(formatLoaders formatFunctions) : formats{ formatFunctions } {};

	texture imageLoader(std::string filepath);
	mesh meshLoader(std::string filepath);
	shader shaderLoader(std::initializer_list<std::string> filepaths);	//can take any amount of filepaths
private:
	formatLoaders formats;
};