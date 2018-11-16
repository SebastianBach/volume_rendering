#ifndef VOLUME_DEMO_POLYGONOBJECT_H__
#define VOLUME_DEMO_POLYGONOBJECT_H__

class PolygonObject
{
public:
	PolygonObject();
	~PolygonObject();

	//---------------------------------------------------------------------------
	/// Prepears the object.
	/// @return				False if an error occurred.
	//---------------------------------------------------------------------------
	bool Init();

	// count is count of all float elements
	bool InitVertice(int count, float* values);
	bool InitUVs(int count, float* values);
	bool InitNormals(int count, float* values);
	bool InitIndice(int count, unsigned int* values);

	//---------------------------------------------------------------------------
	/// Draws the polygon object.
	/// @return				False if an error occurred.
	//---------------------------------------------------------------------------
	bool Draw() const;

	void Clear();
public:

	unsigned int _vao;
	unsigned int _vertexBuffer;
	unsigned int _uvBuffer;
	unsigned int _normalBuffer;
	unsigned int _indexBuffer;
	unsigned int _indexCount;



};


#endif
