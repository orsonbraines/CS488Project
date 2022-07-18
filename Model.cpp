#include <vector>
#include <array>
#include <fstream>
#include <sstream>
#include <map>
#include <cassert>
#include <glm/ext.hpp>

#include "Model.h"
#include "Util.h"

uint Model::s_prevId = 0;

static std::map<std::string, Material> loadMaterials(const std::string& mtlFile) {
	std::map<std::string, Material> mats;

	std::string curMat = "";
	std::ifstream in(mtlFile);
	if (!in) {
		throw GraphicsException("Unable to open: " + mtlFile);
	}
	std::string line;
	while (std::getline(in, line)) {
		std::istringstream lineIn(line);
		std::string type;
		lineIn >> type;
		if (type == "newmtl") {
			lineIn >> curMat;
			mats[curMat];
		}
		else if (type == "Ns") {
			lineIn >> mats[curMat].Ns;
		}
		else if (type == "Ks") {
			float r, g, b;
			lineIn >> r >> g >> b;
			mats[curMat].Ks = glm::vec3(r,g,b);
		}
		else if (type == "Kd") {
			float r, g, b;
			lineIn >> r >> g >> b;
			mats[curMat].Kd = glm::vec3(r, g, b);
		}
	}

	return mats;
}

Model::Model(const std::string& fileName) : m_isTextured(false), m_id(++s_prevId) {
	std::vector<std::array<float, 3>> verts;
	std::vector<std::array<float, 3>> normals;
	std::vector<std::array<float, 2>> uvs;
	std::vector<ushort> iboData;
	std::vector<float> vboData;
	std::map<std::array<ushort, 3>, ushort> vertNormal2index;
	ushort currIdx = 0;
	std::map<std::string, Material> mtllib;
	

	int lastSlash = fileName.rfind("/");
	std::string filePrefix = lastSlash == std::string::npos ? "" : fileName.substr(0, lastSlash + 1);
	std::ifstream in(fileName);
	if (!in) {
		throw GraphicsException("Unable to open: " + fileName);
	}
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
		else if (type == "vt") {
			m_isTextured = true;
			uvs.emplace_back(std::array<float, 2>());
			for (int i = 0; i < 2; ++i) {
				lineIn >> uvs.back()[i];
			}
		}
		else if (type == "f") {
			for (int i = 0; i < 3; ++i) {
				std::string s;
				lineIn >> s;
				std::array<ushort, 3> key{0,0,0};
				int keyIdx = 0;

				// read in the vertices vith format v/vt/vn
				for (char ch : s) {
					if (ch == '/') {
						++keyIdx;
					}
					else {
						key[keyIdx] = key[keyIdx] * 10 + (ch - '0');
					}
				}
				
				auto it = vertNormal2index.find(key);
				if (it == vertNormal2index.end()) {
					vertNormal2index[key] = currIdx++;
					for (int j = 0; j < 3; ++j) {
						vboData.push_back(verts[key[0] - 1][j]);
					}
					for (int j = 0; j < 3; ++j) {
						vboData.push_back(normals[key[2] - 1][j]);
					}
					if (m_isTextured) {
						for (int j = 0; j < 2; ++j) {
							vboData.push_back(uvs[key[1] - 1][j]);
						}
					}
				}
				iboData.push_back(vertNormal2index[key]);
			}
		}
		else if (type == "mtllib") {
			std::string name;
			lineIn >> name;
			mtllib = loadMaterials(filePrefix + name);
		}
		else if (type == "usemtl") {
			std::string mtl;
			lineIn >> mtl;
			m_mtls.emplace_back(std::make_pair(iboData.size(), mtllib[mtl]));
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

	uint stride = m_isTextured ? 8 * sizeof(float) : 6 * sizeof(float);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	if (m_isTextured) {
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}

	glBindVertexArray(0); // unbind vao first so it remembers buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	m_nIndices = iboData.size();
}

Model::~Model() {
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ibo);
}

ModelInstance::ModelInstance() :
	m_KdUniformLoc(-1),
	m_KsUniformLoc(-1),
	m_NsUniformLoc(-1),
	m_model(nullptr),
	m_M(1.0f) {}

ModelInstance::ModelInstance(Model* model) : 
	m_KdUniformLoc(-1), 
	m_KsUniformLoc(-1), 
	m_NsUniformLoc(-1),
	m_model(model),
	m_M(1.0f) {}

void ModelInstance::draw() const {
	assert(m_model);
	const std::vector<std::pair<uint, Material>>& mtls = m_model->getMtls();
	glBindVertexArray(m_model->getVao());
	for (uint i = 0; i < mtls.size(); ++i) {
		if (!m_model->isTextured()) {
			glUniform3fv(m_KdUniformLoc, 1, glm::value_ptr(mtls[i].second.Kd));
		}
		glUniform3fv(m_KsUniformLoc, 1, glm::value_ptr(mtls[i].second.Ks));
		glUniform1f(m_NsUniformLoc, mtls[i].second.Ns);
		if (i + 1 == mtls.size()) {
			glDrawElements(GL_TRIANGLES, m_model->getNumIndices() - mtls[i].first, GL_UNSIGNED_SHORT, (void*)(mtls[i].first * sizeof(ushort)));
		}
		else {
			glDrawElements(GL_TRIANGLES, mtls[i + 1].first - mtls[i].first, GL_UNSIGNED_SHORT, (void*)(mtls[i].first * sizeof(ushort)));
		}
	}
	glBindVertexArray(0);
}

void ModelInstance::setUniformLocations(GLint KdUniformLoc, GLint KsUniformLoc, GLint NsUniformLoc) {
	m_KdUniformLoc = KdUniformLoc;
	m_KsUniformLoc = KsUniformLoc;
	m_NsUniformLoc = NsUniformLoc;
}