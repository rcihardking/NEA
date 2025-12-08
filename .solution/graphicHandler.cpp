#include "graphicHandler.h"

static void loadShader(GLuint* ID, std::string vertexFilepath, std::string fragmentFilepath) {
	std::vector<std::string> filepaths = { vertexFilepath, fragmentFilepath };
	static constexpr GLenum types[2] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };

	if (filepaths.size() > 4) {
		assert(false);
	}

	std::vector<GLuint> shaders;
	shaders.reserve(filepaths.size());
	for (int i = 0; i < filepaths.size(); ++i) {
		std::ifstream text(filepaths[i]);
		if (!text.is_open()) {
			std::cout << filepaths[i] << "\ndoesn't exist";
			assert(false);
		}

		std::stringstream buffer;
		buffer << text.rdbuf();
		std::string temp = buffer.str();
		const char* shaderText = temp.c_str();
		text.close();

		GLuint newShader = glCreateShader(types[i]);
		glShaderSource(newShader, 1, &shaderText, NULL);
		glCompileShader(newShader);

		int result;
		glGetShaderiv(newShader, GL_COMPILE_STATUS, &result);
		if (!result) {
			char errLog[512];
			glGetShaderInfoLog(newShader, 512, NULL, errLog);
			std::cout << errLog << "\n";
			assert(false);
		}

		glAttachShader(*ID, newShader);
	}

	glLinkProgram(*ID);

	for (int i = 0; i < shaders.size(); ++i) {
		glDetachShader(*ID, shaders[i]);
		glDeleteShader(shaders[i]);
	}
}

graphics::staticShader::staticShader(std::string vertexFilepath, std::string fragmentFilepath) {
	loadShader(&ID, vertexFilepath, fragmentFilepath);

	static const GLuint loc0 = glGetUniformLocation(ID, "scale");
    static const GLuint loc1 = glGetUniformLocation(ID, "rotation");
    static const GLuint loc2 = glGetUniformLocation(ID, "translation");
    static const GLuint loc3 = glGetUniformLocation(ID, "proj");

	uniformLocations.push_back(loc0);
	uniformLocations.push_back(loc1);
	uniformLocations.push_back(loc2);
	uniformLocations.push_back(loc3);
}

graphics::staticShader::~staticShader() {
	if (ID) { glDeleteProgram(ID); }
}



graphics::oldshader::oldshader(std::string vertexFilepath, std::string fragmentFilepath) { // maybe change so input argument is just one vector?
	std::vector<std::string> filepaths = { vertexFilepath, fragmentFilepath };
	static constexpr GLenum types[2] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };

	if (filepaths.size() > 4) {
		assert(false);
	}

	std::vector<GLuint> shaders;
	shaders.reserve(filepaths.size());
	for (int i = 0; i < filepaths.size(); ++i) {
		std::ifstream text(filepaths[i]);
		if (!text.is_open()) {
			std::cout << filepaths[i] << "\ndoesn't exist";
			assert(false);
		}

		std::stringstream buffer;
		buffer << text.rdbuf();
		std::string temp = buffer.str();
		const char* shaderText = temp.c_str();
		text.close();

		GLuint newShader = glCreateShader(types[i]);
		glShaderSource(newShader, 1, &shaderText, NULL);
		glCompileShader(newShader);

		int result;
		glGetShaderiv(newShader, GL_COMPILE_STATUS, &result);
		if (!result) {
			char errLog[512];
			glGetShaderInfoLog(newShader, 512, NULL, errLog);
			std::cout << errLog << "\n";
			assert(false);
		}

		glAttachShader(ID, newShader);
	}

	glLinkProgram(ID);

	for (int i = 0; i < shaders.size(); ++i) {
		glDetachShader(ID, shaders[i]);
		glDeleteShader(shaders[i]);
	}
}

graphics::oldshader::~oldshader() {
	if (ID) { glDeleteProgram(ID); }
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

int graphics::oldmesh::readObj(std::string filepath) { // this code is CRAP CRAP CRAP!!! must fix

	std::vector<float> vertexPos;
	std::vector<float> vertexNor;
	std::vector<float> vertexTex;

	std::ifstream obj(filepath);
	if (!obj.is_open()) {
		return 1;
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

	return 0;
}

graphics::oldmesh::oldmesh(std::string filepath, GLuint shader, GLuint texture) : shaderID{ shader }, textureID{ texture } {
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	static const std::regex type("([a-z]|[A-Z]|[0-9]|_|/|./|../)*.([a-z]*)");
	std::smatch matchObj;
	std::regex_match(filepath, matchObj, type); // matchObj[2] will be the file extension

	if (matchObj.size() < 2) {
		std::cout << "malformed file path\n";
		assert(false);
	}
	if (matchObj[2] == "obj") {
		if (readObj(filepath)) {
			std::cout << "file doesnt exist?\n";
			assert(false);
		}
	}
	else {
		std::cout << "unknown mesh format\n";
		assert(false);
	}

	size = verticies.size();

	glBufferData(GL_ARRAY_BUFFER, verticies.size() * sizeof(float), verticies.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0); // positions
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // textures
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float))); // normals
	glEnableVertexAttribArray(2);
}

graphics::oldmesh::~oldmesh() {
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void graphics::oldmesh::draw() {
	glUseProgram(shaderID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	static const GLuint loc0 = glGetUniformLocation(shaderID, "scale");
	static const GLuint loc1 = glGetUniformLocation(shaderID, "rotation");
	static const GLuint loc2 = glGetUniformLocation(shaderID, "translation");
	static const GLuint loc3 = glGetUniformLocation(shaderID, "proj");

	glUniformMatrix4fv(loc0, 1, true, scale.array);
	glUniformMatrix4fv(loc1, 1, true, rotation.array);
	glUniformMatrix4fv(loc2, 1, true, translation.array);
	glUniformMatrix4fv(loc3, 1, true, projection.array);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, size);

	glBindTexture(GL_TEXTURE_2D, 0);
}



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

static std::vector<float> readOBJ(std::string meshFilepath) {
	std::vector<float> verticies;
	std::vector<float> positions;
	std::vector<float> textures;
	std::vector<float> normals;

	std::ifstream meshFile(meshFilepath);

	if (!meshFile.is_open()) {
		return verticies;
	}

	static const std::regex header("([a-z]*) ([a-z]|[A-Z]|[0-9]|/|.)*");
	static const std::regex v("v (-?[0-9]+.[0-9]*) (-?[0-9]+.[0-9]*) (-?[0-9]+.[0-9]*)");
	static const std::regex vt("vt (-?[0-9]+.[0-9]*) (-?[0-9]+.[0-9]*)");
	static const std::regex vn("vn (-?[0-9]+.[0-9]*) (-?[0-9]+.[0-9]*) (-?[0-9]+.[0-9]*)");
	static const std::regex f("f ([0-9]*)/([0-9]*)/([0-9]*) ([0-9]*)/([0-9]*)/([0-9]*) ([0-9]*)/([0-9]*)/([0-9]*)");

	for (std::string line; std::getline(meshFile, line); ) {
		std::smatch headerMatch;
		std::regex_match(line, headerMatch, header);
		if (headerMatch.size() < 2) {
			continue;
		}

		if (headerMatch[1] == "v") {
			std::smatch matchObj;
			std::regex_match(line, matchObj, v);
			if (matchObj.size() < 2) {
				assert(false);
			}

			for (int i = 0; i < 3; ++i) {
				positions.push_back(stof(matchObj[i + 1].str()));
			}
		}
		else if (headerMatch[1] == "vt") {
			std::smatch matchObj;
			std::regex_match(line, matchObj, vt);
			if (matchObj.size() < 2) {
				assert(false);
			}

			for (int i = 0; i < 2; ++i) {
				textures.push_back(stof(matchObj[i + 1].str()));
			}
		}
		else if (headerMatch[1] == "vn") {
			std::smatch matchObj;
			std::regex_match(line, matchObj, vn);
			if (matchObj.size() < 2) {
				assert(false);
			}

			for (int i = 0; i < 3; ++i) {
				normals.push_back(stof(matchObj[i + 1].str()));
			}
		}
		else if (headerMatch[1] == "f") {
			std::smatch matchObj;
			std::regex_match(line, matchObj, f);
			if (matchObj.size() < 2) {
				assert(false);
			}
			std::array<int, 9> indexes;
			for (int i = 0; i < 9; ++i) {
				indexes[i] = stoi(matchObj[i + 1].str()) - 1;
			}

			for (int i = 0; i < 3; ++i) {
				verticies.push_back(positions[indexes[i * 3]]);
				verticies.push_back(textures[indexes[i * 3 + 1]]);
				verticies.push_back(normals[indexes[i * 3 + 2]]);
			}
		}
		else {
			continue;
		}
	}

	return verticies;
}

int graphics::scene::loadMesh(std::string meshFilepath) {
	mesh newMesh;
	std::vector<float> verticies = readOBJ(meshFilepath);

	GLuint vao;
	GLuint vbo;
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, verticies.size() * sizeof(float), verticies.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0); // positions
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // textures
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float))); // normals
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	newMesh.size = verticies.size();
	newMesh.vbo = vbo;
	newMesh.vao = vao;

	std::cout << newMesh.size << "\n" << newMesh.vao << "\n" << newMesh.vbo << "\n";

	meshes.push_back(newMesh);
	return meshes.size();
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

int graphics::scene::loadImage(std::string imageFilepath) {
	int width;
	int height;
	unsigned char* data = readPNG(imageFilepath, &width, &height);
	if (data == nullptr) {
		// need to add expection handling
		assert(false);
	}

	texture newTexture;

	glGenTextures(1, &newTexture.image);
	glBindTexture(GL_TEXTURE_2D, newTexture.image);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	// need to be able to recognise what colour type image is

	glGenerateMipmap(GL_TEXTURE_2D);

	delete[width * height] data;

	textures.push_back(newTexture);
	return textures.size();
}



void graphics::staticInstance::draw() {
	if (meshIndex != 0) {
		glUseProgram(myShader->ID);
		graphics::mesh* myMesh = &myScene->meshes[meshIndex - 1];

		if (textureIndex != 0) {
			graphics::texture* myTexture = &myScene->textures[textureIndex - 1];
			glBindTexture(GL_TEXTURE_2D, myTexture->image);
		}
		glBindVertexArray(myMesh->vao);
		glBindBuffer(GL_ARRAY_BUFFER, myMesh->vbo);

		glUniformMatrix4fv(myShader->uniformLocations[0], 1, true, scale.array);
		glUniformMatrix4fv(myShader->uniformLocations[1], 1, true, rotation.array);
		glUniformMatrix4fv(myShader->uniformLocations[2], 1, true, translation.array);
		glUniformMatrix4fv(myShader->uniformLocations[3], 1, true, myScene->perspective.array);

		glDrawArrays(GL_TRIANGLES, 0, myMesh->size);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	
	if (children.size() == 0) {
		return;
	}
	for (auto it = children.begin(); it != children.end(); ++it) {
		staticInstance* child = *it;
		child->draw();
	}
	
}

void graphics::staticInstance::removeChild(staticInstance* child) {
	for (auto it = children.begin(); it != children.end(); ++it) {
		if (*it == child) {
			children.erase(it);
			return;
		}
	}
	std::cout << "child doesnt exist\n";
	return;
}

void graphics::staticInstance::addChild(staticInstance* child) {
	children.push_back(child);
}

void graphics::staticInstance::changeParent(staticInstance* newParent) {
	if (parent != nullptr) {
		parent->removeChild(this);
	}
	newParent->addChild(this);
	parent = newParent;
}



void graphics::location::rotate(std::initializer_list<float> rot) {
	if (rot.size() == 3) {
		std::move(rot.begin(), rot.end(), orientation);
		rotation = createEulerRotation(orientation);
	}
}

void graphics::location::move(std::initializer_list<float> pos) {
	if (pos.size() == 3) {
		std::move(pos.begin(), pos.end(), position);
		translation = createTranslation(position);
	}
}

void graphics::location::resize(float factor) {
	size = factor;
	scale = createScale(factor);
}