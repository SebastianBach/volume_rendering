#ifndef VOLUME_DEMO_RENDERENGINE_H__
#define VOLUME_DEMO_RENDERENGINE_H__

#include "window.h"
#include "polygonobject.h"
#include "program.h"

#include <vector>

static const int MAX_OBJECT_COUNT = 18;

class ObjectArray
{
public:
	ObjectArray();
	~ObjectArray();

	bool AddObject(glm::vec3& pos, glm::vec3& color, int& index);
	bool AddObject();


	unsigned int GetObjectCount();
	//bool GetObjectPos(int index, glm::vec3& pos);
	bool SetObjectPos(int index, glm::vec3& pos);
	//bool GetObjectColor(int index, glm::vec3& color);
	//bool SetObjectColor(int index, glm::vec3& color);

	bool SetDynamicObject(float x, float y);

	bool RemoveLastObject();

	
	int GetDataSize();
	glm::vec3* GetPositionData();
	glm::vec3* GetColorData();

	void Animation(float step);


private:

	bool IsIndexOK(int index);

	int _count;
	std::vector<glm::vec3> _pos;
	std::vector<glm::vec3> _colors;
	glm::vec3 _userObject;
	bool _countChanged;
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

	// executes unit tests to check the OGL functionality
	// must be called after Init()
	bool UnitTest();

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

