#include <vector>
#include <array>
#include <fstream>
#include <sstream>
#include <map>
#include <cassert>

#include "Model.h"

Model::Model(const std::string& fileName) {
	std::vector<std::array<float, 3>> verts;
	std::vector<std::array<float, 3>> normals;
	std::vector<ushort> iboData;
	std::vector<float> vboData;
	std::map<std::pair<ushort, ushort>, ushort> vertNormal2index;
	ushort currIdx = 0;

	std::ifstream in(fileName);
	std::string line;
	while (std::getline(in, line)) {
		std::istringstream lineIn(line);
		std::string type;
		lineIn >> type;
		if (type == "v") {
			verts.emplace_back(std::array<float, 3>());
			for (int i = 0; i < 3; ++i) {
				lineIn >> verts.back()[i];
			}
		}
		else if (type == "vn") {
			normals.emplace_back(std::array<float, 3>());
			for (int i = 0; i < 3; ++i) {
				lineIn >> normals.back()[i];
			}
		}
		else if (type == "f") {
			for (int i = 0; i < 3; ++i) {
				std::string s;
				lineIn >> s;
				int slash = s.find('/');
				assert(slash != std::string::npos);
				assert(slash + 1 < s.size());
				ushort v = std::stoi(s.substr(0, slash + 1));
				ushort vn = std::stoi(s.substr(slash + 2));
				std::pair<ushort, ushort> key = std::make_pair(v, vn);
				auto it = vertNormal2index.find(key);
				if (it == vertNormal2index.end()) {
					vertNormal2index[key] = currIdx++;
					for (int i = 0; i < 3; ++i) {
						vboData.push_back(verts[v-1][i]);
					}
					for (int i = 0; i < 3; ++i) {
						vboData.push_back(normals[vn-1][i]);
					}
				}
				iboData.push_back(vertNormal2index[key]);
			}
		}
	}

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vboData.size() * sizeof(float), (void*)vboData.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, iboData.size() * sizeof(ushort), (void*)iboData.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // unbind vao first so it remembers buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	m_nVerts = verts.size();
	m_nIndices = iboData.size();
}

Model::~Model() {
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ibo);
}

void Model::draw() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_nIndices, GL_UNSIGNED_SHORT, (void*)0);
}