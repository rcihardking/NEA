#include "maths.h"

inline static void initMatrix(int h, int w, graphicmath::matrix *mat, std::initializer_list<float>* numbers) {
	mat->height = h;
	mat->width = w;
	for (auto i = numbers->begin(); i != numbers->end(); i++) {
		mat->array.push_back(*i);
	}
}

inline static void initMatrix(int h, int w, graphicmath::matrix *mat) {
	mat->height = h;
	mat->width = w;
	mat->array.resize(w * h);
}


// matrix constructors
graphicmath::matrix::matrix(int h, int w, std::initializer_list<float> numbers) {
	initMatrix(h, w, this, &numbers);
}

graphicmath::matrix::matrix(int d, std::initializer_list<float> numbers) {
	initMatrix(d, d, this, &numbers);
}

graphicmath::matrix::matrix(int h, int w) {
	initMatrix(h, w, this);
}

graphicmath::matrix::matrix(int d) {
	initMatrix(d, d, this);
}

graphicmath::matrix::matrix() {}



// matrix methods
void graphicmath::matrix::transpose() { // should probably remove this method as opengl handles transposition
	bool sqr = width == height;
	std::vector<float> swapArray;
	swapArray.resize(width * height);

	int swap = width;
	width = height;
	height = swap;

	switch (sqr) {
	case false:
		assert(false); // non square matrix transposition doesnt work yet, remove assertion later

		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; i++) {
				swapArray[j * width + i] = array[j + i * width];
			}
		}

		break;
	case true:
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < width; j++) {
				swapArray[j * width + i] = array[j + i * width];
			}
		}
		array.assign(swapArray.begin(), swapArray.end());
		break;
	}
}

graphicmath::matrix graphicmath::matrix::operator * (matrix mat) { // matrix multiplication
	assert(this->width == mat.height);
	matrix newMatrix(this->height, mat.width);

	// algorithm
	for (int k = 0; k < this->height; k++) {
		for (int j = 0; j < this->width; j++) {
			for (int i = 0; i < mat.width; i++) {
				newMatrix.array[i + k * mat.width] += this->array[j + k * this->width] * mat.array[i + j * mat.width];
			}
		}
	}

	return newMatrix;
}


void graphicmath::matrix::operator = (std::initializer_list<float> numbers) { // quick reassign
	assert(numbers.size() <= height * width);

	array.clear();
	for (auto i = numbers.begin(); i != numbers.end(); i++) {
		array.push_back(*i);
	}
}


 // common matrix aliases
graphicmath::matrix graphicmath::matIdentity() { // assumes 4x4 identity matrix
	matrix newMatrix(4);

	for (int i = 0; i < 4; i++) {
		newMatrix.array[i + i * newMatrix.width] = 1.0f;
	}

	return newMatrix;
}

graphicmath::matrix graphicmath::matIdentity(int dimensions) {
	matrix newMatrix(dimensions);

	for (int i = 0; i < dimensions; i++) {
		newMatrix.array[i + i * newMatrix.width] = 1.0f;
	}

	return newMatrix;
}

graphicmath::matrix graphicmath::matIdentity(int dimensions, float diagValue) {
	matrix newMatrix(dimensions);

	for (int i = 0; i < dimensions; i++) {
		newMatrix.array[i + i * newMatrix.width] = diagValue;
	}

	return newMatrix;
}

graphicmath::matrix graphicmath::vec3(std::initializer_list<float> numbers) {
	 matrix newMatrix;
	 initMatrix(3, 1, &newMatrix, &numbers);
	 return newMatrix;
 }

graphicmath::matrix graphicmath::vec3() {
	matrix newMatrix;
	initMatrix(3, 1, &newMatrix);
	return newMatrix;
}

graphicmath::matrix graphicmath::vec4(std::initializer_list<float> numbers) {
	 matrix newMatrix;
	 initMatrix(4, 1, &newMatrix, &numbers);
	 return newMatrix;
 }

graphicmath::matrix graphicmath::vec4() {
	matrix newMatrix;
	initMatrix(4, 1, &newMatrix);
	return newMatrix;
}

graphicmath::matrix graphicmath::mat4(std::initializer_list<float> numbers) {
	matrix newMatrix;
	initMatrix(4, 4, &newMatrix, &numbers);
	return newMatrix;
}

graphicmath::matrix graphicmath::mat4() {
	matrix newMatrix;
	initMatrix(4, 4, &newMatrix);
	return newMatrix;
}

 // common matrix types
graphicmath::matrix graphicmath::matTranslation(graphicmath::matrix vec3) {
	assert(vec3.width == 1 && vec3.height == 3);

	matrix newMatrix = matIdentity(4);
	for (int i = 0; i < 3; i++) {
		newMatrix.array[3 + i * 4] = vec3.array[i];
	}
	return newMatrix;
}

graphicmath::matrix graphicmath::matScale(float scaleFactor) {
	matrix newMatrix = matIdentity(4);
	for (int i = 0; i < 3; i++) {
		newMatrix.array[i + i * 4] = scaleFactor;
	}
	return newMatrix;
}

graphicmath::matrix graphicmath::matRotation(graphicmath::matrix vec3, float rotation) {
	assert(vec3.width == 1 && vec3.height == 3);

	float sinValue = sin(rotation);
	float cosValue = cos(rotation);
	float m1Cos = 1 - cosValue;

	float x = vec3.array[0];
	float y = vec3.array[1];
	float z = vec3.array[2];
	assert(x <= 1 && y <= 1 && z <= 1);

	matrix newMatrix = matIdentity(4);

	newMatrix.array[0] = cosValue + x * x * m1Cos; newMatrix.array[1] = x * y * m1Cos - z * sinValue; newMatrix.array[2] = x * z * m1Cos + y * sinValue;
	newMatrix.array[4] = y * x * m1Cos + z * sinValue; newMatrix.array[5] = cosValue + y * y * m1Cos; newMatrix.array[6] = y * z * m1Cos - x * sinValue; 
	newMatrix.array[8] = z * x * m1Cos - y * sinValue; newMatrix.array[9] = z * y * m1Cos + x * sinValue; newMatrix.array[10] = cosValue + z * z * m1Cos;

	newMatrix.array[15] = 1;

	return newMatrix;
}

graphicmath::matrix graphicmath::matPerspective(float fov, float aspect, float near, float far) {
	float f = 1.0f / tan(fov / 2.0f);
	matrix newMatrix = mat4();

	newMatrix.array[0] = f / aspect;
	newMatrix.array[5] = f;
	newMatrix.array[10] = (far + near) / (far - near);
	newMatrix.array[14] = -1.0f;
	newMatrix.array[11] = (2 * far * near) / (far - near);

	return newMatrix;
}