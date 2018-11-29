#ifndef VOLUME_DEMO_RENDERENGINE_H__
#define VOLUME_DEMO_RENDERENGINE_H__

#include "window.h"
#include "polygonobject.h"
#include "program.h"

#include <vector>

//---------------------------------------------------------------------------
/// Maximum number of objects.
//---------------------------------------------------------------------------
static const int MAX_OBJECT_COUNT = 18;

//---------------------------------------------------------------------------
/// Utility class storing all information on the scene objects.
//---------------------------------------------------------------------------
class ObjectArray
{
public:
	//---------------------------------------------------------------------------
	/// Constructor
	//---------------------------------------------------------------------------
	ObjectArray();

	//---------------------------------------------------------------------------
	/// Destructor.
	//---------------------------------------------------------------------------
	~ObjectArray();

	//---------------------------------------------------------------------------
	/// Adds an object to the scene.
	/// @param[in]	pos		The object position.
	/// @param[in]	color	The object color.
	/// @param[out]	index	The new object index.
	/// @return				False if an error occured.
	//---------------------------------------------------------------------------
	bool AddObject(glm::vec3& pos, glm::vec3& color, int& index);

	//---------------------------------------------------------------------------
	/// Adds an object to the scene.
	/// @return				False if an error occured.
	//---------------------------------------------------------------------------
	bool AddObject();

	//---------------------------------------------------------------------------
	/// Returns the number of objects in the scene
	/// @return				The number of objects.
	//---------------------------------------------------------------------------
	unsigned int GetObjectCount();

	//---------------------------------------------------------------------------
	/// Sets the position of the dynamic, user controlled object.
	/// @param[in]	x		The x-coordinate.
	/// @param[int]	y		The y-coordinate.
	/// @return				False if an error occured.
	//---------------------------------------------------------------------------
	bool SetDynamicObject(float x, float y);

	//---------------------------------------------------------------------------
	/// Removes the last object from the scene.
	/// @return				False if an error occured.
	//---------------------------------------------------------------------------
	bool RemoveLastObject();

	//---------------------------------------------------------------------------
	/// Returns the size of the arrays accessed with GetPositionData() and GetColorData().
	/// @return				The size of the arrays.
	//---------------------------------------------------------------------------
	int GetDataSize();

	//---------------------------------------------------------------------------
	/// Returns the array containing position data.
	/// @return				The position data.
	//---------------------------------------------------------------------------
	glm::vec3* GetPositionData();

	//---------------------------------------------------------------------------
	/// Returns the array containing color data.
	/// @return				The color data.
	//---------------------------------------------------------------------------
	glm::vec3* GetColorData();

	//---------------------------------------------------------------------------
	/// Animates the scene.
	/// @param[in]	step		The current animation step.
	//---------------------------------------------------------------------------
	void Animation(float step);


private:

	int _count;						///> number of elements.
	std::vector<glm::vec3> _pos;	///> position information.
	std::vector<glm::vec3> _colors;	///> color information.
	glm::vec3 _userObject;			///> position of the user object.
	bool _countChanged;				///> True if the number of elements has changed. Reset Animation().
};

enum ShaderMode
{
	Beauty = 0,
	LambertLighting,
	PhongLighting,
	Fresnel,
	Normals,
	Experimental
};

enum NoiseMode : unsigned int
{
	NOISE = 1,
	NO_NOISE = 2
};

//---------------------------------------------------------------------------
/// General scene settings.
//---------------------------------------------------------------------------
struct SceneSettings
{
	bool _timeStep;
	unsigned int _renderMode;
	float _timeOff;
	float _dynamicObjectX;
	float _dynamicObjectY;
	NoiseMode _noise;
	bool _addSphere;
	bool _removeObject;
	bool _addObject;
};

class RenderEngine
{
public:
	RenderEngine();
	~RenderEngine();

	//---------------------------------------------------------------------------
	/// Prepares OpenGL.
	/// @return				False if an error occurred.
	//---------------------------------------------------------------------------
	bool Init();

	//---------------------------------------------------------------------------
	/// Creates the scene data. Must be called after Init().
	/// @return				False if an error occurred.
	//---------------------------------------------------------------------------
	bool CreateScene();

	//---------------------------------------------------------------------------
	/// Updates the scene.
	/// @return				False if an error occurred.
	//---------------------------------------------------------------------------
	void UpdateScene(const SceneSettings& settings);

	//---------------------------------------------------------------------------
	/// Renders the scene.
	/// @return				False if an error occurred.
	//---------------------------------------------------------------------------
	bool Render();

	bool Close();

private:

	bool CreateNoiseTexture();

	PolygonObject _viewPlane;
	PolygonObject _ground;

	ShaderProgram _shader;
	ShaderProgram _groundShader;

	unsigned int _noiseTexture;


	// scene
	glm::vec3 _camPos;
	glm::mat4 _projectionMatrix;
	glm::mat4 _viewMatrix;

	glm::mat4 _viewPlaneModelMatrix;
	glm::mat4 _groundPlaneModelMatrix;


	float _step; // current animation time

	SceneSettings _settings;

	ObjectArray _objects;

	
};

#endif

