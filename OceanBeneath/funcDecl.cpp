#include "funcDecl.h"

void Mesh::LoadRawFloatData(std::string path)
{
	auto data = ::LoadRawFloatData(path);
	VAO = CreateMeshVAO(data);
	primitiveCount = size(data) / 8;
}

void Mesh::Draw(GLuint shader)
{
	auto modelLoc = glGetUniformLocation(shader, "model");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, primitiveCount);
	glBindVertexArray(0);

}

