#pragma once

#include <libpng/png.h>
#include <glad/glad.h>
#include <iostream>
#include <string>
#include <regex>
#include <cassert>
#include <fstream>
#include <array>

class imageLoader {
private:
	std::vector<unsigned char*> dataArrays;
	std::vector<int> imageDimensions;

	int readPNG(std::string imageFilepath);
public:
	int addImages(std::vector<std::string> imageFilepaths);
	int addImages(std::initializer_list<std::string> imageFilepaths);
	std::vector<GLuint> genImages();
};

class meshLoader {
private:
	std::vector<std::vector<float>> vertexArrays;

	int readOBJ(std::string meshFilepath);
public:
	int addMeshes(std::vector<std::string> meshFilepaths);
	int addMeshes(std::initializer_list<std::string> meshFilepaths);
	//std::vector<GLuint> genMeshes(GLuint vao);
};

class scene {
private:
	struct mesh {
		GLuint vbo;
		size_t size;

		GLuint vao;
		GLuint shader;
	};

	struct texture { // struct is only here for the sake of extensibility
		GLuint image;
	};

	class instance {
	private:
		instance* parent;
		std::vector<instance*> children;

		//position
		int meshIndex;
		int textureIndex;
	public:
		const std::vector<instance*> getChildren();
		const instance* getParent();
		instance* changeParent();
	};

	std::vector<mesh> meshes;
	std::vector<texture> textures;
public:
};