#include "fileHandler_NEW.h"


texture::~texture() {
	if (textureID != NULL) {
		glDeleteTextures(1, &textureID);
	}
}

mesh::~mesh() {
	if (vbo != NULL) {
		glDeleteBuffers(1, &vbo);
	}
	if (vao != NULL) {
		glDeleteBuffers(1, &vao);
	}
}

shader::~shader() {
	if (shaderID != NULL) {
		glDeleteProgram(shaderID);
	}
}


// helper function to get the file extension from a file path
// e.g., returns png for abc.png
static inline std::string getExt(std::string filepath) {
	static const std::regex ext("([a-z]|[A-Z]|[0-9]|/|./|../|_)*.([a-z]*|[A-Z]*)");
	std::smatch match;
	std::regex_match(filepath, match, ext);

	if (match.size() < 2) {
		std::cout << "no file extension\n";
		return "";
	}

	return match[2];
}



image readPNG(std::string filepath) {
	image newImage;
	newImage.data = nullptr;
	newImage.width = 0;
	newImage.height = 0;

	//checks to ensure file being read exists and is a png
	FILE* file = fopen(filepath.c_str(), "rb");
	if (!file) {
		return newImage;
	}
	unsigned char header[8];
	if (fread(header, 1, 8, file) != 8) {
		return newImage;
	}
	static constexpr unsigned char pngSig[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };
	if (memcmp(header, pngSig, 8) != 0) {
		return newImage;
	}

	// initialising libpng
	png_structp pngPtr = png_create_read_struct(
		PNG_LIBPNG_VER_STRING,
		NULL,
		NULL,
		NULL
	);
	if (!pngPtr) {
		fclose(file);
		return newImage;
	}
	png_infop infoPtr = png_create_info_struct(pngPtr);
	if (!infoPtr) {
		png_destroy_read_struct(&pngPtr, NULL, NULL);
		fclose(file);
		return newImage;
	}
	if (setjmp(png_jmpbuf(pngPtr))) {
		png_destroy_read_struct(&pngPtr, &infoPtr, NULL);
		fclose(file);
		return newImage;
	}
	png_init_io(pngPtr, file);
	png_set_sig_bytes(pngPtr, 8);

	//reading png and image resolution
	png_read_png(pngPtr, infoPtr, PNG_TRANSFORM_IDENTITY, NULL);
	unsigned char** rowPtrs = png_get_rows(pngPtr, infoPtr);
	int rowBytes = static_cast<int>(png_get_rowbytes(pngPtr, infoPtr));
	newImage.width = png_get_image_width(pngPtr, infoPtr);
	newImage.height = png_get_image_height(pngPtr, infoPtr);

	//check if the image is too big for opengl
	if (newImage.width * newImage.height > GL_MAX_TEXTURE_SIZE * GL_MAX_TEXTURE_SIZE) {
		std::cout << "texture has exceeded OpenGL max texture size\n";
		return newImage;
	}

	unsigned char* data = new unsigned char[rowBytes * newImage.height];
	newImage.data = data;

	// moving image data into buffer
	for (int i = 0; i < newImage.height; ++i) {
		memmove(data + i * rowBytes, rowPtrs[i], rowBytes); // fairly sure this is faster than std::move
	}

	//cleanup libpng
	png_destroy_read_struct(&pngPtr, &infoPtr, NULL);
	fclose(file);

	return newImage;
}

template<int size>
static std::array<float, size> getOBJData(std::string line, std::regex re, bool* flag) {
	std::array<float, size> datapart = { 0 };
	std::smatch matchObj;
	std::regex_match(line, matchObj, re);
	if (matchObj.size() < 2) {
		std::cout << "malformed line\n";
		*flag = true;
		return datapart;
	}

	for (int i = 0; i < size; ++i) {
		datapart[i] = std::stof(matchObj[i + 1].str());
	}
	return datapart;
}

std::vector<float> readOBJ(std::string filepath) {
	std::vector<float> vertices;
	std::ifstream text(filepath);
	if (!text.is_open()) {
		std::cout << filepath << "\ndoesn't exist\n";
		return vertices;
	}

	static const std::regex header("([a-z]*) ([a-z]|[A-Z]|[0-9]|/|.)*");
	static const std::regex v("v (-?[0-9]+.[0-9]*) (-?[0-9]+.[0-9]*) (-?[0-9]+.[0-9]*)");
	static const std::regex vt("vt (-?[0-9]+.[0-9]*) (-?[0-9]+.[0-9]*)");
	static const std::regex vn("vn (-?[0-9]+.[0-9]*) (-?[0-9]+.[0-9]*) (-?[0-9]+.[0-9]*)");
	static const std::regex f("f ([0-9]*/[0-9]*/[0-9]*) ([0-9]*/[0-9]*/[0-9]*) ([0-9]*/[0-9]*/[0-9]*)");
	static const std::regex ind("([0-9]*)/([0-9]*)/([0-9]*)");

	std::vector< std::array<float, 3> > positions;
	std::vector< std::array<float, 3> > normals;
	std::vector< std::array<float, 3> > faces;
	std::vector< std::array<float, 2> > textures;

	bool flag = false;
	for (std::string line; std::getline(text, line); ) {
		std::smatch headerMatch;
		std::regex_match(line, headerMatch, header);
		if (headerMatch.size() < 2) {
			continue;
		}

		if (headerMatch[1] == "v") {
			positions.push_back(getOBJData<3>(line, v, &flag));
		}
		else if (headerMatch[1] == "vt") {
			textures.push_back(getOBJData<2>(line, vt, &flag));
		}
		else if (headerMatch[1] == "vn") {
			normals.push_back(getOBJData<3>(line, vt, &flag));
		}
		else if (headerMatch[1] == "f") {
			std::array<std::string, 3> facepart;
			std::smatch matchObj;
			std::regex_match(line, matchObj, f);
			if (matchObj.size() < 2) {
				std::cout << "malformed line\n";
				return vertices;
			}

			for (int i = 0; i < 3; ++i) {
				facepart[i] = matchObj[i + 1].str();
			}

			for (int i = 0; i < 3; ++i) {
				faces.push_back(getOBJData<3>(facepart[i], ind, &flag));
			}
		}

		if (flag) {	//if there is a malformed data line we skip loading the entire mesh
			return vertices;
		}

		for (auto it = faces.begin(); it != faces.end(); ++it) {
			std::array<float, 3> face = *it;
			for (auto it = positions[face[0]].begin(); it != positions[face[0]].end(); ++it) {
				vertices.push_back(*it);
			}
			for (auto it = textures[face[1]].begin(); it != textures[face[1]].end(); ++it) {
				vertices.push_back(*it);
			}
			for (auto it = normals[face[2]].begin(); it != normals[face[2]].end(); ++it) {
				vertices.push_back(*it);
			}
		}

		return vertices;
	};
};


texture fileLoader::imageLoader(std::string filepath) {
	texture newTexture;
	newTexture.textureID = 0;

	std::string fileExt = getExt(filepath);
	if (formats.imgFormats.find(fileExt) == formats.imgFormats.end()) {
		std::cout << "file format not supported\n";
		return newTexture;
	}

	image newImage = formats.imgFormats[fileExt](filepath);	//grab image data

	if (newImage.data == nullptr) {	//error has happened when loading image
		return newTexture;
	}

	//create opengl texture id
	glGenTextures(1, &newTexture.textureID);
	glBindTexture(GL_TEXTURE_2D, newTexture.textureID);

	//basic texture setup
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//send image data to opengl api
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, newImage.width, newImage.height, 0, GL_RGB, GL_UNSIGNED_BYTE, newImage.data);

	//unbind texture
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	//delete image data now that image is loaded by opengl
	delete[newImage.width * newImage.height] newImage.data;
	newImage.data = nullptr;

	return newTexture;
}

mesh fileLoader::meshLoader(std::string filepath) {
	mesh newMesh;
	newMesh.vao = 0;
	newMesh.vbo = 0;
	std::string fileExt = getExt(filepath);
	std::vector<float> vertices = formats.meshFormats[fileExt](filepath);

	if (vertices.size() == 1) {
		return newMesh; //failed to read obj file
	}

	glGenVertexArrays(1, &newMesh.vao);
	glBindVertexArray(newMesh.vao);

	glGenBuffers(1, &newMesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, newMesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0); // positions
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // textures
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float))); // normals
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	newMesh.size = vertices.size();
	return newMesh;
}

//compiles shader parts and returns the opengl id
static unsigned int compileShader(std::string filepath, GLenum type) {
	std::ifstream text(filepath);
	if (!text.is_open()) {
		std::cout << filepath << "\ndoesn't exist";
		return 0;
	}

	std::stringstream buffer;
	buffer << text.rdbuf();
	std::string temp = buffer.str();
	const char* shaderText = temp.c_str();	//extract c string version of the file text
	text.close();

	unsigned int shaderpart = glCreateShader(type);
	glShaderSource(shaderpart, 1, &shaderText, NULL);	//send shaderpart text to opengl for compilation
	glCompileShader(shaderpart);	//compile shader

	int result;
	glGetShaderiv(shaderpart, GL_COMPILE_STATUS, &result);	//check if shader compiled properly
	if (!result) {	//output error and cleanup if it didnt
		char errLog[512];
		glGetShaderInfoLog(shaderpart, 512, NULL, errLog);
		std::cout << errLog << "\n";
		glDeleteShader(shaderpart);
		return 0;
	}

	return shaderpart;
};

shader fileLoader::shaderLoader(std::initializer_list<std::string> filepaths) {
	std::map<std::string, bool> flags = {	//dictionary containing flags if that shader type has been loaded
		{"vert", false},
		{"frag", false},
		{"tess", false},
		{"geom", false}
	};
	std::map<std::string, GLenum> types = {	//dictionary to get the correct glenum when compiling shader
		{"vert", GL_VERTEX_SHADER},
		{"frag", GL_FRAGMENT_SHADER},
		{"tess", GL_TESS_CONTROL_SHADER},
		{"geom", GL_GEOMETRY_SHADER}
	};

	shader newShader;
	newShader.shaderID = glCreateProgram();

	for (auto it = filepaths.begin(); it != filepaths.end(); ++it) {
		std::string fpExt = getExt(*it);
		
		auto flagsIt = flags.find(fpExt);	//get iterator of key so we dont have to query for it multiple times
		if (flagsIt == flags.end()) {	//file extension isnt a recognised shader
			continue;
		}
		if (flagsIt->second) {	//shader of that type already loaded for shader program
			continue; 
		}

		unsigned int shaderPart = compileShader(*it, types[fpExt]);	//compile the shader
		if (shaderPart == 0) {	//an error occured if the returned value is zero
			continue;
		}

		flagsIt->second = true;	//set flag to true so we know we already have that type of shader
		glAttachShader(newShader.shaderID, shaderPart);
		glDeleteShader(shaderPart);	//shaderpart is no longer needed and wastes memory
	}
	for (auto it = flags.begin(); it != flags.end(); ++it) {	//check if any shader part was loaded
		if (it->second) {
			return newShader;
		}
	}

	std::cout << "No shader part was successfully loaded\n";
	return newShader;
}