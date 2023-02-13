#include "polygonobject.h"

#include "log.h"

#include "glad/glad.h"

PolygonObject::PolygonObject()
{
    _vao          = 0;
    _vertexBuffer = 0;
    _uvBuffer     = 0;
    _normalBuffer = 0;
    _indexBuffer  = 0;
    _indexCount   = 0;
}

PolygonObject::~PolygonObject() {}

bool PolygonObject::Init()
{
    if (IsNotValue(_vao, 0U, MSG_INFO("_vao already set.")))
        return false;

    glGenVertexArrays(1, &_vao);

    if (IsNull(_vao, MSG_INFO("Failed to generate VAO")))
        return false;

    return true;
}

bool PolygonObject::InitVertice(int count, float* values)
{
    if (IsNull(_vao, MSG_INFO("VAO not set.")))
        return false;
    if (IsNullptr(values, MSG_INFO("Invalid vertice values")))
        return false;
    if (IsNull(count, MSG_INFO("Invalid count argument.")))
        return false;

    glGenBuffers(1, &_vertexBuffer);
    if (IsNull(_vertexBuffer, MSG_INFO("Could not create vertex buffer.")))
        return false;

    // store data
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), values,
                 GL_STATIC_DRAW);

    // activate VAO
    glBindVertexArray(_vao);

    glEnableVertexAttribArray(0); // Vertex position
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0,
                          ((GLubyte*)NULL + (0)));

    // deactivate VAO
    glBindVertexArray(0);

    return true;
}

bool PolygonObject::InitUVs(int count, float* values)
{
    if (IsNull(_vao, MSG_INFO("VAO not set.")))
        return false;
    if (IsNullptr(values, MSG_INFO("Invalid UV values")))
        return false;
    if (IsNull(count, MSG_INFO("Invalid count argument.")))
        return false;

    glGenBuffers(1, &_uvBuffer);
    if (IsNull(_uvBuffer, MSG_INFO("Could not create UV buffer.")))
        return false;

    // store data
    glBindBuffer(GL_ARRAY_BUFFER, _uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), values,
                 GL_STATIC_DRAW);

    // activate VAO
    glBindVertexArray(_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _uvBuffer);
    glEnableVertexAttribArray(2); // Texture coords
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0,
                          ((GLubyte*)NULL + (0)));

    // deactivate VAO
    glBindVertexArray(0);

    return true;
}

bool PolygonObject::InitNormals(int count, float* values)
{
    if (IsNull(_vao, MSG_INFO("VAO not set.")))
        return false;
    if (IsNullptr(values, MSG_INFO("Invalid normals values")))
        return false;
    if (IsNull(count, MSG_INFO("Invalid count argument.")))
        return false;

    glGenBuffers(1, &_normalBuffer);

    if (IsNull(_normalBuffer, MSG_INFO("Could not create normal buffer.")))
        return false;

    // store data
    glBindBuffer(GL_ARRAY_BUFFER, _normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), values,
                 GL_STATIC_DRAW);

    // activate VAO
    glBindVertexArray(_vao);

    glEnableVertexAttribArray(1); // Vertex normal
    glBindBuffer(GL_ARRAY_BUFFER, _normalBuffer);
    glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0,
                          ((GLubyte*)NULL + (0)));

    // deactivate VAO
    glBindVertexArray(0);

    return true;
}

bool PolygonObject::InitIndice(int count, unsigned int* values)
{
    if (IsNull(_vao, MSG_INFO("VAO not set.")))
        return false;
    if (IsNullptr(values, MSG_INFO("Invalid indice values")))
        return false;
    if (IsNull(count, MSG_INFO("Invalid count argument.")))
        return false;

    glGenBuffers(1, &_indexBuffer);
    if (IsNull(_indexBuffer, MSG_INFO("Could not create index buffer.")))
        return false;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), values,
                 GL_STATIC_DRAW);

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
    if (IsNull(_vao, MSG_INFO("Invalid VAO.")))
        return false;
    if (IsNull(_indexCount, MSG_INFO("Invalid index count.")))
        return false;

    glBindVertexArray(_vao);
    glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT,
                   ((GLubyte*)NULL + (0)));

    return true;
}