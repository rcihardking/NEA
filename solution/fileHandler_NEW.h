#pragma once
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


//defining structs used by functions
//images
struct image {
	unsigned int width;
	unsigned int height;
	unsigned char* data;
};

struct texture {
	unsigned int textureID;
	~texture();	//deconstructor for automatic cleanup
};

//meshes
struct mesh {
	unsigned int vbo;
	unsigned int vao;
	unsigned int size;
	~mesh();	//deconstructor for automatic cleanup
};

//shaders
struct shader {
	unsigned int shaderID;
	~shader();	//deconstructor for automatic cleanup
};



//defining our read functions
//images
image readPNG(std::string filepath);

//meshes
std::vector<float> readOBJ(std::string filepath);




//defining default dictionaries
typedef std::vector<float>(*readMeshPtr)(std::string);
std::map<std::string, readMeshPtr> default_meshFormatLoaders = {
	{"obj", &readOBJ}
};

typedef image (*readImgPtr)(std::string);
std::map<std::string, readImgPtr> default_imageFormatLoaders = {
	{"png", &readPNG}
};



//defining file loading classes
struct formatLoaders {
	std::map<std::string, readImgPtr> imgFormats;
	std::map<std::string, readMeshPtr> meshFormats;
};


class fileLoader {
public:
	fileLoader(formatLoaders formatFunctions);

	texture imageLoader(std::string filepath);
	mesh meshLoader(std::string filepath);
	shader shaderLoader(std::initializer_list<std::string> filepaths);	//can take any amount of filepaths
private:
	formatLoaders formats;
};