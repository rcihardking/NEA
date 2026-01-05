#include "fileHandler.h"

static inline std::string getExt(std::string filepath) { // returns the file extension (eg returns png for a.png)
	static const std::regex ext("([a-z]|[A-Z]|[0-9]|/|./|../|_)*.([a-z]*|[A-Z]*)");
	std::smatch match;
	std::regex_match(filepath, match, ext);

	if (match.size() < 2) {
		std::cout << "no file extension\n";
		return "";
	}

	return match[2];
}




// images
static unsigned char* readPNG(std::string imageFilepath, int* width, int* height) {
	// checks to ensure file being read exists and is a png
	FILE* file = fopen(imageFilepath.c_str(), "rb");
	if (!file) {
		return nullptr;
	}
	unsigned char header[8];
	if (fread(header, 1, 8, file) != 8) {
		return nullptr;
	}
	static constexpr unsigned char pngSig[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };
	if (memcmp(header, pngSig, 8) != 0) {
		return nullptr;
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
		return nullptr;
	}
	png_infop infoPtr = png_create_info_struct(pngPtr);
	if (!infoPtr) {
		png_destroy_read_struct(&pngPtr, NULL, NULL);
		fclose(file);
		return nullptr;
	}
	if (setjmp(png_jmpbuf(pngPtr))) {
		png_destroy_read_struct(&pngPtr, &infoPtr, NULL);
		fclose(file);
		return nullptr;
	}
	png_init_io(pngPtr, file);
	png_set_sig_bytes(pngPtr, 8);

	// reading png and getting important data
	png_read_png(pngPtr, infoPtr, PNG_TRANSFORM_IDENTITY, NULL);
	unsigned char** rowPtrs = png_get_rows(pngPtr, infoPtr);
	int rowBytes = static_cast<int>(png_get_rowbytes(pngPtr, infoPtr));
	*width = png_get_image_width(pngPtr, infoPtr);
	*height = png_get_image_height(pngPtr, infoPtr);

	if (*width * *height > GL_MAX_TEXTURE_SIZE * GL_MAX_TEXTURE_SIZE) {
		std::cout << "texture has exceeded OpenGL max texture size\n";
		return nullptr;
	}

	unsigned char* data = new unsigned char[rowBytes * *height];

	// moving data into heap buffer
	for (int i = 0; i < *height; ++i) {
		memmove(data + i * rowBytes, rowPtrs[i], rowBytes); // fairly sure this is faster than std::move
	}

	// cleanup
	png_destroy_read_struct(&pngPtr, &infoPtr, NULL);
	fclose(file);

	return data;
}

typedef unsigned char* (*readImagePtr)(std::string, int*, int*);
std::map<std::string, readImagePtr> readimgFunctions = {
	{ "png", &readPNG }
};

unsigned char* file::readImage(std::string filepath, int* width, int* height) {
	std::string ext = getExt(filepath);
	if (ext == "") {
		assert(false);
		//ADD EXCEPTION HANDLER!!!
	}

	if (readimgFunctions.find(ext) == readimgFunctions.end()) { // file extension is not a supported image type
		return nullptr;
	}

	return readimgFunctions[ext](filepath, width, height);
}



// meshes

//static std::vector<float> readOBJ(std::string meshFilepath) {
//	std::vector<float> verticies;
//	std::vector<float> positions;
//	std::vector<float> textures;
//	std::vector<float> normals;
//
//	std::ifstream meshFile(meshFilepath);
//
//	if (!meshFile.is_open()) {
//		return verticies;
//	}
//
//	static const std::regex header("([a-z]*) ([a-z]|[A-Z]|[0-9]|/|.)*");
//	static const std::regex v("v (-?[0-9]+.[0-9]*) (-?[0-9]+.[0-9]*) (-?[0-9]+.[0-9]*)");
//	static const std::regex vt("vt (-?[0-9]+.[0-9]*) (-?[0-9]+.[0-9]*)");
//	static const std::regex vn("vn (-?[0-9]+.[0-9]*) (-?[0-9]+.[0-9]*) (-?[0-9]+.[0-9]*)");
//	static const std::regex f("f ([0-9]*)/([0-9]*)/([0-9]*) ([0-9]*)/([0-9]*)/([0-9]*) ([0-9]*)/([0-9]*)/([0-9]*)");
//
//	for (std::string line; std::getline(meshFile, line); ) {
//		std::smatch headerMatch;
//		std::regex_match(line, headerMatch, header);
//		if (headerMatch.size() < 2) {
//			continue;
//		}
//
//		if (headerMatch[1] == "v") {
//			std::smatch matchObj;
//			std::regex_match(line, matchObj, v);
//			if (matchObj.size() < 2) {
//				assert(false);
//			}
//
//			for (int i = 0; i < 3; ++i) {
//				positions.push_back(stof(matchObj[i + 1].str()));
//			}
//		}
//		else if (headerMatch[1] == "vt") {
//			std::smatch matchObj;
//			std::regex_match(line, matchObj, vt);
//			if (matchObj.size() < 2) {
//				assert(false);
//			}
//
//			for (int i = 0; i < 2; ++i) {
//				textures.push_back(stof(matchObj[i + 1].str()));
//			}
//		}
//		else if (headerMatch[1] == "vn") {
//			std::smatch matchObj;
//			std::regex_match(line, matchObj, vn);
//			if (matchObj.size() < 2) {
//				assert(false);
//			}
//
//			for (int i = 0; i < 3; ++i) {
//				normals.push_back(stof(matchObj[i + 1].str()));
//			}
//		}
//		else if (headerMatch[1] == "f") {
//			std::smatch matchObj;
//			std::regex_match(line, matchObj, f);
//			if (matchObj.size() < 2) {
//				assert(false);
//			}
//			std::array<int, 9> indexes;
//			for (int i = 0; i < 9; ++i) {
//				indexes[i] = stoi(matchObj[i + 1].str()) - 1;
//			}
//
//			for (int i = 0; i < 3; ++i) {
//				verticies.push_back(positions[indexes[i * 3]]);
//				verticies.push_back(textures[indexes[i * 3 + 1]]);
//				verticies.push_back(normals[indexes[i * 3 + 2]]);
//			}
//		}
//		else {
//			continue;
//		}
//	}
//
//	return verticies;
//}

static void objRegex(std::string line, std::regex reg, std::vector<float>* vec, int len) {
	std::smatch matchObj;
	std::regex_match(line, matchObj, reg);

	if (matchObj.size() < 2) {
		assert(false);
	}

	for (int i = 0; i < len; ++i) {
		vec->push_back(std::stof(matchObj[i + 1].str()));
	}
}

static std::vector<float> readOBJ(std::string filepath) {
	std::vector<float> verticies;
	std::vector<float> vertexPos;
	std::vector<float> vertexNor;
	std::vector<float> vertexTex;

	std::ifstream obj(filepath);
	if (!obj.is_open()) {
		return verticies;
	}

	static const std::regex v("v (-?[0-9]+.[0-9]*) (-?[0-9]+.[0-9]*) (-?[0-9]+.[0-9]*)");
	static const std::regex vn("vn (-?[0-9]+.[0-9]*) (-?[0-9]+.[0-9]*) (-?[0-9]+.[0-9]*)");
	static const std::regex vt("vt (-?[0-9]+.[0-9]*) (-?[0-9]+.[0-9]*)");
	static const std::regex f("f ([0-9]*/[0-9]*/[0-9]*) ([0-9]*/[0-9]*/[0-9]*) ([0-9]*/[0-9]*/[0-9]*)");
	static const std::regex index("([0-9]*)/([0-9]*)/([0-9]*)");

	for (std::string line; std::getline(obj, line); ) {
		switch (line.c_str()[0] << sizeof(char) * 8 | line.c_str()[1]) {
		case * "v" << sizeof(char) * 8 | *" ": // 30240
			objRegex(line, v, &vertexPos, 3);

			break;

		case * "v" << sizeof(char) * 8 | *"n": // 30318
			objRegex(line, vn, &vertexNor, 3);

			break;

		case * "v" << sizeof(char) * 8 | *"t": // 30324
			objRegex(line, vt, &vertexTex, 2);


			break;
		case * "f" << sizeof(char) * 8 | *" ": // 26144
			std::smatch matchObj;
			std::regex_match(line, matchObj, f);

			if (matchObj.size() < 2) {
				assert(false); // malformed obj file
			}

			std::vector<std::string> str_f;
			for (int i = 0; i < 3; i++) {
				str_f.push_back(matchObj[i + 1].str());
			}

			for (int i = 0; i < 3; i++) {
				std::smatch faceIndices;
				std::regex_match(str_f[i], faceIndices, index);

				verticies.push_back(vertexPos[(std::stoi(faceIndices[1].str()) - 1) * 3]);
				verticies.push_back(vertexPos[(std::stoi(faceIndices[1].str()) - 1) * 3 + 1]);
				verticies.push_back(vertexPos[(std::stoi(faceIndices[1].str()) - 1) * 3 + 2]);

				verticies.push_back(vertexTex[(std::stoi(faceIndices[2].str()) - 1) * 2]);
				verticies.push_back(vertexTex[(std::stoi(faceIndices[2].str()) - 1) * 2 + 1]);

				verticies.push_back(vertexNor[(std::stoi(faceIndices[3].str()) - 1) * 3]);
				verticies.push_back(vertexNor[(std::stoi(faceIndices[3].str()) - 1) * 3 + 1]);
				verticies.push_back(vertexNor[(std::stoi(faceIndices[3].str()) - 1) * 3 + 2]);
			}

			break;
		}
	}

	return verticies;
}

typedef std::vector<float>(*readMeshPtr)(std::string);
std::map<std::string, readMeshPtr> readmeshFunctions = {
	{"obj", &readOBJ}
};

std::vector<float> file::readMesh(std::string filepath) {
	std::string ext = getExt(filepath);
	std::vector<float> verticies;

	if (readmeshFunctions.find(ext) == readmeshFunctions.end()) {
		return verticies;
	}

	return readmeshFunctions[ext](filepath);
}