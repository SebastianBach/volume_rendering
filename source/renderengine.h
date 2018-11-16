#ifndef VOLUME_DEMO_RENDERENGINE_H__
#define VOLUME_DEMO_RENDERENGINE_H__

#include "window.h"
#include "polygonobject.h"
#include "program.h"

#include <vector>

enum ShaderMode
{
	Beauty = 0,
	LambertLighting,
	PhongLighting,
	Fresnel,
	Normals,
	Experimental
};

enum ObjectMode: unsigned int
{
	SPHERE = 1,
	METABALL = 2,
	DEFORMED_SPHERE = 3
};

struct SceneSettings
{
	bool _timeStep;
	unsigned int _renderMode;
	float _timeOff;
	bool _sphereMode;
	ObjectMode _objectMode;		// 0: sphere, 1: metaballs, 2: defomred spheres.
	float _dynamicObjectX;
	float _dynamicObjectY;
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


	float _step;


	SceneSettings _settings;

	std::vector<glm::vec3> _sphereVec;
	
};

#endif

