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

graphics::shader::shader(std::string vertexFilepath, std::string fragmentFilepath) { 
	loadShader(&ID, vertexFilepath, fragmentFilepath);
}

graphics::shader::~shader() {
	if (ID) { glDeleteProgram(ID); }
}

int graphics::scene::loadMesh(std::string meshFilepath, GLuint vao) {
	graphics::mesh newMesh;
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

int graphics::scene::loadImage(std::string imageFilepath) {
	int width, height;
	unsigned char* data = file::readImage(imageFilepath, &width, &height);
	if (data == nullptr) {
		// need to add expection handling
		assert(false);
	}

	graphics::texture newTexture;

	glGenTextures(1, &newTexture.img);
	glBindTexture(GL_TEXTURE_2D, newTexture.img);

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



graphics::instance* graphics::instance::search(std::string name) {
	if (identifier == name) {
		return this;
	}
	for (auto it = children.begin(); it != children.end(); ++it) {
		instance* child = *it;
		instance* found = child->search(name);
		if (found != nullptr) {
			return found;
		}
	}
	return nullptr;
}

void drawImplementation(graphics::instance* self) {
	if (self->meshIndex != 0) {
		glUseProgram(self->myShader->ID);
		graphics::mesh* myMesh = &self->myScene->meshes[self->meshIndex - 1];

		if (self->textureIndex != 0) {
			graphics::texture* myTexture = &self->myScene->textures[self->textureIndex - 1];
			glBindTexture(GL_TEXTURE_2D, myTexture->img);
		}
		glBindVertexArray(myMesh->vao);
		glBindBuffer(GL_ARRAY_BUFFER, myMesh->vbo);

		static const int lightPos = glGetUniformLocation(self->myShader->ID, "lightPosition");
		static const int trans = glGetUniformLocation(self->myShader->ID, "trans");
		static const int cam = glGetUniformLocation(self->myShader->ID, "cam");
		static const int proj = glGetUniformLocation(self->myShader->ID, "proj");
		mat4 transformation = self->getTransformation();
		mat4 camera = self->myScene->currentCamera.getTransformation();

		float lightPosition[3] = { 5.0f, 5.0f, 5.0f };
		glUniform3fv(lightPos, 1, lightPosition);
		glUniformMatrix4fv(trans, 1, true, transformation.array);
		glUniformMatrix4fv(cam, 1, true, camera.array);
		glUniformMatrix4fv(proj, 1, true, self->myScene->perspective.array);

		glDrawArrays(GL_TRIANGLES, 0, myMesh->size);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	const std::vector<graphics::instance*> children = self->getChildren();
	if (children.size() == 0) {
		return;
	}
	for (auto it = children.begin(); it != children.end(); ++it) {
		graphics::instance* child = *it;
		child->draw();
	}
}

void graphics::instance::draw() {
	if (drawImplementation == nullptr) {
		drawImplementation = this->parent->drawImplementation;
		if (drawImplementation == nullptr) {
			assert(false); // trying to draw without a draw implementation
		}
	}
	drawImplementation(this);

	if (children.size() == 0) {
		return;
	}
	for (auto it = children.begin(); it != children.end(); ++it) {
		instance* child = *it;
		child->draw();
	}
}


void graphics::instance::changeParent(instance* newParent) {
	if (parent != nullptr) {
		for (auto it = parent->children.begin(); it != parent->children.end(); ++it) {
			if (*it == this) {
				parent->children.erase(it);
				break;
			}
		}
	}
	newParent->children.push_back(this);
	parent = newParent;
}

void graphics::instance::rotate(std::initializer_list<float> rot) {
	if (rot.size() != 3) {
		return;
	}
	std::move(rot.begin(), rot.end(), orientation);
	rotation = graphics::createEulerRotation(orientation);

	if (children.size() == 0) {
		return;
	}
	for (auto it = children.begin(); it != children.end(); ++it) {
		instance* child = *it;
		child->rotate(rot);
	}
}

void graphics::instance::rotate(vec3 rot) {
	std::move(rot.array, rot.array + 3, orientation);
	rotation = graphics::createEulerRotation(orientation);

	if (children.size() == 0) {
		return;
	}
	for (auto it = children.begin(); it != children.end(); ++it) {
		instance* child = *it;
		child->rotate(rot);
	}
}

void graphics::instance::move(std::initializer_list<float> pos) {
	if (pos.size() != 3) {
		return;
	}
	std::move(pos.begin(), pos.end(), position);
	translation = graphics::createTranslation(position);

	if (children.size() == 0) {
		return;
	}
	for (auto it = children.begin(); it != children.end(); ++it) {
		instance* child = *it;
		child->move(pos);
	}
}

void graphics::instance::move(vec3 pos) {
	std::move(pos.array, pos.array + 3, position);
	translation = graphics::createTranslation(position);

	if (children.size() == 0) {
		return;
	}
	for (auto it = children.begin(); it != children.end(); ++it) {
		instance* child = *it;
		child->move(pos);
	}
}

void graphics::instance::resize(float factor) {
	size = factor;
	scale = graphics::createScale(factor);

	if (children.size() == 0) {
		return;
	}
	for (auto it = children.begin(); it != children.end(); ++it) {
		instance* child = *it;
		child->resize(factor);
	}
}



void graphics::camera::rotate(std::initializer_list<float> rot) {
	if (rot.size() != 3) {
		std::move(rot.begin(), rot.end(), orientation);
		rotation = graphics::createEulerRotation(-orientation[0], -orientation[1], -orientation[2]);
	}
}

void graphics::camera::rotate(vec3 rot) {
	std::move(rot.array, rot.array + 3, orientation);
	rotation = graphics::createEulerRotation(-orientation[0], -orientation[1], -orientation[2]);
}

void graphics::camera::move(std::initializer_list<float> pos) {
	if (pos.size() == 3) {
		std::move(pos.begin(), pos.end(), position);
		translation = graphics::createTranslation(-position[0], -position[1], -position[2]);
	}
}

void graphics::camera::move(vec3 pos) {
	std::move(pos.array, pos.array + 3, position);
	translation = graphics::createTranslation(-position[0], -position[1], -position[2]);
}

void graphics::camera::resize(float factor) {
	size = factor;
	scale = graphics::createScale(-factor);
}



void graphics::location::rotate(std::initializer_list<float> rot) {
	if (rot.size() == 3) {
		std::move(rot.begin(), rot.end(), orientation);
		rotation = graphics::createEulerRotation(orientation);
	}
}

void graphics::location::rotate(vec3 rot) {
	std::move(rot.array, rot.array + 3, orientation);
	rotation = graphics::createEulerRotation(orientation);
}

void graphics::location::move(std::initializer_list<float> pos) {
	if (pos.size() == 3) {
		std::move(pos.begin(), pos.end(), position);
		translation = graphics::createTranslation(position);
	}
}

void graphics::location::move(vec3 pos) {
	std::move(pos.array, pos.array + 3, position);
	translation = graphics::createEulerRotation(position);
}

void graphics::location::resize(float factor) {
	size = factor;
	scale = graphics::createScale(factor);
}

mat4 graphics::location::getTransformation() {
	return translation * rotation * scale;
}
