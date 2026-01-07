#include "fileHandler_NEW.h"

unsigned int basicVaoCreater() {
	unsigned int newvao;
	glGenVertexArrays(1, &newvao);
	glBindVertexArray(newvao);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0); // positions
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // textures
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float))); // normals
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return newvao;
}

unsigned int basicTextureCreator(unsigned char* data, int width, int height) {
	unsigned int newTexture;
	glGenTextures(1, &newTexture);
	glBindTexture(GL_TEXTURE_2D, newTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	// need to be able to recognise what colour type image is

	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	return newTexture;
}


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

typedef std::vector<float>(*readmeshPtr)(std::string);
std::map<std::string, readmeshPtr> readmeshFunctions = {
	{"obj", &readOBJ}
};

meshLoader::meshLoader(int s) {
	meshes.resize(s);
}

graphics::mesh meshLoader::loadMesh(std::string fp) {
	hashobject< graphics::mesh >* meshhash = meshes.get(fp);
	if (meshhash == nullptr) {
		if (createVao == nullptr) {
			assert(false);
		}
		std::string ext = getExt(fp);
		if (ext == "") {
			assert(false);
		}
		readmeshPtr readmesh = readmeshFunctions[ext];
		if (readmesh == nullptr) {
			assert(false);
		}
		std::vector<float> verticies = readmesh(fp);
		// check if verticies is empty?

		graphics::mesh newMesh;
		glGenBuffers(1, &newMesh.vbo);
		glBindBuffer(GL_ARRAY_BUFFER, newMesh.vbo);
		glBufferData(GL_ARRAY_BUFFER, verticies.size() * sizeof(float), verticies.data(), GL_STATIC_DRAW);

		newMesh.vao = createVao();
		// check if vao created properly?

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		meshes.add(fp, newMesh);
		return newMesh;
	}
	else {
		return meshhash->value;
	}
}


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

typedef unsigned char* (*readimagePtr)(std::string, int*, int*);
std::map<std::string, readimagePtr> readimgFunctions = {
	{ "png", &readPNG }
};

textureLoader::textureLoader(int s) {
	textures.resize(s);
}

graphics::texture textureLoader::loadTexture(std::string fp) {
	hashobject< graphics::texture >* texturehash = textures.get(fp);
	if (texturehash == nullptr) {
		if (createTexture == nullptr) {
			assert(false);
		}
		std::string ext = getExt(fp);
		if (ext == "") {
			assert(false);
		}
		readimagePtr readimage = readimgFunctions[fp];
		if (readimage == nullptr) {
			assert(false);
		}
		int width, height;
		unsigned char* data = readimage(fp, &width, &height);
		if (data == nullptr) {
			assert(false);
		}

		graphics::texture newTexture;
		newTexture.img = createTexture(data, width, height);
		// check if texture created properly?

		textures.add(fp, newTexture);
		delete[width * height] data;
		return newTexture;
	}
	else {
		return texturehash->value;
	}
}