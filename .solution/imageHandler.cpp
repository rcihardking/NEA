#define _CRT_SECURE_NO_WARNINGS
#include "imageHandler.h"


static unsigned char* loadPNG(FILE* image, int *pWidth, int *pHeight) {
	// check if the supplied image file is actually a png
	const unsigned char sigPNG[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };
	unsigned char buffer[8];
	fread(buffer, 1, 8, image);

	if (memcmp(sigPNG, buffer, 8)) {
		fclose(image);
		return NULL;
	}

	// libpng setup
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) {
		fclose(image);
		return NULL;
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		fclose(image);
		return NULL;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(image);
		return NULL;
	}
	
	png_init_io(png_ptr, image);
	png_set_sig_bytes(png_ptr, 8);

	// reading png file
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

	unsigned int width, height;
	png_get_IHDR(png_ptr, info_ptr, &width, &height, NULL, NULL, NULL, NULL, NULL);

	png_bytep *row_pointers = png_get_rows(png_ptr, info_ptr);
	size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
	unsigned char* image_data = new unsigned char[height * rowbytes];
	
	for (unsigned int i = 0; i < height; i++) {
		memcpy(image_data + i * rowbytes, row_pointers[i], rowbytes);
	}

	// cleanup
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	fclose(image);

	*pWidth = width;
	*pHeight = height;
	return image_data;
}

static GLuint innitTexture(unsigned char *image_data, int width, int height) {

	GLuint texture;
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
	glGenerateMipmap(GL_TEXTURE_2D);

	delete[] image_data;
	image_data = nullptr;
	
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

GLuint createTexture(const char *filePath) {
	std::string sfilePath = filePath;
	auto pos = sfilePath.find_last_of(".");
	std::string ext = sfilePath.substr(pos + 1);

	int width, height;
	unsigned char* image_data;
	FILE* file = fopen(filePath, "rb");

	if (ext == "png") {
		image_data = loadPNG(file, &width, &height);
		return innitTexture(image_data, width, height);
	}
	else {
		// unsupported file type
		return NULL;
	}


}