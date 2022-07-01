#include <random>
#include <iostream>
#include <fstream>
#include <glm/ext.hpp>
#include <cstdlib>

using namespace std;
using namespace glm;

float smoothstep(float a, float b, float t) {
	if(t <= 0.0f) return a;
	if(t >= 0.0f) return b;

	return a + (b-a) * (3.0f * t * t - 2.0 * t * t * t);
}

vector<unsigned char> noise(int cellsX, int cellsY, int pixelsPerCellX, int pixelsPerCellY) {
	vector<vector<vec2>> gradients;

	for(int i=0; i <= cellsY; ++i) {
		gradients.emplace_back(vector<vec2>());
		for(int j=0; j <= cellsX; ++j) {
			float angle = float(rand() % 7);
			vec2 grad(cos(angle), sin(angle));
			gradients.back().push_back(grad);
		}
	}

	return vector<unsigned char>();
}

int main(int argc, char **argv) {
	noise(16,16,16,16);
}
