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

std::vector<float> readOBJ(std::string filepath) {

};


texture fileLoader::imageLoader(std::string filepath) {
	std::string fileExt = getExt(filepath);
	image newImage = formats.imgFormats[fileExt](filepath);
	texture newTexture;
	newTexture.textureID = 0;

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
	std::string fileExt = getExt(filepath);
	std::vector<float> verticies = formats.meshFormats[fileExt](filepath);


}

static void compileShader(std::string filepath, GLenum type);

shader fileLoader::shaderLoader(std::initializer_list<std::string> filepaths) {
	std::map<std::string, bool> flags = {
		{"vert", false},
		{"frag", false},
		{"tess", false},
		{"geom", false}
	};
	std::map<std::string, GLenum> types = {
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

		flagsIt->second = true;	//set flag to true so we know we already have that type of shader
		
	}

	return newShader;
}