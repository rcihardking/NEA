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

newgraphics::staticShader::staticShader(std::string vertexFilepath, std::string fragmentFilepath) { 
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

newgraphics::staticShader::~staticShader() {
	if (ID) { glDeleteProgram(ID); }
}

int newgraphics::scene::loadMesh(std::string meshFilepath, GLuint vao) {
	mesh newMesh;
	std::vector<float> verticies = file::readMesh(meshFilepath);

	GLuint vbo;
	GLuint newvao;
	glGenVertexArrays(1, &newvao);
	glBindVertexArray(newvao);

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
	newMesh.vao = newvao;

	meshes.push_back(newMesh);
	return meshes.size();
}

int newgraphics::scene::loadImage(std::string imageFilepath) {
	int width, height;
	unsigned char* data = file::readImage(imageFilepath, &width, &height);
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
	glBindTexture(GL_TEXTURE_2D, 0);

	delete[width * height] data;

	textures.push_back(newTexture);
	return textures.size();
}



newgraphics::staticInstance* newgraphics::staticInstance::search(std::string name) {
	if (identifier == name) {
		return this;
	}
	for (auto it = children.begin(); it != children.end(); ++it) {
		staticInstance* child = *it;
		staticInstance* found = child->search(name);
		if (found != nullptr) {
			return found;
		}
	}
	return nullptr;
}

void newgraphics::staticInstance::draw() {
	if (meshIndex != 0) {
		glUseProgram(myShader->ID);
		newgraphics::mesh* myMesh = &myScene->meshes[meshIndex - 1];

		if (textureIndex != 0) {
			newgraphics::texture* myTexture = &myScene->textures[textureIndex - 1];
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

void newgraphics::staticInstance::removeChild(staticInstance* child) {
	for (auto it = children.begin(); it != children.end(); ++it) {
		if (*it == child) {
			children.erase(it);
			return;
		}
	}
	std::cout << "child doesnt exist\n";
	return;
}

void newgraphics::staticInstance::addChild(staticInstance* child) {
	children.push_back(child);
}

void newgraphics::staticInstance::changeParent(staticInstance* newParent) {
	if (parent != nullptr) {
		parent->removeChild(this);
	}
	newParent->addChild(this);
	parent = newParent;
}

void newgraphics::staticInstance::rotate(std::initializer_list<float> rot) {
	if (rot.size() != 3) {
		return;
	}
	std::move(rot.begin(), rot.end(), orientation);
	rotation = graphics::createEulerRotation(orientation);
	transformation = transformation * rotation;

	if (children.size() == 0) {
		return;
	}
	for (auto it = children.begin(); it != children.end(); ++it) {
		staticInstance* child = *it;
		child->rotate(rot);
	}
}

void newgraphics::staticInstance::move(std::initializer_list<float> pos) {
	if (pos.size() != 3) {
		return;
	}
	std::move(pos.begin(), pos.end(), position);
	translation = graphics::createTranslation(position);
	transformation = transformation * translation;

	if (children.size() == 0) {
		return;
	}
	for (auto it = children.begin(); it != children.end(); ++it) {
		staticInstance* child = *it;
		child->move(pos);
	}
}

void newgraphics::staticInstance::resize(float factor) {
	size = factor;
	scale = graphics::createScale(factor);
	transformation = transformation * scale;

	if (children.size() == 0) {
		return;
	}
	for (auto it = children.begin(); it != children.end(); ++it) {
		staticInstance* child = *it;
		child->resize(factor);
	}
}



void newgraphics::location::rotate(std::initializer_list<float> rot) {
	if (rot.size() == 3) {
		std::move(rot.begin(), rot.end(), orientation);
		rotation = graphics::createEulerRotation(orientation);
		transformation = transformation * rotation;
	}
}

void newgraphics::location::move(std::initializer_list<float> pos) {
	if (pos.size() == 3) {
		std::move(pos.begin(), pos.end(), position);
		translation = graphics::createTranslation(position);
		transformation = transformation * translation;
	}
}

void newgraphics::location::resize(float factor) {
	size = factor;
	scale = graphics::createScale(factor);
	transformation = transformation * scale;
}