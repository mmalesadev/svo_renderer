#include "Shader.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>

Shader::Shader()
{
	loaded = false;
}

bool Shader::loadShader(std::string file, int type)
{
	FILE * fp;
	fopen_s(&fp, file.c_str(), "rt");
	if (!fp)
	{
		printf("Cannot load shader file: %s", file.c_str());
		return false;
	}

	std::vector<std::string> lines;
	char line[255];
	while(fgets(line, 255, fp)) lines.push_back(line);
	fclose(fp);

	const char ** program = new const char * [(int)lines.size()];
	for(int i = 0; i < (int)lines.size(); ++i)
	{
		program[i] = lines[i].c_str();
	}

	shaderId = glCreateShader(type);

	// Passes source code of the shader for compilation to GPU
	glShaderSource(shaderId, (int)lines.size(), program, nullptr);

	// Compiles the shader's source code
	glCompileShader(shaderId);

	delete [] program;

	// Check compilation status
	int compilationStatus;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compilationStatus);
	
	if(compilationStatus == GL_FALSE) return false;
	this->type = type;
	loaded = true;

	return 1;
}

void Shader::deleteShader()
{
	if(!isLoaded()) return;
	loaded = false;
	glDeleteShader(shaderId);
}

bool Shader::isLoaded()
{
	return loaded;
}

GLuint Shader::getShaderID()
{
	return shaderId;
}

/****************************
ShaderProgram class functions
****************************/
ShaderProgram::ShaderProgram()
{
	linked = false;
}

void ShaderProgram::loadShaderProgram(std::string shaderName)
{
	Shader vertexShader, geometryShader, fragmentShader;// , commonShader;

	vertexShader.loadShader("data/shaders/" + shaderName + ".vert", GL_VERTEX_SHADER);
	geometryShader.loadShader("data/shaders/" + shaderName + ".geom", GL_GEOMETRY_SHADER);
	fragmentShader.loadShader("data/shaders/" + shaderName + ".frag", GL_FRAGMENT_SHADER);
	//commonShader.loadShader("data/shaders/common.frag", GL_FRAGMENT_SHADER);

	createProgram();
	addShaderToProgram(&vertexShader);
	addShaderToProgram(&geometryShader);
	addShaderToProgram(&fragmentShader);
	//addShaderToProgram(&commonShader);
	linkProgram();

	vertexShader.deleteShader();
	geometryShader.deleteShader();
	fragmentShader.deleteShader();
	//commonShader.deleteShader();
}

void ShaderProgram::createProgram()
{
	programId = glCreateProgram();
}

void ShaderProgram::deleteProgram()
{
	if(!linked) return;
	linked = false;
	glDeleteProgram(programId);
}

bool ShaderProgram::addShaderToProgram(Shader * shader)
{
	if(!shader->isLoaded()) return false;
	glAttachShader(programId, shader->getShaderID());
	return true;
}

bool ShaderProgram::linkProgram()
{
	glLinkProgram(programId);
	int linkStatus;
	glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);
	linked = linkStatus == GL_TRUE;
	return linked;
}

void ShaderProgram::useProgram()
{
	if(linked) glUseProgram(programId);
}

GLuint ShaderProgram::getProgramID()
{
	return programId;
}

// Setting floats

void ShaderProgram::setUniform(std::string sName, float* fValues, int iCount)
{
	int iLoc = glGetUniformLocation(programId, sName.c_str());
	glUniform1fv(iLoc, iCount, fValues);
}

void ShaderProgram::setUniform(std::string sName, const float fValue)
{
	int iLoc = glGetUniformLocation(programId, sName.c_str());
	glUniform1fv(iLoc, 1, &fValue);
}

// Setting vectors

void ShaderProgram::setUniform(std::string sName, glm::vec2* vVectors, int iCount)
{
	int iLoc = glGetUniformLocation(programId, sName.c_str());
	glUniform2fv(iLoc, iCount, (GLfloat*)vVectors);
}

void ShaderProgram::setUniform(std::string sName, const glm::vec2 vVector)
{
	int iLoc = glGetUniformLocation(programId, sName.c_str());
	glUniform2fv(iLoc, 1, (GLfloat*)&vVector);
}

void ShaderProgram::setUniform(std::string sName, glm::vec3* vVectors, int iCount)
{
	int iLoc = glGetUniformLocation(programId, sName.c_str());
	glUniform3fv(iLoc, iCount, (GLfloat*)vVectors);
}

void ShaderProgram::setUniform(std::string sName, const glm::vec3 vVector)
{
	int iLoc = glGetUniformLocation(programId, sName.c_str());
	glUniform3fv(iLoc, 1, (GLfloat*)&vVector);
}

void ShaderProgram::setUniform(std::string sName, glm::vec4* vVectors, int iCount)
{
	int iLoc = glGetUniformLocation(programId, sName.c_str());
	glUniform4fv(iLoc, iCount, (GLfloat*)vVectors);
}

void ShaderProgram::setUniform(std::string sName, const glm::vec4 vVector)
{
	int iLoc = glGetUniformLocation(programId, sName.c_str());
	glUniform4fv(iLoc, 1, (GLfloat*)&vVector);
}

// Setting 4x4 matrices

void ShaderProgram::setUniform(std::string sName, glm::mat4* mMatrices, int iCount)
{
	int iLoc = glGetUniformLocation(programId, sName.c_str());
	glUniformMatrix4fv(iLoc, iCount, false, (GLfloat*)mMatrices);
}

void ShaderProgram::setUniform(std::string sName, const glm::mat4 mMatrix)
{
	int iLoc = glGetUniformLocation(programId, sName.c_str());
	glUniformMatrix4fv(iLoc, 1, false, (GLfloat*)&mMatrix);
}

// Setting integers

void ShaderProgram::setUniform(std::string sName, int* iValues, int iCount)
{
	int iLoc = glGetUniformLocation(programId, sName.c_str());
	glUniform1iv(iLoc, iCount, iValues);
}

void ShaderProgram::setUniform(std::string sName, const int iValue)
{
	int iLoc = glGetUniformLocation(programId, sName.c_str());
	glUniform1i(iLoc, iValue);
}