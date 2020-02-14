#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#define GLEW_STATIC
#include <gl/glew.h>
#include <GLFW/glfw3.h>

using namespace std;
using namespace glm;

GLuint CreateShaderProgram(const char *vertSrc, const char *fragSrc);
GLuint CreateShaderProgram(const char *vertSrc, const char *geomSrc, const char *fragSrc);
GLuint CreateShaderProgramFromFile(const char *vertPath, const char *fragPath);
GLuint CreateShaderProgramFromFile(const char *vertPath, const char *geomPath, const char *fragPath);
GLuint CompileShader(GLenum shaderType, const char *src);
GLuint CompileShaderFromFile(GLenum shaderType, const char *path);
GLuint CreateMeshVAO(std::vector<float> & data);
void Update(float dt);
std::vector<float> LoadRawFloatData(std::string path);
GLuint LoadTexture(std::string path);
GLuint LoadTextureF(std::string path);
GLuint LoadTexture(vector<std::string> cubePath);


void SetUniform(GLuint shaderProgram, const char *name, float value);
void SetUniform(GLuint shaderProgram, const char *name, vec3 value);
void SetUniform(GLuint shaderProgram, const char *name, mat4 value);
void SetUniform(GLuint shaderProgram, const char *name, mat3 value);
void SetUniform(GLuint shaderProgram, const char *name, int value);


struct Mesh
{
	mat4 model=mat4(1);
	GLuint VAO=0;
	int primitiveCount=0;
	void LoadRawFloatData(std::string path);
	void Draw(GLuint shader);
};

struct VertexAttributeLayout
{
	GLuint size;
	GLenum type;
	GLboolean normalized;
	void *offset;
};

struct VertexAttribute
{
	vec3 position;
	vec3 normal;
	vec2 texCoord;

	static const VertexAttributeLayout* AttributeLayout()
	{
		static const VertexAttributeLayout layouts[] = {
			{ 3,GL_FLOAT,GL_FALSE,0 },
			{ 3,GL_FLOAT,GL_FALSE,(void *)(sizeof(vec3)) },
			{ 2,GL_FLOAT,GL_FALSE,(void *)(sizeof(vec3) + sizeof(vec3)) },
			{ 0,GL_FLOAT,GL_FALSE,0 },
		};
		return layouts;
	}
};

struct Texture
{
	GLuint id;
	GLenum internalFormat;
	int width, height;

	explicit Texture(GLenum internalFormat,int w,int h):
		internalFormat(internalFormat), width(w),height(h)
	{
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D,id);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, internalFormat==GL_DEPTH24_STENCIL8? GL_DEPTH_STENCIL :GL_RGBA, internalFormat == GL_DEPTH24_STENCIL8 ? GL_UNSIGNED_INT_24_8 :GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Resize(int w,int h)
	{
		if ((w!=width||h != height)&&w&&h)
		{
			width = w;
			height = h;
			glBindTexture(GL_TEXTURE_2D, id);
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, internalFormat == GL_DEPTH24_STENCIL8 ? GL_DEPTH_STENCIL : GL_RGBA, internalFormat == GL_DEPTH24_STENCIL8 ? GL_UNSIGNED_INT_24_8 : GL_UNSIGNED_BYTE, 0);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	void Bind(int index)
	{
		glActiveTexture(GL_TEXTURE0 + index);
		glBindTexture(GL_TEXTURE_2D, id);
	}
};


static void error_callback(int error, const char* description)
{
	cerr << "Error:" << description << endl;
}
