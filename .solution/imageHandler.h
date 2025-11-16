#pragma once

#include <libpng/png.h>
#include <glad/glad.h>
#include <iostream>
#include <string>
#include <regex>
#include <cassert>

//GLuint createTexture(const char *filePath);

class image {
public:
	image(std::string filepath);
	~image();

	GLuint createTexture();

	int width = 0;
	int height = 0;
private:
	unsigned char* data = nullptr;

	int readPNG(std::string filepath);
};