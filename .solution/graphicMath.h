#pragma once
#include <array>
#include <initializer_list>
#include <iostream>
#include <stdarg.h>
#include <assert.h>
#include <tuple>

namespace graphicsmath {
	template <int h, int w>
	class matrix {
	public:
		inline matrix(int a, ...) {
			array = new float[h * w];

			va_list args;
			va_start(args, h * w);

			for (int i = 0; i < h * w; ++i) {
				array[i] = va_arg(args, int);
			}

			va_end(args);
		};

		inline std::tuple<float*, int, int> operator* (matrix mat) {
			if (w != mat.height) {
				std::cout << "tried to multiply incompatible matricies";
				assert(false);
			}
			
			float* newArray = new float[h * mat.width];
			
			for (int k = 0; k < this->height; k++) {
				for (int j = 0; j < this->width; j++) {
					for (int i = 0; i < mat.width; i++) {
						newArray[i + k * mat.width] += this->array[j + k * this->width] * mat.array[i + j * mat.width];
					}
				}
			}

			return std::make_tuple(newArray, h, mat.width)
		};

		float* array;

		const int height = h;
		const int width = w;
	};
}