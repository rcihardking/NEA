#define _CRT_SECURE_NO_WARNINGS
#include "imageHandler.h"

int inline image::readPNG(std::string filepath) {
	// checks to ensure file being read exists and is a png
	FILE* file = fopen(filepath.c_str(), "rb");
	if (!file) {
		return 1;
	}
	unsigned char header[8];
	if (fread(header, 1, 8, file) != 8) {
		return 2;
	}
	static constexpr unsigned char pngSig[8] = {137, 80, 78, 71, 13, 10, 26, 10};
	if (memcmp(header, pngSig, 8) != 0) {
		return 3;
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
		return 4;
	}
	png_infop infoPtr = png_create_info_struct(pngPtr);
	if (!infoPtr) {
		png_destroy_read_struct(&pngPtr, NULL, NULL);
		fclose(file);
		return 4;
	}
	if (setjmp(png_jmpbuf(pngPtr))) {
		png_destroy_read_struct(&pngPtr, &infoPtr, NULL);
		fclose(file);
		return 5;
	}
	png_init_io(pngPtr, file);
	png_set_sig_bytes(pngPtr, 8);

	// reading png and getting important data
	png_read_png(pngPtr, infoPtr, PNG_TRANSFORM_IDENTITY, NULL);
	unsigned char** rowPtrs = png_get_rows(pngPtr, infoPtr);
	int rowBytes = static_cast<int>(png_get_rowbytes(pngPtr, infoPtr));
	height = png_get_image_height(pngPtr, infoPtr);
	width = png_get_image_width(pngPtr, infoPtr);

	data = new unsigned char[rowBytes * height];
	
	// moving data into heap buffer
	for (int i = 0; i < height; ++i) {
		memmove(data + i * rowBytes, rowPtrs[i], rowBytes); // fairly sure this is faster than std::move
	}
	
	// cleanup
	png_destroy_read_struct(&pngPtr, &infoPtr, NULL);
	fclose(file);

	return 0;
}

image::image(std::string filepath) {
	static const std::regex type("([a-z]|[A-Z]|[0-9]|_|/|./|../)*.([a-z]*)");
	std::smatch matchObj;
	std::regex_match(filepath, matchObj, type); // matchObj[2] will be the file extension

	if (matchObj.size() < 2) {
		std::cout << "malformed file path\n";
		assert(false);
	}

	if (matchObj[2] == "png") {
		if (readPNG(filepath)) {
			std::cout << "failed to read image\n"; 
			// add some actual error handling stuff!
			assert(false);
		}
	}
	else {
		std::cout << "unknown image format\n";
		assert(false);
	}
}

void image::createTexture() {
	if (data == nullptr) {
		return;
	}
	static const int maxSize = GL_MAX_TEXTURE_SIZE * GL_MAX_TEXTURE_SIZE; // this value should probably be taken with a grain of salt.
	if (width * height > maxSize) {
		std::cout << "texture has exceeded OpenGL max texture size\n";
		delete[width * height] data;
		data = nullptr;
		return;
	}
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	delete[width * height] data;
	data = nullptr;
}

image::~image() {
	if (data != nullptr) { // just incase allocated memory is never destroyed
		delete[width * height] data;
		data = nullptr;
	}
	if (texture != 0) {
		glDeleteTextures(1, &texture);
	}
}