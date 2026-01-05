#pragma once
#include <iostream>
#include <initializer_list>
#include <cmath>

#define PI 3.14159265359
#define TORADFACTOR 0.01745329251

template <int h, int w>
struct matrix {
	float array[h * w] = {};
	static constexpr int height = h; // so other matrices can be created using width and height
	static constexpr int width = w;
	bool identity = false;

	inline matrix() {};

	inline matrix(std::initializer_list<float> init) {
		if (init.size() <= h * w) {
			std::move(init.begin(), init.end(), array); // dont know if this is undefined behaviour
		}
		else { // only here to stop buffer overflows
			std::cout << "an initalizer list provided is larger than matrix size" << "\n";
			std::move(init.begin(), init.begin() + h * w, array);
		}
	};
	inline matrix(float init[h*w]) {
		std::move(init, init + h * w, array);
	};

	template<class T>
	inline auto operator* (T other) {
		static_assert(width == other.height, "attempted to multiply 2 incompatable matrices");
		// compiler error avoids a runtime crash due to a programmer error
		if (this->identity == true) {
			return other;
		}
		else if (other.identity == true) {
			return *this;
		}

		matrix<h, other.width> newMatrix;

		for (int k = 0; k < h; ++k) {
			for (int j = 0; j < width; ++j) {
				for (int i = 0; i < other.width; ++i) {
					newMatrix.array[i + k * other.width] += array[j + k * width] * other.array[i + j * other.width];
				}
			}
		}

		return newMatrix;
	};

	template<class T>
	inline auto operator+ (T other) {
		static_assert(width == other.width && height == other.height, "attempted to add matricies of different dimensions");

		matrix<h, w> newMatrix;

		for (int i = 0; i < h * w; ++i) {
			newMatrix.array[i] = this->array[i] + other.array[i];
		}

		return newMatrix;
	}
};

typedef matrix<4, 4> mat4;
typedef matrix<4, 1> vec4;
typedef matrix<3, 1> vec3;

inline mat4 iden4() {
	mat4 newMatrix = { 
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	newMatrix.identity = true;
	return newMatrix;
};

namespace graphics {
	mat4 createEulerRotation(float x, float y, float z);
	mat4 createEulerRotation(float rot[3]);
	mat4 createTranslation(float x, float y, float z);
	mat4 createTranslation(float pos[3]);
	mat4 createPerspective(float fov, float aspect, float far, float near);
	mat4 createScale(float factor);
};

inline float toRad(float degrees) { return static_cast<float>(degrees * TORADFACTOR); }