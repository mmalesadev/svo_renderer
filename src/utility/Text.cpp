#include "Text.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Text::~Text()
{
	destroy();
}

void Text::init()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &vertexVBO);
	glGenBuffers(1, &uvVBO);

	texture.loadTexture2D("../data/textures/alphabetTexture.tga", true);
}

void Text::setTextValue(std::string textValue)
{
	this->textValue = textValue;
	calculateTextSize();
}

void Text::render(int posX, int posY, int size)
{
	unsigned int textLength = static_cast<unsigned int>(textValue.length());

	std::vector<glm::vec2> vertices;
	std::vector<glm::vec2> UVs;

	int lineNo = 1;
	int lineCharacterNo = 0;
	for (unsigned int i = 0; i < textLength; ++i)
	{
		char character = textValue[i];
		// MAKING THE "/" SIGN THE NEW LINE SIGN
		if (character == 47)
		{
			++lineNo;
			lineCharacterNo = 0;
			continue;
		}

		glm::vec2 vertexUpLeft = glm::vec2(posX + lineCharacterNo*size, posY - lineNo*size + size);
		glm::vec2 vertexUpRight = glm::vec2(posX + lineCharacterNo*size + size, posY - lineNo*size + size);
		glm::vec2 vertexDownRight = glm::vec2(posX + lineCharacterNo*size + size, posY - lineNo*size);
		glm::vec2 vertexDownLeft = glm::vec2(posX + lineCharacterNo*size, posY - lineNo*size);

		vertices.push_back(vertexUpLeft);
		vertices.push_back(vertexDownLeft);
		vertices.push_back(vertexUpRight);

		vertices.push_back(vertexDownRight);
		vertices.push_back(vertexUpRight);
		vertices.push_back(vertexDownLeft);

		float uvX = (character % 16) / 16.0f;
		float uvY = 1.0f - ((character / 16) / 16.0f);

		glm::vec2 uvUpLeft = glm::vec2(uvX, uvY);
		glm::vec2 uvUpRight = glm::vec2(uvX + 1.0f / 16.0f, uvY);
		glm::vec2 uvDownRight = glm::vec2(uvX + 1.0f / 16.0f, uvY - 1.0f / 16.0f);
		glm::vec2 uvDownLeft = glm::vec2(uvX, uvY - 1.0f / 16.0f);

		UVs.push_back(uvUpLeft);
		UVs.push_back(uvDownLeft);
		UVs.push_back(uvUpRight);

		UVs.push_back(uvDownRight);
		UVs.push_back(uvUpRight);
		UVs.push_back(uvDownLeft);

		++lineCharacterNo;
	}

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
	glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	texture.bindTexture();

	glBindVertexArray(VAO);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
	glDisable(GL_BLEND);
}

void Text::update(float deltaTime)
{
}

void Text::destroy()
{
	texture.releaseTexture();
}

void Text::calculateTextSize()
{
	unsigned int textLength = static_cast<unsigned int>(textValue.length());
	unsigned int lineCharacterNo = 0;
	int lineNo = 1;
	for (unsigned int i = 0; i < textLength; ++i)
	{
		char character = textValue[i];
		if (character == 47)
		{
			if (lineCharacterNo > textWidthInChars) textWidthInChars = lineCharacterNo;
			lineCharacterNo = 0;
			++lineNo;
			continue;
		}
		++lineCharacterNo;
	}
	numberOfLines = lineNo;
}
