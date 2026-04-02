#include "graphicHandler.h"


instance* instance::search(std::string name) {
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

void drawImplementation(instance* self,scene* myScene, mesh* myMesh, texture* myTexture, shader* myShader, float shiny, float reflect) {
	if (myMesh->vbo != 0) {
		glUseProgram(myShader->shaderID);

		if (myTexture->textureID != 0) {
			glBindTexture(GL_TEXTURE_2D, myTexture->textureID);
		}
		glBindVertexArray(myMesh->vao);
		glBindBuffer(GL_ARRAY_BUFFER, myMesh->vbo);

		static const int lightPositions = glGetUniformLocation(myShader->shaderID, "lightPositions");
		static const int lightIntensities = glGetUniformLocation(myShader->shaderID, "lightIntensities");
		static const int cameraPos = glGetUniformLocation(myShader->shaderID, "cameraPosition");
		static const int trans = glGetUniformLocation(myShader->shaderID, "trans");
		static const int cam = glGetUniformLocation(myShader->shaderID, "cam");
		static const int proj = glGetUniformLocation(myShader->shaderID, "proj");
		mat4 transformation = self->getTransformation();
		mat4 camera = myScene->currentCamera.getTransformation();


		glUniform3fv(lightPositions, 1, myScene->lights[0].getPosition().array);
		glUniform1f(lightIntensities, myScene->lights[0].intensity);
		glUniform3fv(cameraPos, 1, myScene->currentCamera.getPosition().array);
		glUniformMatrix4fv(trans, 1, true, transformation.array);
		glUniformMatrix4fv(cam, 1, true, camera.array);
		glUniformMatrix4fv(proj, 1, true, myScene->currentCamera.getPerspective().array);

		glDrawArrays(GL_TRIANGLES, 0, myMesh->size);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void instance::draw() {
	if (drawImplementation == nullptr) {
		if (this->parent == nullptr) {
			std::cout << "could not find a draw implementation\n";
			return;
		}
		if (this->parent->drawImplementation == nullptr) {
			std::cout << "could not find a draw implementation\n";
			return;
		}
	}

	drawImplementation(this, myScene, myMesh, myTexture, myShader, shinyness, reflectivity);

	if (children.size() == 0) {
		return;
	}
	for (auto it = children.begin(); it != children.end(); ++it) {
		instance* child = *it;
		child->draw();
	}
}


void instance::changeParent(instance* newParent) {
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

void instance::rotate(std::initializer_list<float> rot) {
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

void instance::rotate(vec3 rot) {
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

void instance::move(std::initializer_list<float> pos) {
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

void instance::move(vec3 pos) {
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

void instance::resize(float factor) {
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



void camera::rotate(std::initializer_list<float> rot) {
	if (rot.size() != 3) {
		std::move(rot.begin(), rot.end(), orientation);
		rotation = graphics::createEulerRotation(-orientation[0], -orientation[1], -orientation[2]);
	}
}

void camera::rotate(vec3 rot) {
	std::move(rot.array, rot.array + 3, orientation);
	rotation = graphics::createEulerRotation(-orientation[0], -orientation[1], -orientation[2]);
}

void camera::move(std::initializer_list<float> pos) {
	if (pos.size() == 3) {
		std::move(pos.begin(), pos.end(), position);
		translation = graphics::createTranslation(-position[0], -position[1], -position[2]);
	}
}

void camera::move(vec3 pos) {
	std::move(pos.array, pos.array + 3, position);
	translation = graphics::createTranslation(-position[0], -position[1], -position[2]);
}

void camera::resize(float factor) {
	size = factor;
	scale = graphics::createScale(-factor);
}

void camera::changePerspective(float fov, float aspect, float n, float f) {
	perspective = graphics::createPerspective(fov, aspect, n, f);
}


void location::rotate(std::initializer_list<float> rot) {
	if (rot.size() == 3) {
		std::move(rot.begin(), rot.end(), orientation);
		rotation = graphics::createEulerRotation(orientation);
	}
}

void location::rotate(vec3 rot) {
	std::move(rot.array, rot.array + 3, orientation);
	rotation = graphics::createEulerRotation(orientation);
}

void location::move(std::initializer_list<float> pos) {
	if (pos.size() == 3) {
		std::move(pos.begin(), pos.end(), position);
		translation = graphics::createTranslation(position);
	}
}

void location::move(vec3 pos) {
	std::move(pos.array, pos.array + 3, position);
	translation = graphics::createEulerRotation(position);
}

void location::resize(float factor) {
	size = factor;
	scale = graphics::createScale(factor);
}

mat4 location::getTransformation() {
	return translation * rotation * scale;
}
