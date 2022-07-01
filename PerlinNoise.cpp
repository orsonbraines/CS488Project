#include <random>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <array>
#include <vector>
#include <glm/ext.hpp>

using namespace std;
using namespace glm;

typedef unsigned uint;

unsigned seed;

float smoothIntperolate (float a, float b, float t) {
	if(t <= 0.0f) return a;
	if(t >= 0.0f) return b;
	// 6t**5 - 15t**4 + 10t**3
	float u = ((6.0f * t  - 15.0f) * t + 10.0f) * t * t * t;

	return a * (1.0f-u) + b * u;
}

int hashPoint(int x, int y, const vector<int> &P, const int nGradients) {
	return P[(P[x] + y) % P.size()] % nGradients;
}

float dotGradient(int gridX, int gridY, float x, float y, const vec2 &grad) {
	return dot(vec2(x - gridX, y - gridY), grad);
}

vector<unsigned char> noise(int nCells, int samplesPerCell) {
	// permutation array
	vector<int> P;
	for (int i = 0; i <= nCells; ++i) {
		P.push_back(i);
	}
	// random number generator
	mt19937 r(seed);
	shuffle(P.begin(), P.end(), r);

	float s2 = glm::inversesqrt(2.0f);
	uniform_real_distribution<float> thetaDist(0, 2.0f * glm::pi<float>());

	const int nGradients = 16;
	array<vec2, nGradients> gradients;
	for (int i = 0; i < nGradients; ++i) {
		float theta = thetaDist(r);
		gradients[i] = vec2(cos(theta), sin(theta));
	}

	vector<unsigned char> result(nCells* nCells*samplesPerCell* samplesPerCell);
	int idx = 0;

	for (int y = 0; y < nCells; ++y) {
		for (int x = 0; x < nCells; ++x) {
			vec2 g00 = gradients[hashPoint(x, y, P, nGradients)];
			vec2 g01 = gradients[hashPoint(x, y+1, P, nGradients)];
			vec2 g10 = gradients[hashPoint(x+1, y, P, nGradients)];
			vec2 g11 = gradients[hashPoint(x+1, y+1, P, nGradients)];
			for (int fy = 0; fy < samplesPerCell; ++fy) {
				for (int fx = 0; fx < samplesPerCell; ++fx) {
					float fracX = float(fx) / samplesPerCell;
					float fracY = float(fy) / samplesPerCell;
					float dot00 = dotGradient(x, y, x + fracX, x + fracY, g00);
					float dot01 = dotGradient(x, y + 1, x + fracX, x + fracY, g01);
					float dot10 = dotGradient(x + 1, y, x + fracX, x + fracY, g10);
					float dot11 = dotGradient(x + 1, y + 1, x + fracX, x + fracY, g11);
					float lerp1 = smoothIntperolate(dot00, dot10, fracX);
					float lerp2 = smoothIntperolate(dot01, dot11, fracX);
					float bilerp = smoothIntperolate(lerp1, lerp2, fracY);
					int ax = x * samplesPerCell + fx;
					int ay = y * samplesPerCell + fy;
					result[ay * nCells * samplesPerCell + ax] = unsigned char(bilerp * 128.0f + 127.0f);
				}
			}
		}
	}

	return result;
}

int main(int argc, char **argv) {
	seed = 32478959;
	vector<unsigned char> noiseData = noise(16,64);
	ofstream o("textures/noise");
	o.write((char*)noiseData.data(), noiseData.size());
}
