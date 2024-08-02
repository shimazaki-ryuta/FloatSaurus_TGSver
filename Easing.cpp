#include "Easing.h"
#define _USE_MATH_DEFINES
#include <math.h>
float EaseIn(float x, int exponent) {
	return powf(x, float(exponent));
}

float EaseOut(float x, int exponent) {
	return 1 - powf(1.0f - x, float(exponent));
}

float EaseInOut(float x, int exponent) {
	return x < 0.5 ? 2 * x * x : 1 - powf(-2 * x + 2, float(exponent)) / 2.0f;
}


float EaseInBack(float x, int exponent) {
	float c1 = 1.70158f;
	float c3 = c1 + 1.0f;

	return c3 * x * powf(x, float(exponent)) - c1 * powf(x, float(exponent));
}

float EaseOutBack(float x, int exponent) {
	float c1 = 1.70158f;
	float c3 = c1 + 1.0f;

	return 1.0f + c3 * -fabsf(powf(x - 1.0f, float(exponent) + 1.0f)) + c1 * fabsf(powf(x - 1.0f, float(exponent)));
}