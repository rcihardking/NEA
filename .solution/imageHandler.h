#pragma once

#include <libpng/png.h>
#include <glad/glad.h>
#include <iostream>
#include <string>
#include <regex>
#include <cassert>

class image {
public:
	GLuint texture = 0;
	int width = 0;
	int height = 0;

	image(std::string filepath);
	~image();

	void createTexture();
private:
	unsigned char* data = nullptr;

	int readPNG(std::string filepath);
};