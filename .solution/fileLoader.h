#pragma once

#include <libpng/png.h>
#include <glad/glad.h>
#include <iostream>
#include <string>
#include <regex>
#include <cassert>

class imageLoader {
private:
	std::vector<unsigned char*> dataArrays;
	std::vector<int> imageDimensions;

	int readPNG(std::string imageFilepath);
public:
	int addImage(std::string imageFilepath);
	int addImages(std::vector<std::string> imageFilepaths);
	std::vector<GLuint> genImages();
};

class meshLoader {
private:
	std::vector< std::vector<float> > vertexArrays;
public:
	int addMesh(std::string meshFilepath);
	int addMeshs(std::vector<std::string> meshFilepaths);
	std::vector<GLuint> genMeshes(GLuint vao);
};