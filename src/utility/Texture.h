#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>

enum textureFiltering
{
	TEXTURE_FILTER_MAG_NEAREST = 0, // Nearest criterion for magnification
	TEXTURE_FILTER_MAG_BILINEAR, // Bilinear criterion for magnification
	TEXTURE_FILTER_MAG_ANISOTROPIC,
	TEXTURE_FILTER_MIN_NEAREST, // Nearest criterion for minification
	TEXTURE_FILTER_MIN_BILINEAR, // Bilinear criterion for minification
	TEXTURE_FILTER_MIN_NEAREST_MIPMAP, // Nearest criterion for minification, but on closest mipmap
	TEXTURE_FILTER_MIN_BILINEAR_MIPMAP, // Bilinear criterion for minification, but on closest mipmap
	TEXTURE_FILTER_MIN_TRILINEAR, // Bilinear criterion for minification on two closest mipmaps, then averaged
	TEXTURE_FILTER_ANISOTROPIC,
};

/*
Class wrapping texture
*/
class Texture
{
private:
	int width, height, BPP; // texture width, height, bytes per pixel
	GLuint sampler;	// sampler name
	bool mipMapsGenerated;

	int minification, magnification;

public:
	static std::vector<Texture> loadedTextures;

	std::string path;
	std::string type;
	GLuint texture;	// texture ID

	Texture();
	~Texture();

	bool loadTexture2D(std::string path, bool generateMipMaps = false);
	void bindTexture(int textureUnit = 0);

	void setSamplerParameter(GLenum parameter, GLenum value);
	void setFiltering(int magnification, int minification);

	int getMinificationFilter();
	int getMagnificationFilter();

	void releaseTexture();
};

#endif
