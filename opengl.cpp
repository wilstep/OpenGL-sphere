// OpenGL Involute gear simulation
// Stephen R Williams, Feb 2019
// License: GPL-3.0

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <array>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

#include "opengl.hpp"

static const GLfloat pi = 3.1415926535897932f;
static GLuint shaderProgram;
static GLint uniColor;
static GLuint vao, vbo, ebo;


// Put shaders in files: vertex.shader, fragment.shader
static void shaders()
{
    std::string vertexCode, fragmentCode;
    // First read source code from files into the two strings declared directly above
    {
        std::ifstream fin;
        std::stringstream vShaderStream, fShaderStream;
        
        fin.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        // vertex shader first
        fin.open("vertex.shader");
        vShaderStream << fin.rdbuf();
        fin.close();
        vertexCode = vShaderStream.str();
        // fragment shader
        fin.open("fragment.shader");
        fShaderStream << fin.rdbuf();
        fin.close();
        fragmentCode = fShaderStream.str();
    }
    // Now compile and link the shaders
    {
        // Create and compile the vertex shader
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        const char *vCode = vertexCode.c_str();
        glShaderSource(vertexShader, 1, &vCode, NULL);
        glCompileShader(vertexShader);
        int success;
        char infoLog[1024];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if(!success){
            glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
            std::string str = "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" + std::string(infoLog);
            throw std::runtime_error(str);
        }   
        // Create and compile the fragment shader
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        const char *fCode = fragmentCode.c_str();
        glShaderSource(fragmentShader, 1, &fCode, NULL);
        glCompileShader(fragmentShader);
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if(!success){
            glGetShaderInfoLog(fragmentShader, 1024, NULL, infoLog);
            std::string str = "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" + std::string(infoLog);
            throw std::runtime_error(str);
        }     
        // Link the vertex and fragment shader into a shader program
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glBindFragDataLocation(shaderProgram, 0, "outColor");
        glLinkProgram(shaderProgram);
        glDeleteShader(fragmentShader);
        glDeleteShader(vertexShader);
        glUseProgram(shaderProgram);
    }
}

static std::vector<GLfloat> pvec(16);

// ar is the screen's aspect ratio, width / height
std::vector<GLfloat>& oglWrap::perspective(GLfloat theta, GLfloat ar, GLfloat zn, GLfloat zf)
{
    theta = theta * pi / 360.0f; // half angle in radians
    GLfloat tanx = tan(theta);
    
    for(auto &a: pvec) a = 0.0f; 
    pvec[0] = 1.0f / (tanx * ar);
    pvec[5] = 1.0f / tanx;
    pvec[10] = -(zf + zn) / (zf - zn);
    pvec[11] = -2.0f * zf * zn / (zf - zn);
    pvec[14] = -1.0;
    return pvec;
}

static std::vector<GLfloat> rvecY(16);

std::vector<GLfloat>& oglWrap::rotateY(GLfloat theta)
{
    theta = theta * pi / 180.0f; // angle in radians
    
    GLfloat sinx = sin(theta), cosx = cos(theta);
    for(auto &a: rvecY) a = 0.0f; 
    rvecY[0] = cosx;
    rvecY[2] = sinx;
    rvecY[5] = 1.0f;
    rvecY[8] = -sinx;
    rvecY[10] = cosx;
    rvecY[15] = 1.0f;
    return rvecY;
}

static std::vector<GLfloat> rvecZ(16);

std::vector<GLfloat>& oglWrap::rotateZ(GLfloat theta)
{
    theta = theta * pi / 180.0f; // angle in radians
    
    GLfloat sinx = sin(theta), cosx = cos(theta);
    for(auto &a: rvecZ) a = 0.0f; 
    rvecZ[0] = cosx;
    rvecZ[1] = -sinx;
    rvecZ[4] = sinx;
    rvecZ[5] = cosx;
    rvecZ[10] = 1.0f;
    rvecZ[15] = 1.0f;
    return rvecZ;
}


void oglWrap::info()
{
    std::cout << "\nOpenGL core profile version string: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "OpenGL vendor string: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "OpenGL renderer string: " << glGetString(GL_RENDERER) << std::endl;
}

void oglWrap::setUp()
{
    shaders();
    uniColor = glGetUniformLocation(shaderProgram, "triangleColor");
}

void oglWrap::close()
{
    glDeleteProgram(shaderProgram);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
}

void oglWrap::createBuff(std::vector<GLfloat> &vertices, std::vector<GLuint> &indices)
{
    // Create Vertex Array Object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create a Vertex Buffer Object and copy the vertex data to it
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo); // and an element buffer object
    
    // vertex data
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
   
    // index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // element buffer object
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    // Specify the layout of the vertex data
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
}


// n: number of indices = 3 * number of triangles
// offset: number of indices to offset by
void oglWrap::draw(GLuint n, GLuint offset)
{
    glDrawElements(GL_TRIANGLES, n, GL_UNSIGNED_INT, (void*)(offset * sizeof(GLuint)));
}

// Wrapper functions to set the uniforms
void oglWrap::setFloat(const std::string &name, GLfloat value)
{
    glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value); 
}

void oglWrap::setVec3(const std::string &name, GLfloat data[]) 
{ 
    GLint uni = glGetUniformLocation(shaderProgram, name.c_str());
    glUniform3fv(uni, 1, data);
}

void oglWrap::setMat4(const std::string &name, GLfloat data[])
{
    GLint uni = glGetUniformLocation(shaderProgram, name.c_str());
    glUniformMatrix4fv(uni, 1, GL_TRUE, data); // transpose is set to true
}

void oglWrap::setColor(GLfloat x, GLfloat y, GLfloat z) 
{  
    glUniform3f(uniColor, x, y, z); 
}



