#include "graphicHandler.h"


instance* instance::search(std::string name) {
	if (identifier == name) {	//return if it is the identifier
		return this;
	}
	//iterates through the children of the root of the search and then their children until it finds a match or runs out of instances
	for (auto it = children.begin(); it != children.end(); ++it) {
		instance* child = *it;
		instance* found = child->search(name);
		if (found != nullptr) {
			return found;
		}
	}
	return nullptr;	//return nothing if everything is searched through
}

void drawImplementation(instance* self,scene* myScene, mesh* myMesh, texture* myTexture, shader* myShader, float shiny, float reflect) {
	if (myMesh->vbo != 0) {
		glUseProgram(myShader->shaderID);	//set correct shader to be used

		//only bind a texture if there is one
		if (myTexture->textureID != 0) {
			glBindTexture(GL_TEXTURE_2D, myTexture->textureID);
		}

		//bind the correct vao and vbo before doing anything
		glBindVertexArray(myMesh->vao);
		glBindBuffer(GL_ARRAY_BUFFER, myMesh->vbo);

		//get locations of the uniforms in our shaders
		static const int lightPositions = glGetUniformLocation(myShader->shaderID, "lightPositions");
		static const int lightIntensities = glGetUniformLocation(myShader->shaderID, "lightIntensities");
		static const int cameraPos = glGetUniformLocation(myShader->shaderID, "cameraPosition");
		static const int ambient = glGetUniformLocation(myShader->shaderID, "ambient");
		static const int trans = glGetUniformLocation(myShader->shaderID, "trans");
		static const int cam = glGetUniformLocation(myShader->shaderID, "cam");
		static const int proj = glGetUniformLocation(myShader->shaderID, "proj");

		//get required matrices
		mat4 transformation = self->getTransformation();
		mat4 camera = myScene->currentCamera.getTransformation();
		mat4 perspective = myScene->currentCamera.getPerspective();

		//send required data to the shaders
		glUniform3fv(lightPositions, 1, myScene->lights[0].getPosition().array);
		glUniform1f(lightIntensities, myScene->lights[0].intensity);
		glUniform1f(ambient, myScene->ambientConstant);
		glUniform3fv(cameraPos, 1, myScene->currentCamera.getPosition().array);
		glUniformMatrix4fv(trans, 1, true, transformation.array);
		glUniformMatrix4fv(cam, 1, true, camera.array);
		glUniformMatrix4fv(proj, 1, true, perspective.array);

		//actually draw
		glDrawArrays(GL_TRIANGLES, 0, myMesh->size);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void instance::draw() {
	//if there is no drawImplementation check parent for one
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

	drawImplementation(this, myScene, myMesh, myTexture, myShader, shinyness, reflectivity);	//actually draw

	//iterate through children and tell them to draw themselves
	if (children.size() == 0) {
		return;
	}
	for (auto it = children.begin(); it != children.end(); ++it) {
		instance* child = *it;
		child->draw();
	}
}


void instance::changeParent(instance* newParent) {
	if (parent != nullptr) {	//remove self from old parents children
		for (auto it = parent->children.begin(); it != parent->children.end(); ++it) {
			if (*it == this) {
				parent->children.erase(it);
				break;
			}
		}
	}
	newParent->children.push_back(this);	//add self to new parents children
	parent = newParent;	//set new parent to parent value
}

/*
these are a lot of methods that just update the location values in a class
they all then create a matrix for the respective transformation
*/

//adds recursion to the functions so that any position update propogates down the tree
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


//reverses argument inputs so moving camera moves everything in the correct direction
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


//default location functions
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
	return translation * rotation * scale;	//returns the product of the 3 transformations stored by the location class
}

scene::scene() {	//create the window
	if (!glfwInit()) {	//exit if window creator couldnt be initialised
		exit(1);
	}

	//pass window arguments
	glfwSetErrorCallback(error_callback);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(800, 800, "Test Window", NULL, NULL);	//create the window
	if (!window) {	//exit if a window couldnt be created
		glfwTerminate();
		exit(1);
	}

	glfwMakeContextCurrent(window);
	if (!gladLoadGL()) {	//exit if functions couldnt be loaded
		glfwTerminate();
		exit(1);
	}

	glDepthFunc(GL_LESS);	//makes sure things obscured from the camera arent renderde
	glEnable(GL_DEPTH_TEST);
}