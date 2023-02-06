#include "modeling.h"
#include "log.h"

//---------------------------------------------------------------------------
/// Stores the polygon data in the given PolygonObject.
/// @param[out]	poly		The target PolygonObject.
/// @param[in]	vertexSize	The size of vertex data.
/// @param[in]	vertexData	Vertex data array.
/// @param[in]	normalSize	The size of normal data.
/// @param[in]	normalData	Normal data array.
/// @param[in]	uvSize		The size of UV data.
/// @param[in]	uvData		UV data array.
/// @param[in]	indexSize	The size of indice data.
/// @param[in]	indexData	Index data array.
/// @return					False if an error occurred.
//---------------------------------------------------------------------------
static bool SetPolyData(PolygonObject& poly, int vertexSize,
                        float* const vertexData, int normalSize,
                        float* const normalData, int uvSize,
                        float* const uvData, int indexSize,
                        unsigned int* const indexData)
{
    if (IsFalse(poly.InitVertice(vertexSize, vertexData),
                MSG_INFO("Could not set vertice.")))
        return false;

    if (IsFalse(poly.InitNormals(normalSize, normalData),
                MSG_INFO("Could not set normals.")))
        return false;

    if (IsFalse(poly.InitUVs(uvSize, uvData), MSG_INFO("Could not set UVs.")))
        return false;

    if (IsFalse(poly.InitIndice(indexSize, indexData),
                MSG_INFO("Could not set indice.")))
        return false;

    return true;
}

//---------------------------------------------------------------------------
/// Stores a vector in the given vector array.
/// @param[in]	v		The vector array.
/// @param[in]	index	The index to store the vector at.
/// @param[in]	count	Maximum number of elements in the vector array.
/// @param[in]	x		Vector x component.
/// @param[in]	y		Vector y component.
/// @param[in]	z		Vector z component.
//---------------------------------------------------------------------------
void SetVector(float* const v, unsigned int index, unsigned int count, float x,
               float y, float z)
{
    if (v == nullptr)
    {
        ErrorMessage(MSG_INFO("Invalid argument."));
        return;
    }

    const unsigned int startOffset = index * 3;
    const unsigned int maxOffset   = startOffset + 2;

    if (maxOffset >= count)
    {
        ErrorMessage(MSG_INFO("Index out of bounds."));
        return;
    }

    v[startOffset]     = x;
    v[startOffset + 1] = y;
    v[startOffset + 2] = z;
}

//---------------------------------------------------------------------------
/// Stores a set of UV coordinates in the given array.
/// @param[in]	v		The UV array.
/// @param[in]	index	The index to store the UV set at.
/// @param[in]	count	Maximum number of elements in the UV array.
/// @param[in]	uValue	UV u-value.
/// @param[in]	uValue	UV v-value
//---------------------------------------------------------------------------
static void SetUV(float* const v, unsigned int index, unsigned int count,
                  float uValue, float vValue)
{
    if (v == nullptr)
    {
        ErrorMessage(MSG_INFO("Invalid argument."));
        return;
    }

    const unsigned int startOffset = index * 2;
    const unsigned int maxOffset   = startOffset + 1;

    if (maxOffset >= count)
    {
        ErrorMessage(MSG_INFO("Index out of bounds."));
        return;
    }

    v[startOffset]     = uValue;
    v[startOffset + 1] = vValue;
}

//---------------------------------------------------------------------------
/// Stores a set of indice in the given index array.
/// @param[in]	v		The index array.
/// @param[in]	index	The index to store the indice at.
/// @param[in]	count	Maximum number of elements in the index array.
/// @param[in]	a		The first index.
/// @param[in]	b		The second index.
/// @param[in]	c		The third index.
//---------------------------------------------------------------------------
void SetTriangle(unsigned int* const v, unsigned int index, unsigned int count,
                 int a, int b, int c)
{
    if (v == nullptr)
    {
        ErrorMessage(MSG_INFO("Invalid argument."));
        return;
    }

    const unsigned int startOffset = index * 3;
    const unsigned int maxOffset   = startOffset + 2;

    if (maxOffset >= count)
    {
        ErrorMessage(MSG_INFO("Index out of bounds."));
        return;
    }

    v[startOffset]     = a;
    v[startOffset + 1] = b;
    v[startOffset + 2] = c;
}

bool CreateTriangle(PolygonObject& poly)
{
    if (IsFalse(poly.Init(), MSG_INFO("Could not create polygon object.")))
        return false;

    const unsigned int vertexCount   = 3;
    const unsigned int triangleCount = 1;

    // vertice
    const unsigned int vertexDataSize = vertexCount * 3;
    float              vertexData[vertexDataSize];
    // uvs
    const unsigned int uvDataSize = vertexCount * 2;
    float              uvData[uvDataSize];
    // normals
    const unsigned int normalDataSize = vertexCount * 3;
    float              normalData[normalDataSize];
    // indice
    const unsigned int indexDataSize = triangleCount * 3;
    unsigned int       indexData[indexDataSize];

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

    const bool res =
        SetPolyData(poly, vertexDataSize, vertexData, normalDataSize,
                    normalData, uvDataSize, uvData, indexDataSize, indexData);

    if (IsFalse(res, MSG_INFO("Could not store polygon data.")))
        return false;

    return true;
}

bool CreatePlane(PolygonObject& poly)
{
    if (IsFalse(poly.Init(), MSG_INFO("Could not create polygon object.")))
        return false;

    const unsigned int vertexCount   = 4;
    const unsigned int triangleCount = 2;

    // vertice
    const unsigned int vertexDataSize = vertexCount * 3;
    float              vertexData[vertexDataSize];
    // uvs
    const unsigned int uvDataSize = vertexCount * 2;
    float              uvData[uvDataSize];
    // normals
    // todo: set per-face normals
    const unsigned int normalDataSize = vertexCount * 3;
    float              normalData[normalDataSize];
    // indice
    const unsigned int indexDataSize = triangleCount * 3;
    unsigned int       indexData[indexDataSize];

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
    const bool res =
        SetPolyData(poly, vertexDataSize, vertexData, normalDataSize,
                    normalData, uvDataSize, uvData, indexDataSize, indexData);

    if (IsFalse(res, MSG_INFO("Could not store polygon data.")))
        return false;

    return true;
}
