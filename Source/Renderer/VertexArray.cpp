#include "VertexArray.h"
#include <GL/glew.h>

VertexArray::VertexArray(const float* verts, unsigned int numVerts, const unsigned int* indices,
						 unsigned int numIndices)
: mNumVerts(numVerts)
, mNumIndices(numIndices)
, mVertexBuffer(0)
, mIndexBuffer(0)
, mVertexArray(0)
{
	//VAO
	glGenVertexArrays(1, &mVertexArray);
	glBindVertexArray(mVertexArray);

	//VBO
	glGenBuffers(1, &mVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * mNumVerts, verts, GL_STATIC_DRAW);

	//IBO
	glGenBuffers(1, &mIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mNumIndices, indices, GL_STATIC_DRAW);


	GLsizei stride = sizeof(float) * 4;
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 2));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

VertexArray::~VertexArray()
{
	glDeleteBuffers(1, &mVertexBuffer);
	glDeleteBuffers(1, &mIndexBuffer);
	glDeleteVertexArrays(1, &mVertexArray);
}

void VertexArray::SetActive() const
{
	glBindVertexArray(mVertexArray);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
}
