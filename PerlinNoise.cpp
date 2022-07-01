#include <random>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <array>
#include <vector>
#include <cassert>
#include <string>
#include <cstring>
#include <glm/ext.hpp>

using namespace std;
using namespace glm;

typedef unsigned uint;

unsigned seed;
mt19937 randGen;


float smoothIntperolate (float a, float b, float t) {
	if(t <= 0.0f) return a;
	if(t >= 1.0f) return b;
	// 6t**5 - 15t**4 + 10t**3
	float u = ((6.0f * t  - 15.0f) * t + 10.0f) * t * t * t;

	return a * (1.0f-u) + b * u;
}

class Perlin {
public:
	Perlin(uint nCells);
	float operator()(float x, float y);
private:
	uint nCells;
	vector<vector<vec2>> m_gradients;
};

Perlin::Perlin(uint nCells) : nCells(nCells) {
	uniform_real_distribution<float> thetaDist(0.0f, 2 * pi<float>());
	for (int i = 0; i <= nCells; ++i) {
		m_gradients.push_back(vector<vec2>());
		for (int j = 0; j <= nCells; ++j) {
			float theta = thetaDist(randGen);
			m_gradients.back().push_back(vec2(cos(theta), sin(theta)));
		}
	}
}

float Perlin::operator()(float x, float y) {
	assert(x >= 0.0 && y >= 0.0f && x < float(nCells) && y < float(nCells));
	int gridX = int(x);
	int gridY = int(y);
	float fracX = x - gridX;
	float fracY = y - gridY;
	float dot00 = dot(m_gradients[gridX][gridY], vec2(fracX, fracY));
	float dot10 = dot(m_gradients[gridX+1][gridY], vec2(fracX - 1.0f, fracY));
	float dot01 = dot(m_gradients[gridX][gridY + 1], vec2(fracX, fracY - 1.0f));
	float dot11 = dot(m_gradients[gridX + 1][gridY + 1], vec2(fracX - 1.0f, fracY - 1.0f));
	float interp0 = smoothIntperolate(dot00, dot10, fracX);
	float interp1 = smoothIntperolate(dot01, dot11, fracX);
	return smoothIntperolate(interp0, interp1, fracY);
}

void writeBMP(const string& fileName, uint w, uint h, char* data) {
	unsigned short header[27];
	memset(header, 0, sizeof(header));
	uint colourTable[256];
	uint dataSize = w * h;
	uint fileSize = sizeof(header) + sizeof(colourTable) + dataSize;
	header[0] = 0x4d42; // magic number
	header[1] = fileSize & 0xffff; // file size lower bytes
	header[2] = fileSize >> 16; // // file size upper bytes
	header[5] = sizeof(header) + sizeof(colourTable); // data offset
	header[7] = 40; // size of info header
	header[9] = w; // width
	header[11] = h; // height
	header[13] = 1; // number of colour planes
	header[14] = 8; // bits per pixel
	header[15] = 0; // no compression
	header[19] = 2834; // horizontal resolution
	header[21] = 2834; // vertical resolution
	header[23] = 256; // number of colours
	for (uint i = 0; i < 256; ++i) {
		colourTable[i] = i | (i << 8) | (i << 16);
	}

	ofstream o(fileName, ios::binary);
	assert(o);
	o.write((char*)header, sizeof(header));
	o.write((char*)colourTable, sizeof(colourTable));
	o.write(data, dataSize);
}

char* genNoiseGrid(int nCells, int samplesPerCell) {
	float* grid = new float[nCells * nCells * samplesPerCell * samplesPerCell];
	int idx = 0;
	Perlin p(nCells);
	uniform_real_distribution<float> sampleOffset(0.0f, 1.0f / samplesPerCell);
	float minNoise = 100.0f;
	float maxNoise = -100.0f;
	for (int i = 0; i < nCells * samplesPerCell; ++i) {
		for (int j = 0; j < nCells * samplesPerCell; ++j) {
			float y = i / float(samplesPerCell) + sampleOffset(randGen);
			float x = j / float(samplesPerCell) + sampleOffset(randGen);
			float n = p(x, y);
			grid[idx++] = n;
			minNoise = glm::min(minNoise, n);
			maxNoise = glm::max(maxNoise, n);
		}
	}

	char* cgrid = new char[nCells * nCells * samplesPerCell * samplesPerCell];

	for (int i = 0; i < nCells * nCells * samplesPerCell * samplesPerCell; ++i) {
		cgrid[i] = char((grid[i] - minNoise) / (maxNoise - minNoise) * 255.0f);
	}

	delete[] grid;

	return cgrid;
}

int main(int argc, char **argv) {
	seed = 2345234453;
	randGen = mt19937(seed);

	int nCells = 4;
	int samplesPerCell = 64;
	char* data = genNoiseGrid(nCells, samplesPerCell);
	
	writeBMP("textures/noise.bmp", nCells*samplesPerCell, nCells * samplesPerCell, data);

	delete[] data;
}
