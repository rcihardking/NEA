#include "graphicMath.h"

matrix<4,4> createEulerRotation(float x, float y, float z) {
	float cosX = cosf(x);
	float sinX = sinf(x);
	float cosY = cosf(y);
	float sinY = sinf(y);
	float cosZ = cosf(z);
	float sinZ = sinf(z);

	matrix<4,4> rotX = { 
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cosX, -sinX, 0.0f,
		0.0f, sinX, cosX, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	matrix<4,4> rotY = {
		cosY, 0.0f, sinY, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		-sinY, 0.0f, cosY, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	matrix<4,4> rotZ = {
		cosZ, -sinZ, 0.0f, 0.0f,
		sinZ, cosZ, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	return rotY * (rotX * rotZ);
}

matrix<4, 4> createPerspective(float fov, float aspect, float near, float far) {
	float f = 1.0f / tanf(fov / 2.0f);
	matrix<4, 4> persp = {
		f / aspect, 0.0f, 0.0f, 0.0f,
		0.0f, f, 0.0f, 0.0f,
		0.0f, 0.0f, (far + near) / (far - near), (2 * far * near) / (far - near),
		0.0f, 0.0f, -1.0f, 0.0f
	};
	return persp;
}

matrix<4, 4> createTranslation(float x, float y, float z) {
	matrix<4, 4> trans = {
		1.0f, 0.0f, 0.0f, x,
		0.0f, 1.0f, 0.0f, y,
		0.0f, 0.0f, 1.0f, z,
		0.0f, 0.0f, 0.0f, 1
	};
	return trans;
}

matrix<4, 4> createScale(float factor) {
	matrix <4, 4> scale = {
		factor, 0.0f, 0.0f, 0.0f,
		0.0f, factor, 0.0f, 0.0f,
		0.0f, 0.0f, factor, 0.0f,
		0.0f, 0.0f, 0.0f, factor
	};
	return scale;
}
