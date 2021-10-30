#include "Shader.h"


/* Gibt Shader-Daten wieder frei */ 
void freeShader(GLuint shaderId) {
	glDeleteProgram(shaderId);
}

/* Shader verwenden */ 
void bindShader(GLuint shaderId) {
	glUseProgram(shaderId);
}

/* Shader nicht verwenden */ 
void unbindShader() {
	glUseProgram(0);
}

/* Compiles shader source code; Exits on compile error */
GLuint compile(const char*shaderSource, GLenum type) {
	GLuint id = glCreateShader(type);
	glShaderSource(id, 1, &shaderSource, 0);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	
	if (result != GL_TRUE) {
		int length = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = malloc(length);
		glGetShaderInfoLog(id, length, &length, message); 
		printf("Shader compile error: %s\n", message);
		free(message);
		exit(0);
	}
	
	return id;
}

/* Reads shader source code */
char* parse(const char* filename) {
	FILE* file;
	file = fopen(filename, "rb");  // unter Windows fopen_s
	if (file == NULL) {
		printf("Err: File %s not found\n", filename);
		exit(0);
		
	}

	fseek(file, 0, SEEK_END);
	size_t filesize = ftell(file);
	rewind(file);
	char * contents = malloc(filesize);
	fread(contents, 1, filesize, file);
	fclose(file);
	return contents;
}

/* returns shaderID */
GLuint createShader(const char* vertexShaderFilename, const char* fragmentShaderFilename) {
	char * vertexShaderSource = parse(vertexShaderFilename);
	char * fragmentShaderSource = parse(fragmentShaderFilename);

	GLuint program = glCreateProgram();
	GLuint vs = compile(vertexShaderSource, GL_VERTEX_SHADER);
	GLuint fs = compile(fragmentShaderSource, GL_FRAGMENT_SHADER);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);

	return program;
}