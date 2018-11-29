#ifndef VOLUME_DEMO_POLYGONOBJECT_H__
#define VOLUME_DEMO_POLYGONOBJECT_H__

//---------------------------------------------------------------------------
/// A PolygonObjects represents an OpenGL VAO that stores polygon data and
/// associated data (normals, UVS).
//---------------------------------------------------------------------------
class PolygonObject
{
public:
	//---------------------------------------------------------------------------
	/// Constructor
	//---------------------------------------------------------------------------
	PolygonObject();
	//---------------------------------------------------------------------------
	/// Destructor
	//---------------------------------------------------------------------------
	~PolygonObject();

	//---------------------------------------------------------------------------
	/// Prepears the object.
	/// @return				False if an error occurred.
	//---------------------------------------------------------------------------
	bool Init();

	//---------------------------------------------------------------------------
	/// Stores vertex data.
	/// @param[in]	count	The number of all float elements.
	/// @param[in]	values	An array with float values.
	/// @return				False if an error occured.
	//---------------------------------------------------------------------------
	bool InitVertice(int count, float* values);

	//---------------------------------------------------------------------------
	/// Stores UV data.
	/// @param[in]	count	The number of all float elements.
	/// @param[in]	values	An array with float values.
	/// @return				False if an error occured.
	//---------------------------------------------------------------------------
	bool InitUVs(int count, float* values);

	//---------------------------------------------------------------------------
	/// Stores normal data.
	/// @param[in]	count	The number of all float elements.
	/// @param[in]	values	An array with float values.
	/// @return				False if an error occured.
	//---------------------------------------------------------------------------
	bool InitNormals(int count, float* values);

	//---------------------------------------------------------------------------
	/// Stores index data.
	/// @param[in]	count	The number of all int elements.
	/// @param[in]	values	An array with unsigned int values.
	/// @return				False if an error occured.
	//---------------------------------------------------------------------------
	bool InitIndice(int count, unsigned int* values);

	//---------------------------------------------------------------------------
	/// Draws the polygon object.
	/// @return				False if an error occurred.
	//---------------------------------------------------------------------------
	bool Draw() const;
public:

	unsigned int _vao;						///> OpenGL VAO ID.
	unsigned int _vertexBuffer;				///> Vertex buffer ID.
	unsigned int _uvBuffer;					///> UV buffer ID.
	unsigned int _normalBuffer;				///> Normal buffer ID.
	unsigned int _indexBuffer;				///> Index buffer ID.
	unsigned int _indexCount;				///> Number of indice.		
};


#endif
