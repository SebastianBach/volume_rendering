#include "renderengine.h"

// see https://github.com/Dav1dde/glad
#include "glad/glad.h"

#include "log.h"
#include <string>
#include <gl/GLU.h>

#include "modeling.h"

#include "glm/gtc/matrix_transform.hpp"

static bool CheckOglError()
{
	const auto error = glGetError();
	if (error != GL_NO_ERROR)
	{
		const unsigned char* errStr = gluErrorString(error);
		DATA_MSG("OpenGL Error at line:");
		ERROR_MSG((const char*)errStr);
		return false;
	}

	return true;
}

RenderEngine::RenderEngine()
{
	_noiseTexture = 0;
	_step = 0.0;
	_settings._renderMode = ShaderMode::Beauty;
	_settings._sphereMode = true;
	_settings._objectMode = ObjectMode::SPHERE;
}

RenderEngine::~RenderEngine()
{

}

bool RenderEngine::Init()
{
	if (IsNull(gladLoadGL(), "Could not load OGL functions.", ERROR_CONTEXT)) return false;
	if (IsFalse(CheckOglError(), "OpenGL Error.", ERROR_CONTEXT)) return false;

	// check OGL version / extensions
	DATA_MSG("Vendor:");
	DATA_MSG((const char*)glGetString(GL_VENDOR));
	DATA_MSG("Renderer:");
	DATA_MSG((const char*)glGetString(GL_RENDERER));
	DATA_MSG("GL Version:");
	DATA_MSG((const char*)glGetString(GL_VERSION));
	DATA_MSG("GLSL Version:");
	DATA_MSG((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

	// requires OGL 4.3
	/*
	glEnable(GL_DEBUG_OUTPUT);                  // Enables debug output
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);      // Synchronous debug output, later omit on release builds
	glDebugMessageCallback(debugCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
	glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0,
		GL_DEBUG_SEVERITY_NOTIFICATION, -1, "Start debugging");
		*/
	
	glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, 1280, 720);

	return true;
}

bool UnitTest()
{
	return true;
}

#include <iostream>
#include <cmath>

bool RenderEngine::CreateScene()
{
	if (IsFalse(CreatePlane(_viewPlane), "Could not create view plane.", ERROR_CONTEXT)) return false;
	if (IsFalse(CheckOglError(), "OpenGL Error.", ERROR_CONTEXT)) return false;
	if (IsFalse(CreatePlane(_ground), "Could not create ground plane.", ERROR_CONTEXT)) return false;
	if (IsFalse(CheckOglError(), "OpenGL Error.", ERROR_CONTEXT)) return false;

	// front shader
	if (IsFalse(_shader.Init(), "Shader setup failed.", ERROR_CONTEXT)) return false;

	const bool fragmentShader = _shader.LoadFragmentShader("shader/fragment_head.glsl", "shader/volume_body.glsl");
	if (IsFalse(fragmentShader, "Could not load fragment shader.", ERROR_CONTEXT)) return false;

	if (IsFalse(CheckOglError(), "OpenGL Error.", ERROR_CONTEXT)) return false;

	const bool vertexShader = _shader.LoadVertexShader("shader/vertex.glsl");
	if (IsFalse(vertexShader, "Could not load vertex shader.", ERROR_CONTEXT)) return false;

	if (IsFalse(_shader.Link(), "Could not link shader.", ERROR_CONTEXT)) return false;
	if (IsFalse(CheckOglError(), "OpenGL Error.", ERROR_CONTEXT)) return false;

	// ground shader
	if (IsFalse(_groundShader.Init(), "Shader setup failed.", ERROR_CONTEXT)) return false;
	if (IsFalse(_groundShader.LoadFragmentShader("shader/fragment_head.glsl", "shader/ground_body.glsl"), "Could not load fragment shader.", ERROR_CONTEXT)) return false;
	if (IsFalse(_groundShader.LoadVertexShader("shader/vertex.glsl"), "Could not load vertex shader.", ERROR_CONTEXT)) return false;
	if (IsFalse(_groundShader.Link(), "Could not link shader.", ERROR_CONTEXT)) return false;

	if (IsFalse(CheckOglError(), "OpenGL Error.", ERROR_CONTEXT)) return false;

	if (IsFalse(CreateNoiseTexture(), "Could not create noise texture.", ERROR_CONTEXT)) return false;

	// define standard matrcies
	_camPos = glm::vec3(0, 0, 2);
	_viewMatrix = glm::lookAt(_camPos, glm::vec3(0, 0, 0), glm::vec3(0.0f, 1.0f, 0.0f));

	_projectionMatrix = glm::perspectiveFov(1.0f, 1280.0f, 720.0f, 0.1f, 5.0f);

	_viewPlaneModelMatrix = glm::mat4(1.0f);
	_viewPlaneModelMatrix = glm::translate(_viewPlaneModelMatrix, glm::vec3(-2, -.5, 0));
	_viewPlaneModelMatrix = glm::scale(_viewPlaneModelMatrix, glm::vec3(4, 2, 2));


	_groundPlaneModelMatrix = glm::mat4(1.0f);
	_groundPlaneModelMatrix = glm::translate(_groundPlaneModelMatrix, glm::vec3(-3, -1, -2));
	_groundPlaneModelMatrix = glm::scale(_groundPlaneModelMatrix, glm::vec3(10, 2, 2));
	_groundPlaneModelMatrix = glm::rotate(_groundPlaneModelMatrix, glm::radians(90.0f), glm::vec3(1, 0, 0));

	INFO_MSG("Scene setup done...");

	return true;
}

void RenderEngine::UpdateScene(const SceneSettings& settings)
{
	_settings = settings;

	if(_settings._timeStep)
		_step = _step + 0.005f;

	_step = _step + _settings._timeOff;

}

#include <iostream>

bool RenderEngine::Render()
{
	// set up buffers
	// todo: make sub-function
	if (_settings._renderMode == 99)
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	else if (_settings._renderMode == 7)
		glClearColor(0.9f, 0.5f, 0.3f, 1.0f);
	else
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// enable alpha handling
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (IsFalse(CheckOglError(), "OpenGL Error.", ERROR_CONTEXT)) return false;

	// todo: make sub-function
	{
		if (IsFalse(_shader.Use(), "Could not use shader.", ERROR_CONTEXT)) return false;
		if (IsFalse(CheckOglError(), "OpenGL Error.", ERROR_CONTEXT)) return false;

		const glm::mat4 mv = _viewMatrix * _viewPlaneModelMatrix;
		const glm::mat4 MVP = _projectionMatrix * mv;

		_shader.SetUniform("MVP", MVP);
		_shader.SetUniform("ModelMatrix", _viewPlaneModelMatrix);
		_shader.SetUniform("camPos", _camPos);
		_shader.SetUniform("u_animation", _step);
		_shader.SetUniform("u_shadingMode", _settings._renderMode);
		//_shader.SetUniform("noiseTexture", unsigned int(0));
		//_shader.SetUniform("renderMode", unsigned int(_settings._renderMode));
		//_shader.SetUniform("sphereMode", unsigned int(_settings._sphereMode));
		_shader.SetUniform("u_objectMode", unsigned int(_settings._objectMode));
		_shader.SetUniform("u_dynX", _settings._dynamicObjectX);
		_shader.SetUniform("u_dynY", _settings._dynamicObjectY);
		

		if (IsFalse(CheckOglError(), "OpenGL Error.", ERROR_CONTEXT)) return false;

		if (IsFalse(_viewPlane.Draw(), "Could not draw view plane.", ERROR_CONTEXT)) return false;

		if (IsFalse(CheckOglError(), "OpenGL Error.", ERROR_CONTEXT)) return false;


		_shader.End();
	}
	
	// todo: make sub-function
	{
		// draw ground plane
		const glm::mat4 MVground = _viewMatrix * _groundPlaneModelMatrix;
		const glm::mat4 MVPground = _projectionMatrix * MVground;

		if (IsFalse(_groundShader.Use(), "Could not enable ground shader", ERROR_CONTEXT)) return false;
		if (IsFalse(_groundShader.SetUniform("MVP", MVPground), "Could not set MVP.", ERROR_CONTEXT)) return false;
		if (IsFalse(_groundShader.SetUniform("ModelMatrix", _groundPlaneModelMatrix), "Could not set model matrix.", ERROR_CONTEXT)) return false;
		//if (IsFalse(_groundShader.SetUniform("camPos", _camPos), "Could not set camPos.", ERROR_CONTEXT)) return false;
		//if (IsFalse(_groundShader.SetUniform("time", _step), "Could not set time.", ERROR_CONTEXT)) return false;
		//if (IsFalse(_groundShader.SetUniform("noiseTexture", unsigned int(0)), "Could not set noiseTexture.", ERROR_CONTEXT)) return false;
		//if (IsFalse(_groundShader.SetUniform("renderMode", unsigned int(_settings._renderMode)), "Could not set render mode.", ERROR_CONTEXT)) return false;
		//if (IsFalse(_groundShader.SetUniform("sphereMode", unsigned int(_settings._sphereMode)), "Could not set sphere mode.", ERROR_CONTEXT)) return false;
		// if (IsFalse(_groundShader.SetUniform("objectMode", unsigned int(_settings._objectMode)), "Could not set sphere mode.", ERROR_CONTEXT)) return false;


		_groundShader.SetUniform("u_animation", _step);
		//_groundShader.SetUniform("u_shadingMode", _settings._renderMode);
		//_shader.SetUniform("noiseTexture", unsigned int(0));
		//_shader.SetUniform("renderMode", unsigned int(_settings._renderMode));
		//_shader.SetUniform("sphereMode", unsigned int(_settings._sphereMode));
		_groundShader.SetUniform("u_objectMode", unsigned int(_settings._objectMode));
		_groundShader.SetUniform("u_dynX", _settings._dynamicObjectX);
		_groundShader.SetUniform("u_dynY", _settings._dynamicObjectY);

		if (IsFalse(_ground.Draw(), "Could not draw ground.", ERROR_CONTEXT)) return false;

		_groundShader.End();

		if (IsFalse(CheckOglError(), "OpenGL Error.", ERROR_CONTEXT)) return false;
	}


	return true;
}

bool RenderEngine::Close()
{
	glDeleteTextures(1, &_noiseTexture);

	return true;
}

#include "glm/gtc/noise.hpp"

bool RenderEngine::CreateNoiseTexture()
{
	/* see
	D. Wolff, OpenGL 4 shading language cookbook, Birmingham: Packt Publishing, 2013.

	Chapter "Creating a noise texture using GLM"
	*/
	
	glGenTextures(1, &_noiseTexture);
	if (IsNull(_noiseTexture, "Could not create OGL texture.", ERROR_CONTEXT)) return false;

	const int width = 256;
	const int height = 256;

	if (IsValue(width, 0, "Invalid width", ERROR_CONTEXT)) return false;
	if (IsValue(height, 0, "Invalid width", ERROR_CONTEXT)) return false;

	// TODO: change to one-component bitmap
	const int components = 4;

	// allocate data
	const int dataCnt = width * height * components;
	GLubyte * const data = new GLubyte[dataCnt];

	if (IsNullptr(data, "Could not allocate memory for noise texture.", ERROR_CONTEXT)) return false;

	const float widthF = float(width);
	const float heightF = float(height);


	const float scale = 6.5f;

	for (int y = 0; y< height; ++y)
	{
		const float yf = float(y) / heightF;

		for (int x = 0; x < width; ++x)
		{	
			const float xf = float(x) / widthF;

			// todo: we could also store different noises in different channels
			const glm::vec2 pos(xf * scale, yf * scale);
			const float noise = glm::perlin(pos);
			const float result = (noise + 1.0f) * .5f;
			const GLubyte byteRes = GLubyte(result * 255.9f);

			const int componentOffset = (y * width + x) * components;

			for (int c = 0; c < components; ++c)
			{
				const int componentIndex = componentOffset + c;
				data[componentIndex] = byteRes;
			}
		}
	}

	//setup OpenGL texture and bind to texture unit 0

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _noiseTexture);
	//set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	// copy texture data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);


	delete[] data;

	if (IsFalse(CheckOglError(), "OpenGL Error.", ERROR_CONTEXT)) return false;

	return true;
}


