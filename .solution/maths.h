#pragma once

#define PI_f 3.14159265358979323846f
#define PI_d 3.14159265358979323846

#include <vector>
#include <cassert>
#include <cmath>
#include <iostream>

namespace graphicmath {
	class matrix {
	public:
		int height = 0;
		int width = 0;
		std::vector<float> array;

		// matrix constructor
		matrix(int h, int w, std::initializer_list<float> numbers);
		matrix(int d, std::initializer_list<float> numbers);
		matrix(int h, int w);
		matrix(int d);
		matrix();

		// matrix methods
		void transpose(); // delete later?
		matrix operator * (matrix mat);
		void operator = (std::initializer_list<float> numbers);
	};

	// common matrix aliases
	matrix matIdentity(); // assumes 4x4 identity matrix
	matrix matIdentity(int dimensions);
	matrix matIdentity(int dimensions, float diagValue);

	matrix vec3(std::initializer_list<float> numbers);
	matrix vec3();
	matrix vec4(std::initializer_list<float> numbers);
	matrix vec4();
	matrix mat4(std::initializer_list<float> numbers);
	matrix mat4();

	// these functions ONLY work with 3D using homogenous coordinates
	// common matrix types
	matrix matTranslation(matrix vec3);
	matrix matScale(float scaleFactor);
	matrix matRotation(matrix vec3, float rotation);
	matrix matPerspective(float fov, float aspect, float near, float far);
}

inline float toRad(float degrees) {
	return degrees * (PI_f / 180.0f);
};