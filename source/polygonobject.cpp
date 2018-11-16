#include "polygonobject.h"

#include "log.h"

#include "glad/glad.h"

PolygonObject::PolygonObject()
{
	_vao = 0;
	_vertexBuffer = 0;
	_uvBuffer = 0;
	_normalBuffer = 0;
	_indexBuffer = 0;
}

PolygonObject::~PolygonObject()
{

}

bool PolygonObject::Init()
{
	if (IsNotValue(_vao, 0U, "_vao already set.", ERROR_CONTEXT)) return false;

	glGenVertexArrays(1, &_vao);

	if (IsNull(_vao, "Faild to generate VAO", ERROR_CONTEXT)) return false;

	return true;
}

bool PolygonObject::InitVertice(int count, float* values)
{
	if (IsNull(_vao, "VAO not set.", ERROR_CONTEXT)) return false;
	if (IsNullptr(values, "Invalid vertice values", ERROR_CONTEXT)) return false;
	if (IsNull(count, "Invalid count argument.", ERROR_CONTEXT)) return false;

	glGenBuffers(1, &_vertexBuffer);
	if (IsNull(_vertexBuffer, "Could not create vertex buffer.", ERROR_CONTEXT)) return false;

	// store data
	glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), values, GL_STATIC_DRAW);

	// activate VAO
	glBindVertexArray(_vao);

	glEnableVertexAttribArray(0);  // Vertex position
	glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)));

	// deactivate VAO
	glBindVertexArray(0);

	return true;
}

bool PolygonObject::InitUVs(int count, float* values)
{
	if (IsNull(_vao, "VAO not set.", ERROR_CONTEXT)) return false;
	if (IsNullptr(values, "Invalid UV values", ERROR_CONTEXT)) return false;
	if (IsNull(count, "Invalid count argument.", ERROR_CONTEXT)) return false;

	glGenBuffers(1, &_uvBuffer);
	if (IsNull(_uvBuffer, "Could not create UV buffer.", ERROR_CONTEXT)) return false;

	// store data
	glBindBuffer(GL_ARRAY_BUFFER, _uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), values, GL_STATIC_DRAW);

	// activate VAO
	glBindVertexArray(_vao);

	glBindBuffer(GL_ARRAY_BUFFER, _uvBuffer);
	glEnableVertexAttribArray(2);  // Texture coords
	glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)));

	// deactivate VAO
	glBindVertexArray(0);

	return true;
}

bool PolygonObject::InitNormals(int count, float* values)
{
	if (IsNull(_vao, "VAO not set.", ERROR_CONTEXT)) return false;
	if (IsNullptr(values, "Invalid normals values", ERROR_CONTEXT)) return false;
	if (IsNull(count, "Invalid count argument.", ERROR_CONTEXT)) return false;

	glGenBuffers(1, &_normalBuffer);

	if (IsNull(_normalBuffer, "Could not create normal buffer.", ERROR_CONTEXT)) return false;

	// store data
	glBindBuffer(GL_ARRAY_BUFFER, _normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), values, GL_STATIC_DRAW);

	// activate VAO
	glBindVertexArray(_vao);

	glEnableVertexAttribArray(1);  // Vertex normal
	glBindBuffer(GL_ARRAY_BUFFER, _normalBuffer);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)));

	// deactivate VAO
	glBindVertexArray(0);


	return true;
}

bool PolygonObject::InitIndice(int count, unsigned int* values)
{
	if (IsNull(_vao, "VAO not set.", ERROR_CONTEXT)) return false;
	if (IsNullptr(values, "Invali indice values", ERROR_CONTEXT)) return false;
	if (IsNull(count, "Invalid count argument.", ERROR_CONTEXT)) return false;

	glGenBuffers(1, &_indexBuffer);
	if (IsNull(_indexBuffer, "Could not create index buffer.", ERROR_CONTEXT)) return false;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), values, GL_STATIC_DRAW);

	// activate VAO
	glBindVertexArray(_vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);

	// deactivate VAO
	glBindVertexArray(0);

	_indexCount = count;

	return true;
}

bool PolygonObject::Draw() const
{
	if (IsNull(_vao, "Invalid VAO.", ERROR_CONTEXT)) return false;
	if (IsNull(_indexCount, "Invalid index count.", ERROR_CONTEXT)) return false;

	glBindVertexArray(_vao);
	glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, ((GLubyte *)NULL + (0)));

	return true;
}

void PolygonObject::Clear()
{

}
