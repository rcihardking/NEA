#include "fileHandler_NEW.h"

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

	// checks to ensure file being read exists and is a png
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

	// reading png and getting important data
	png_read_png(pngPtr, infoPtr, PNG_TRANSFORM_IDENTITY, NULL);
	unsigned char** rowPtrs = png_get_rows(pngPtr, infoPtr);
	int rowBytes = static_cast<int>(png_get_rowbytes(pngPtr, infoPtr));
	newImage.width = png_get_image_width(pngPtr, infoPtr);
	newImage.height = png_get_image_height(pngPtr, infoPtr);

	if (newImage.width * newImage.height > GL_MAX_TEXTURE_SIZE * GL_MAX_TEXTURE_SIZE) {
		std::cout << "texture has exceeded OpenGL max texture size\n";
		return newImage;
	}

	unsigned char* data = new unsigned char[rowBytes * newImage.height];
	newImage.data = data;

	// moving data into heap buffer
	for (int i = 0; i < newImage.height; ++i) {
		memmove(data + i * rowBytes, rowPtrs[i], rowBytes); // fairly sure this is faster than std::move
	}

	// cleanup
	png_destroy_read_struct(&pngPtr, &infoPtr, NULL);
	fclose(file);

	return newImage;
}

std::vector<float> readOBJ(std::string filepath) {};


texture fileLoader::imageLoader(std::string filepath) {
	std::string fileExt = getExt(filepath);
	image newImage = formats.imgFormats[fileExt](filepath);

}

mesh fileLoader::meshLoader(std::string filepath) {
	std::string fileExt = getExt(filepath);
	std::vector<float> verticies = formats.meshFormats[fileExt](filepath);


}