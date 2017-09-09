#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

/*
Wraps OpenGL shader loading and compiling
*/
class Shader
{
private:
    GLuint shaderId;    // Shader's ID
    int type;            // GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, etc.
    bool loaded;        // Whether was the shader loaded and compiled

public:
    Shader();

    bool loadShader(std::string file, int type);
    void deleteShader();

    bool isLoaded();
    GLuint getShaderID();
};

/*
Wraps shader programs for easy usage
*/
class ShaderProgram
{
private:
    GLuint programId; // ID of program
    bool linked; // Whether program was linked and is ready to use

public:
    ShaderProgram();

    void loadShaderProgram(std::string shaderName);

    void createProgram();
    void deleteProgram();

    bool addShaderToProgram(Shader * shader);
    bool linkProgram();

    void useProgram();

    GLuint getProgramID();

    // Setting vectors
    void setUniform(std::string sName, glm::vec2* vVectors, int iCount = 1);
    void setUniform(std::string sName, const glm::vec2 vVector);
    void setUniform(std::string sName, glm::vec3* vVectors, int iCount = 1);
    void setUniform(std::string sName, const glm::vec3 vVector);
    void setUniform(std::string sName, glm::vec4* vVectors, int iCount = 1);
    void setUniform(std::string sName, const glm::vec4 vVector);

    // Setting floats
    void setUniform(std::string sName, float* fValues, int iCount = 1);
    void setUniform(std::string sName, const float fValue);

    // Setting 4x4 matrices
    void setUniform(std::string sName, glm::mat4* mMatrices, int iCount = 1);
    void setUniform(std::string sName, const glm::mat4 mMatrix);

    // Setting integers
    void setUniform(std::string sName, int* iValues, int iCount = 1);
    void setUniform(std::string sName, const int iValue);
};