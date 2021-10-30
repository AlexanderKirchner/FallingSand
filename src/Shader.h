#pragma once

#include "Defines.h"

#include <GL/glew.h>

void freeShader(GLuint shaderId);

void bindShader(GLuint shaderId);

void unbindShader(); 

GLuint compile(const char*shaderSource, GLenum type); 

char* parse(const char* filename);

GLuint createShader(const char* vertexShaderFilename, const char* fragmentShaderFilename);