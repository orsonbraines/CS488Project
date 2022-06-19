#include <fstream>
#include <cstring>

#include "Texture.h"
#include "Util.h"

Texture::Texture(const std::string& texFilePath) : m_texId(0) {
	std::ifstream in(texFilePath, std::ios_base::binary);
	if (!in) {
		throw GraphicsException("Unable to open: " + texFilePath);
	}
	char magic[5];
	in.read(magic, 4);
	magic[4] = '\0';
	if (!in || strcmp(magic, "DDS ") != 0) {
		throw GraphicsException("Bad magic number");
	}

	glGenTextures(1, &m_texId);
}