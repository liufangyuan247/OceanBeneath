#include "funcDecl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint CreateShaderProgram(const char * vertSrc, const char * fragSrc)
{
	GLuint shaderProgram = glCreateProgram();
	GLuint vertShader = CompileShader(GL_VERTEX_SHADER, vertSrc);
	GLuint fragShader = CompileShader(GL_FRAGMENT_SHADER, fragSrc);


	glAttachShader(shaderProgram, vertShader);
	glAttachShader(shaderProgram, fragShader);

	glLinkProgram(shaderProgram);

	GLint success = 0;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		GLint logLen = 0;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLen);
		vector<char> buffer(logLen + 1);
		glGetProgramInfoLog(shaderProgram, logLen + 1, &logLen, (char *)&buffer[0]);
		cout << "shader program link error:" << (char *)&buffer[0] << endl;
	}

	glDetachShader(shaderProgram, vertShader);
	glDetachShader(shaderProgram, fragShader);


	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	return shaderProgram;
}

GLuint CreateShaderProgram(const char * vertSrc, const char * geomSrc, const char * fragSrc)
{
	GLuint shaderProgram = glCreateProgram();
	GLuint vertShader = CompileShader(GL_VERTEX_SHADER, vertSrc);
	GLuint geomShader = CompileShader(GL_GEOMETRY_SHADER, geomSrc);
	GLuint fragShader = CompileShader(GL_FRAGMENT_SHADER, fragSrc);


	glAttachShader(shaderProgram, vertShader);
	glAttachShader(shaderProgram, geomShader);
	glAttachShader(shaderProgram, fragShader);

	glLinkProgram(shaderProgram);

	GLint success = 0;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		GLint logLen = 0;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLen);
		vector<char> buffer(logLen + 1);
		glGetProgramInfoLog(shaderProgram, logLen + 1, &logLen, (char *)&buffer[0]);
		cout << "shader program link error:" << (char *)&buffer[0] << endl;
	}

	glDetachShader(shaderProgram, vertShader);
	glDetachShader(shaderProgram, geomShader);
	glDetachShader(shaderProgram, fragShader);


	glDeleteShader(vertShader);
	glDeleteShader(geomShader);
	glDeleteShader(fragShader);

	return shaderProgram;
}

GLuint CreateShaderProgramFromFile(const char * vertPath, const char * fragPath)
{
	GLuint shaderProgram = glCreateProgram();
	GLuint vertShader = CompileShaderFromFile(GL_VERTEX_SHADER, vertPath);
	GLuint fragShader = CompileShaderFromFile(GL_FRAGMENT_SHADER, fragPath);


	glAttachShader(shaderProgram, vertShader);
	glAttachShader(shaderProgram, fragShader);

	glLinkProgram(shaderProgram);

	GLint success = 0;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		GLint logLen = 0;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLen);
		vector<char> buffer(logLen + 1);
		glGetProgramInfoLog(shaderProgram, logLen + 1, &logLen, (char *)&buffer[0]);
		cout << "shader program link error:" << (char *)&buffer[0] << endl;
	}

	glDetachShader(shaderProgram, vertShader);
	glDetachShader(shaderProgram, fragShader);


	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	return shaderProgram;
}

GLuint CreateShaderProgramFromFile(const char * vertPath, const char * geomPath, const char * fragPath)
{
	GLuint shaderProgram = glCreateProgram();
	GLuint vertShader = CompileShaderFromFile(GL_VERTEX_SHADER, vertPath);
	GLuint geomShader = CompileShaderFromFile(GL_GEOMETRY_SHADER, geomPath);
	GLuint fragShader = CompileShaderFromFile(GL_FRAGMENT_SHADER, fragPath);


	glAttachShader(shaderProgram, vertShader);
	glAttachShader(shaderProgram, geomShader);
	glAttachShader(shaderProgram, fragShader);

	glLinkProgram(shaderProgram);

	GLint success = 0;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		GLint logLen = 0;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLen);
		vector<char> buffer(logLen + 1);
		glGetProgramInfoLog(shaderProgram, logLen + 1, &logLen, (char *)&buffer[0]);
		cout << "shader program link error:" << (char *)&buffer[0] << endl;
	}

	glDetachShader(shaderProgram, vertShader);
	glDetachShader(shaderProgram, geomShader);
	glDetachShader(shaderProgram, fragShader);


	glDeleteShader(vertShader);
	glDeleteShader(geomShader);
	glDeleteShader(fragShader);

	return shaderProgram;
}

GLuint CompileShader(GLenum shaderType, const char * src)
{
	GLuint geometryShader = glCreateShader(shaderType);
	GLint len = 1;

	glShaderSource(geometryShader, 1, &src, 0);
	glCompileShader(geometryShader);

	GLint success = 0;
	glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLint logLen = 0;
		glGetShaderiv(geometryShader, GL_INFO_LOG_LENGTH, &logLen);
		vector<char> buffer(logLen + 1);
		glGetShaderInfoLog(geometryShader, logLen + 1, &logLen, (char *)&buffer[0]);
		cout << "shader complie error:" << (char *)&buffer[0] << endl;
	}

	return geometryShader;
}

GLuint CompileShaderFromFile(GLenum shaderType, const char * path)
{
	string line;
	string src;
	ifstream ifs(path);
	while (getline(ifs,line))
	{
		src = src + "\n" + line;
	}
	return CompileShader(shaderType, src.c_str());
}

vector<float> LoadRawFloatData(string path)
{
	vector<float> data;
	ifstream ifs(path);
	float v;
	string line;
	while (ifs >> v)
	{
		data.push_back(v);
	}
	data.resize(data.size() / 8 * 8);
	return data;
}

GLuint LoadTexture(string path)
{
	GLuint tex = 0;
	glGenTextures(1, &tex);
	stbi_uc *data = 0;
	int w, h;
	int channel = 4;
	data = stbi_load(path.c_str(), &w, &h, &channel, 4);
	if (data)
	{
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glGenerateMipmap(GL_TEXTURE_2D);
		free(data);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	return tex;
}

GLuint LoadTextureF(std::string path)
{
	GLuint tex = 0;
	glGenTextures(1, &tex);
	float *data = 0;
	int w, h;
	int channel = 4;
	data = stbi_loadf(path.c_str(), &w, &h, &channel, 4);
	if (data)
	{
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glGenerateMipmap(GL_TEXTURE_2D);
		free(data);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	return tex;
}

GLuint LoadTexture(vector<std::string> cubePath)
{
	GLuint tex = 0;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);

	for (int i=0;i<cubePath.size();++i)
	{

		stbi_uc *data = 0;
		int w, h;
		int channel = 4;
		data = stbi_load(cubePath[i].c_str(), &w, &h, &channel, 4);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			free(data);
		}
	}
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	return tex;
}

void SetUniform(GLuint shaderProgram, const char * name, float value)
{
	auto loc = glGetUniformLocation(shaderProgram, name);
	if (loc==-1)
	{
		cout << "uniform " << name << " not exist!" << endl;
	}
	glUniform1f(loc, value);
}

void SetUniform(GLuint shaderProgram, const char * name, vec3 value)
{
	auto loc = glGetUniformLocation(shaderProgram, name);
	if (loc == -1)
	{
		cout << "uniform " << name << " not exist!" << endl;
	}
	glUniform3fv(loc,1,value_ptr(value));
}

void SetUniform(GLuint shaderProgram, const char * name, mat4 value)
{
	auto loc = glGetUniformLocation(shaderProgram, name);
	if (loc == -1)
	{
		cout << "uniform " << name << " not exist!" << endl;
	}
	glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(value));
}

void SetUniform(GLuint shaderProgram, const char * name, mat3 value)
{
	auto loc = glGetUniformLocation(shaderProgram, name);
	if (loc == -1)
	{
		cout << "uniform " << name << " not exist!" << endl;
	}
	glUniformMatrix3fv(loc, 1, GL_FALSE, value_ptr(value));
}

void SetUniform(GLuint shaderProgram, const char * name, int value)
{
	auto loc = glGetUniformLocation(shaderProgram, name);
	if (loc == -1)
	{
		cout << "uniform " << name << " not exist!" << endl;
	}
	glUniform1i(loc, value);
}

GLuint CreateMeshVAO(vector<float> & data)
{
	GLuint vao;
	GLuint vbo;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*data.size(), &data[0], GL_STATIC_DRAW);

	int index = 0;
	auto *layout = VertexAttribute::AttributeLayout();
	while (layout->size)
	{
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, layout->size, layout->type, layout->normalized, sizeof(VertexAttribute), layout->offset);
		index++;
		layout++;
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return vao;
}


