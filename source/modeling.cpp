#include "modeling.h"

#include "log.h"

static bool SetPolyData(PolygonObject& poly, 
	int vertexSize, float * const vertexData,
	int normalSize, float* const normalData,
	int uvSize, float* const uvData,
	int indexSize, unsigned int* const indexData)
{
	if (IsFalse(poly.InitVertice(vertexSize, vertexData), "Could not set vertice.", ERROR_CONTEXT))
		return false;

	if (IsFalse(poly.InitNormals(normalSize, normalData), "Could not set normals.", ERROR_CONTEXT))
		return false;

	if (IsFalse(poly.InitUVs(uvSize, uvData), "Could not set UVs.", ERROR_CONTEXT))
		return false;

	if (IsFalse(poly.InitIndice(indexSize, indexData), "Could not set indice.", ERROR_CONTEXT))
		return false;

	return true;
}

void SetVector(float* const v, unsigned int index, unsigned int count, float x, float y, float z)
{
	if (v == nullptr)
	{
		ERROR_MSG("Invalid argument.");
		return;
	}

	const unsigned int startOffset = index * 3;
	const unsigned int maxOffset = startOffset + 2;

	if (maxOffset >= count)
	{
		ERROR_MSG("Index out of bounds.");
		return;
	}

	v[startOffset] = x;
	v[startOffset + 1] = y;
	v[startOffset + 2] = z;
}

static void SetUV(float* const v, unsigned int index, unsigned int count, float uValue, float vValue)
{
	if (v == nullptr)
	{
		ERROR_MSG("Invalid argument.");
		return;
	}

	const unsigned int startOffset = index * 2;
	const unsigned int maxOffset = startOffset + 1;

	if (maxOffset >= count)
	{
		ERROR_MSG("Index out of bounds.");
		return;
	}

	v[startOffset] = uValue;
	v[startOffset + 1] = vValue;
}

void SetTriangle(unsigned int* const v, unsigned int index, unsigned int count, int a, int b, int c)
{
	if (v == nullptr)
	{
		ERROR_MSG("Invalid argument.");
		return;
	}

	const unsigned int startOffset = index * 3;
	const unsigned int maxOffset = startOffset + 2;

	if (maxOffset >= count)
	{
		ERROR_MSG("Index out of bounds.");
		return;
	}

	v[startOffset] = a;
	v[startOffset + 1] = b;
	v[startOffset + 2] = c;
}

bool CreateTriangle(PolygonObject& poly)
{
	if(IsFalse(poly.Init(), "Could not create polygon object.", ERROR_CONTEXT)) 
		return false;

	const unsigned int vertexCount = 3;
	const unsigned int triangleCount = 1;

	// vertice
	const unsigned int vertexDataSize = vertexCount * 3;
	float vertexData[vertexDataSize];
	// uvs
	const unsigned int uvDataSize = vertexCount * 2;
	float uvData[uvDataSize];
	// normals
	const unsigned int normalDataSize = vertexCount * 3;
	float normalData[normalDataSize];
	// indice
	const unsigned int indexDataSize = triangleCount * 3;
	unsigned int indexData[indexDataSize];

	// set vertice
	SetVector(vertexData, 0, vertexDataSize, 0.0, 0.0, 0.0);
	SetVector(vertexData, 1, vertexDataSize, 1.0, 0.0, 0.0);
	SetVector(vertexData, 2, vertexDataSize, 1.0, 1.0, 0.0);

	// set normals
	SetVector(normalData, 0, normalDataSize, 0.0, 0.0, 1.0);
	SetVector(normalData, 1, normalDataSize, 0.0, 0.0, 1.0);
	SetVector(normalData, 2, normalDataSize, 0.0, 0.0, 1.0);

	// set UVs
	SetUV(uvData, 0, uvDataSize, 0.0, 0.0);
	SetUV(uvData, 1, uvDataSize, 1.0, 0.0);
	SetUV(uvData, 2, uvDataSize, 1.0, 1.0);

	// set index
	SetTriangle(indexData, 0, indexDataSize, 0, 1, 2);

	// set data

	const bool res = SetPolyData(poly, 
		vertexDataSize, vertexData,
		normalDataSize, normalData,
		uvDataSize, uvData,
		indexDataSize, indexData);

	if (IsFalse(res, "Could not store polygon data.", ERROR_CONTEXT)) 
		return false;

	return true;
}

bool CreatePlane(PolygonObject& poly)
{
	if (IsFalse(poly.Init(), "Could not create polygon object.", ERROR_CONTEXT)) 
		return false;

	const unsigned int vertexCount = 4;
	const unsigned int triangleCount = 2;

	// vertice
	const unsigned int vertexDataSize = vertexCount * 3;
	float vertexData[vertexDataSize];
	// uvs
	const unsigned int uvDataSize = vertexCount * 2;
	float uvData[uvDataSize];
	// normals
	// todo: set per-face normals
	const unsigned int normalDataSize = vertexCount * 3;
	float normalData[normalDataSize];
	// indice
	const unsigned int indexDataSize = triangleCount * 3;
	unsigned int indexData[indexDataSize];

	// set vertice
	SetVector(vertexData, 0, vertexDataSize, 0.0, 0.0, 0.0);
	SetVector(vertexData, 1, vertexDataSize, 1.0, 0.0, 0.0);
	SetVector(vertexData, 2, vertexDataSize, 1.0, 1.0, 0.0);
	SetVector(vertexData, 3, vertexDataSize, 0.0, 1.0, 0.0);

	// set normals
	SetVector(normalData, 0, normalDataSize, 0.0, 0.0, 1.0);
	SetVector(normalData, 1, normalDataSize, 0.0, 0.0, 1.0);
	SetVector(normalData, 2, normalDataSize, 0.0, 0.0, 1.0);
	SetVector(normalData, 3, normalDataSize, 0.0, 0.0, 1.0);

	// set UVs
	SetUV(uvData, 0, uvDataSize, 0.0, 0.0);
	SetUV(uvData, 1, uvDataSize, 1.0, 0.0);
	SetUV(uvData, 2, uvDataSize, 1.0, 1.0);
	SetUV(uvData, 3, uvDataSize, 0.0, 1.0);

	// set index
	SetTriangle(indexData, 0, indexDataSize, 0, 1, 2);
	SetTriangle(indexData, 1, indexDataSize, 0, 2, 3);

	// set data
	const bool res = SetPolyData(poly, 
		vertexDataSize, vertexData,
		normalDataSize, normalData,
		uvDataSize, uvData,
		indexDataSize, indexData);

	if(IsFalse(res, "Could not store polygon data.", ERROR_CONTEXT)) 
		return false;

	return true;
}
