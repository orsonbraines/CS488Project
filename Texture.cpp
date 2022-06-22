#include <fstream>
#include <cassert>
#include <cstring>
#include <iostream>

#include "Texture.h"
#include "Util.h"

Texture::Texture(const std::string& texFilePath) : m_texId(0) {
	size_t dot = texFilePath.find('.');
	assert(dot != std::string::npos);
	std::string ext = texFilePath.substr(dot + 1);
	if (ext == "dds") {
		loadDDS(texFilePath);
	}
	else if (ext == "bmp") {
		loadBMP(texFilePath);
	}
	else {
		throw GraphicsException("Unrecognized texture file extension: " + ext);
	}
}

void Texture::loadBMP(const std::string& texFilePath) {
	std::ifstream in(texFilePath, std::ios_base::binary);
	if (!in) {
		throw GraphicsException("Unable to open: " + texFilePath);
	}

	ushort hdr[7];
	in.read((char*)hdr, 14);
	if (!in || hdr[0] != 0x4d42) {
		throw GraphicsException("Bad magic number");
	}
	uint size = hdr[1] + hdr[2] * (1u << 16);
	uint pixelOffset = hdr[5] + hdr[6] * (1u << 16);

	char* buf = new char[size];
	memcpy(buf, hdr, 14);
	in.read((char*)buf + 14, size - 14);
	if (!in) {
		delete[] buf;
		throw GraphicsException("error reading bmp");
	}

	uint hdr2Size = *(u32*)(buf + 14);
	assert(hdr2Size == 40);
	uint width = *(ushort*)(buf + 18);
	uint height = *(ushort*)(buf + 22);
	uint bitsPerPixel = *(ushort*)(buf + 28);
	uint compressionMethod = *(ushort*)(buf + 30);
	assert(compressionMethod == 0);

	DBG(std::cerr << "size " << size << " offset " << pixelOffset << " width " << width << " height " << height << std::endl);

	// greyScale
	if (bitsPerPixel != 8) {
		delete[] buf;
		throw GraphicsException("for now only greyscale images are supported");
	}

	glGenTextures(1, &m_texId);
	glBindTexture(GL_TEXTURE_2D, m_texId);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, buf + pixelOffset);

	delete[] buf;
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::loadDDS(const std::string& texFilePath) {
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
	int linearSize = header[4];
	int mips = header[6];
	int pixelFormatFlags = header[19];
	int fourCC = header[20];

	DBG(std::cerr << "(h,w,mips) " << height << "," << width << "," << mips << std::endl);
	DBG(std::cerr << "dwPitchOrLinearSize " << linearSize << std::endl);
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mips - 1);

	uint w = width;
	uint h = height;
	uint blockSize = textureFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ? 8 : 16;
	uint size = w * h / 16 * blockSize;
	assert(size == linearSize);
	assert(w == h);
	assert(w > 0 && h > 0);
	assert(((w - 1) & w) == 0);
	assert(((h - 1) & h) == 0);
	char* buf = new char[size];

	for (int i = 0; i < mips; ++i) {
		DBG(std::cerr << w << " " << h << std::endl);
		in.read(buf, size);
		glCompressedTexImage2D(GL_TEXTURE_2D, i, textureFormat, w, h, 0, size, buf);
		// this simplified math will only work if all sizes are powers of 2 and at least 4 texels in both dimensions
		w >>= 1;
		h >>= 1;
		if (size > blockSize) {
			size >>= 2;
		}
	}

	delete[] buf;

	if (!in) {
		throw GraphicsException("Error reading in compressed texture data");
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture() {
	glDeleteTextures(1, &m_texId);
}

void Texture::bind() {
	glBindTexture(GL_TEXTURE_2D, m_texId);
}