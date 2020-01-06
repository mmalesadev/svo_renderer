#include "Texture.h"
#include <FreeImage.h>
#include <iostream>
#include <vector>

std::vector<Texture> Texture::loadedTextures;

Texture::Texture()
{
	mipMapsGenerated = false;
}


Texture::~Texture()
{
}

bool Texture::loadTexture2D(std::string path, bool generateMipMaps)
{
	for (GLuint i = 0; i < loadedTextures.size(); ++i)
	{
		if (loadedTextures[i].path == path)
		{
			*this = loadedTextures[i];
			return true;
		}
	}

	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP * dib;

	fif = FreeImage_GetFileType(path.c_str(), 0);	// Check the file signature and deduce its format

	if (fif == FIF_UNKNOWN) // If still unknown, try to guess from the file extension
		fif = FreeImage_GetFIFFromFilename(path.c_str());

	if (fif == FIF_UNKNOWN) // If still unknown, returns failure
		return false;

	if (FreeImage_FIFSupportsReading(fif))	// Check if the plugin has reading capabilities and load the file
		dib = FreeImage_Load(fif, path.c_str());
	if (!dib)
	{
		return false;
	}
	
	BYTE * dataPointer = FreeImage_GetBits(dib); // Retrieve the image data

	width = FreeImage_GetWidth(dib);
	height = FreeImage_GetHeight(dib);
	BPP = FreeImage_GetBPP(dib);

	unsigned int tp = FreeImage_GetTransparencyCount(dib);

	// If somehow it failed, return failure
	if (dataPointer == nullptr || width == 0 || height == 0)
	{
		FreeImage_Unload(dib);
		return false;
	}

	// Generate an OpenGL texture ID for this texture
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	int format = BPP == 32 ? GL_BGRA : BPP == 24 ? GL_BGR : BPP == 8 ? GL_LUMINANCE : 0;
	int internalFormat = BPP == 32 ? GL_RGBA : BPP == 24 ? GL_RGB : GL_DEPTH_COMPONENT;
	
	// Printing loaded texture
	//std::cout << path << " " << BPP << " " << format << " " << internalFormat << " " << tp << std::endl;

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, dataPointer);

	if(generateMipMaps)
		glGenerateMipmap(GL_TEXTURE_2D);

	glGenSamplers(1, &sampler);

	this->path = path;
	mipMapsGenerated = generateMipMaps;

	FreeImage_Unload(dib);

	loadedTextures.push_back(*this);

	return true;
}

void Texture::bindTexture(int textureUnit)
{
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindSampler(textureUnit, sampler);
}

void Texture::setSamplerParameter(GLenum parameter, GLenum value)
{
	glSamplerParameteri(sampler, parameter, value);
}

void Texture::setFiltering(int magnification, int minification)
{
	// Set magnification filter
	if(magnification == TEXTURE_FILTER_MAG_NEAREST)
		glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	else if(magnification == TEXTURE_FILTER_MAG_BILINEAR)
		glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Set minification filter
	if(minification == TEXTURE_FILTER_MIN_NEAREST) 
		glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	else if(minification == TEXTURE_FILTER_MIN_BILINEAR) 
		glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	else if(minification == TEXTURE_FILTER_MIN_NEAREST_MIPMAP) 
		glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); 
	else if(minification == TEXTURE_FILTER_MIN_BILINEAR_MIPMAP) 
		glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); 
	else if(minification == TEXTURE_FILTER_MIN_TRILINEAR) 
		glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	this->minification = minification;
	this->magnification = magnification;
}

int Texture::getMinificationFilter()
{
	return minification;
}

int Texture::getMagnificationFilter()
{
	return magnification;
}

void Texture::releaseTexture()
{
	glDeleteSamplers(1, &sampler);
	glDeleteTextures(1, &texture);
}