#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "hashtable.h"
#include "graphicHandler.h"

#include <glad/glad.h>
#include <libpng/png.h>

#include <vector>
#include <map>
#include <regex>
#include <iostream>
#include <cassert>
#include <fstream>
#include <stdio.h>


unsigned int basicVaoCreater();

unsigned int basicTextureCreator(unsigned char* data, int width, int height);


typedef unsigned int (*vaoImplementation)();

class meshLoader {
private:
	hashtable< graphics::mesh > meshes;
public:
	meshLoader(int size);
	graphics::mesh loadMesh(std::string fp);

	vaoImplementation createVao = nullptr;
};


typedef unsigned int (*textureImplementation)(unsigned char*, int, int);

class textureLoader {
private:
	hashtable< graphics::texture > textures;

public:
	textureLoader(int size);
	graphics::texture loadTexture(std::string fp);

	textureImplementation createTexture = nullptr;
};