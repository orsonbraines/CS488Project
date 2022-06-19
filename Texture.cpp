#include <fstream>
#include <cstring>
#include <iostream>

#include "Texture.h"
#include "Util.h"

Texture::Texture(const std::string& texFilePath) : m_texId(0) {
	std::ifstream in(texFilePath, std::ios_base::binary);
	if (!in) {
		throw GraphicsException("Unable to open: " + texFilePath);
	}
	char magic[4];
	in.read(magic, 4);
	if (!in || strncmp(magic, "DDS ", 4) != 0) {
		throw GraphicsException("Bad magic number");
	}

	i32 header[31];
	in.read((char*)header, sizeof(header));
	int height = header[2];
	int width = header[3];
	int mips = header[6];
	int pixelFormatFlags = header[19];
	int fourCC = header[20];

	DBG(std::cerr << "(h,w,mips) " << height << "," << width << "," << mips << std::endl);
	DBG(std::cerr << "pixelFormatFlags " << pixelFormatFlags << std::endl);

	u32 textureFormat = 0;
	if (pixelFormatFlags & 4) {
		if (strncmp((char*)&fourCC, "DXT1", 4) == 0) {
			textureFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		}
		else if (strncmp((char*)&fourCC, "DXT3", 4) == 0) {
			DBG(std::cerr << "found DXT3 encoded texture" << std::endl);
			textureFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		}
		else if (strncmp((char*)&fourCC, "DXT5", 4) == 0) {
			textureFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		}
	}

	if (!textureFormat) {
		throw GraphicsException("Texture format not supported");
	}

	

	glGenTextures(1, &m_texId);
	glBindTexture(GL_TEXTURE_2D, m_texId);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	uint w = width;
	uint h = height;
	uint size = w * h;
	if (textureFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) {
		size /= 2;
	}
	char* buf = new char[size];

	for (int i = 0; i < mips; ++i) {
		DBG(std::cerr << w << " " << h << std::endl);
		in.read(buf, size);
		if (i == mips-1) {
			std::ofstream o("texout", std::ios_base::binary);
			o.write(buf, size);
		}
		glCompressedTexImage2D(GL_TEXTURE_2D, i, textureFormat, w, h, 0, size, buf);
		// this simplified math will only work if all sizes are powers of 2 at least 4
		w >>= 1;
		h >>= 1;
		size >>= 2;
	}

	delete[] buf;

	//try out a white texture
	//i32 vals[4] = {-1, -1, -1, 0};
	//glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, 1, 1, 0, 16, &vals);

	if (!in) {
		throw GraphicsException("Error reading in compressed texture data");
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	// test a dummy texture
	//glGenTextures(1, &m_dummyId);
	//glBindTexture(GL_TEXTURE_2D, m_dummyId);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//unsigned char col[12] = { 255,0,0, 0,255,0, 0,0,255, 255,255,255 };
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, col);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//glBindTexture(GL_TEXTURE_2D, 0);

}

Texture::~Texture() {
	glDeleteTextures(1, &m_texId);
}

void Texture::bind() {
	glBindTexture(GL_TEXTURE_2D, m_texId);
}