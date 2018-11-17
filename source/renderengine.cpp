#include "renderengine.h"

// see https://github.com/Dav1dde/glad
#include <glad/glad.h>

#include "log.h"
#include <string>
#include <cstdlib>
#include <gl/GLU.h>

#include "modeling.h"

#include "glm/gtc/matrix_transform.hpp"


template<class ...Args> static bool SetUniform(ShaderProgram& prog, const char* name, Args... args)
{
	 bool uniformResult = prog.SetUniform(name, (args)...);

	 auto composeErrorMessage = [name]()->std::string
	 {
		 std::string message("Failed to set uniform variable: ");
		 message.append(name);
		 return message;
	 };

	if (IsFalse(uniformResult, MSG_INFO(composeErrorMessage())))
		return false;

	return true;
}



static bool CheckOglError()
{
	const auto error = glGetError();
	if (error != GL_NO_ERROR)
	{
		const unsigned char* errStr = gluErrorString(error);
		DataMessage(MSG_INFO(("OpenGL Error at line:")));
		ErrorMessage(MSG_INFO((const char*)errStr));
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
	if (IsNull(gladLoadGL(), MSG_INFO("Could not load OGL functions."))) return false;
	if (IsFalse(CheckOglError(), MSG_INFO("OpenGL Error."))) return false;

	// check OGL version / extensions
	DataMessage(MSG_INFO("Vendor:"));
	DataMessage(MSG_INFO((const char*)glGetString(GL_VENDOR)));
	DataMessage(MSG_INFO(":"));
	DataMessage(MSG_INFO((const char*)glGetString(GL_RENDERER)));
	DataMessage(MSG_INFO("GL Version:"));
	DataMessage(MSG_INFO((const char*)glGetString(GL_VERSION)));
	DataMessage(MSG_INFO("GLSL Version:"));
	DataMessage(MSG_INFO((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION)));
	
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
	if (IsFalse(CreatePlane(_viewPlane), MSG_INFO("Could not create view plane."))) return false;
	if (IsFalse(CheckOglError(), MSG_INFO("OpenGL Error."))) return false;
	if (IsFalse(CreatePlane(_ground), MSG_INFO("Could not create ground plane."))) return false;
	if (IsFalse(CheckOglError(), MSG_INFO("OpenGL Error."))) return false;

	// front shader
	if (IsFalse(_shader.Init(), MSG_INFO("Shader setup failed."))) return false;

	const bool fragmentShader = _shader.LoadFragmentShader("shader/fragment_head.glsl", "shader/volume_body.glsl");
	if (IsFalse(fragmentShader, MSG_INFO("Could not load fragment shader."))) return false;

	if (IsFalse(CheckOglError(), MSG_INFO("OpenGL Error."))) return false;

	const bool vertexShader = _shader.LoadVertexShader("shader/vertex.glsl");
	if (IsFalse(vertexShader, MSG_INFO("Could not load vertex shader."))) return false;

	if (IsFalse(_shader.Link(), MSG_INFO("Could not link shader."))) return false;
	if (IsFalse(CheckOglError(), MSG_INFO("OpenGL Error."))) return false;

	// ground shader
	if (IsFalse(_groundShader.Init(), MSG_INFO("Shader setup failed."))) return false;
	if (IsFalse(_groundShader.LoadFragmentShader("shader/fragment_head.glsl", "shader/ground_body.glsl"), MSG_INFO("Could not load fragment shader."))) return false;
	if (IsFalse(_groundShader.LoadVertexShader("shader/vertex.glsl"), MSG_INFO("Could not load vertex shader."))) return false;
	if (IsFalse(_groundShader.Link(), MSG_INFO("Could not link shader."))) return false;

	if (IsFalse(CheckOglError(), MSG_INFO("OpenGL Error."))) return false;

	if (IsFalse(CreateNoiseTexture(), MSG_INFO("Could not create noise texture."))) return false;

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


	_sphereVec.push_back(glm::vec3(1, 1, -1.0));
	_sphereVec.push_back(glm::vec3(-1, -1, -1.0));
	_sphereVec.push_back(glm::vec3(1, -1, -1.0));

	DataMessage(MSG_INFO(("Scene setup done...")));

	return true;
}

void RenderEngine::UpdateScene(const SceneSettings& settings)
{
	_settings = settings;

	if(_settings._timeStep)
		_step = _step + 0.005f;

	_step = _step + _settings._timeOff;

	// simulation

	std::srand(_step);

	glm::vec3 dynObject = glm::vec3(settings._dynamicObjectX, settings._dynamicObjectY, -1.0);

	const int cnt = _sphereVec.size();

	for (int i = 0; i < cnt; ++i)
	{
		glm::vec3 pos = _sphereVec.at(i);
		glm::vec3 diff = dynObject - pos;
		float length = glm::length(diff);
		if (length < 0.6)
		{
			float random = float(std::rand()) / RAND_MAX;

			diff = diff * 0.03f;
			// diff.x +=( random * 0.1f);
			//diff.y += (random * 0.1f);

			pos = pos + diff;
			pos.z = -1.0;

			_sphereVec[i] = pos;
		}
	}

}

#include <iostream>

bool RenderEngine::Render()
{
	// set up buffers
	//glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// enable alpha handling
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (IsFalse(CheckOglError(), MSG_INFO("OpenGL Error."))) return false;

	// todo: make sub-function
	{
		if (IsFalse(_shader.Use(), MSG_INFO("Could not use shader."))) return false;
		if (IsFalse(CheckOglError(), MSG_INFO("OpenGL Error."))) return false;

		const glm::mat4 mv = _viewMatrix * _viewPlaneModelMatrix;
		const glm::mat4 MVP = _projectionMatrix * mv;

		if (!SetUniform(_shader, "MVP", MVP)) return false;
		if (!SetUniform(_shader, "ModelMatrix", _viewPlaneModelMatrix)) return false;
		if (!SetUniform(_shader, "u_spheres", &_sphereVec.front(), 9)) return false;
		if (!SetUniform(_shader, "u_dynY", _settings._dynamicObjectY)) return false;
		if (!SetUniform(_shader, "u_dynX", _settings._dynamicObjectX)) return false;
		if (!SetUniform(_shader, "u_objectMode", unsigned int(_settings._objectMode))) return false;
		if (!SetUniform(_shader, "u_shadingMode", _settings._renderMode)) return false;
		if (!SetUniform(_shader, "u_animation", _step)) return false;
		if (!SetUniform(_shader, "camPos", _camPos)) return false;

		if (IsFalse(CheckOglError(), MSG_INFO("OpenGL Error."))) return false;

		if (IsFalse(_viewPlane.Draw(), MSG_INFO("Could not draw view plane."))) return false;

		if (IsFalse(CheckOglError(), MSG_INFO("OpenGL Error."))) return false;


		_shader.End();
	}
	
	// todo: make sub-function
	{
		// draw ground plane
		const glm::mat4 MVground = _viewMatrix * _groundPlaneModelMatrix;
		const glm::mat4 MVPground = _projectionMatrix * MVground;

		if (IsFalse(_groundShader.Use(), MSG_INFO("Could not enable ground shader"))) return false;
		if (IsFalse(_groundShader.SetUniform("MVP", MVPground), MSG_INFO("Could not set MVP."))) return false;
		if (IsFalse(_groundShader.SetUniform("ModelMatrix", _groundPlaneModelMatrix), MSG_INFO("Could not set model matrix."))) return false;

		_groundShader.SetUniform("u_animation", _step);
		_groundShader.SetUniform("u_objectMode", unsigned int(_settings._objectMode));
		_groundShader.SetUniform("u_dynX", _settings._dynamicObjectX);
		_groundShader.SetUniform("u_dynY", _settings._dynamicObjectY);

		if (IsFalse(_ground.Draw(), MSG_INFO("Could not draw ground."))) return false;

		_groundShader.End();

		if (IsFalse(CheckOglError(), MSG_INFO("OpenGL Error."))) return false;
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
	if (IsNull(_noiseTexture, MSG_INFO("Could not create OGL texture."))) return false;

	const int width = 256;
	const int height = 256;

	static_assert(width > 0, "Illegal value for width");
	static_assert(height > 0, "Illegal value for width");

	// TODO: change to one-component bitmap
	const int components = 4;

	// allocate data
	const int dataCnt = width * height * components;
	GLubyte * const data = new GLubyte[dataCnt];

	if (IsNullptr(data, MSG_INFO("Could not allocate memory for noise texture."))) return false;

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

	if (IsFalse(CheckOglError(), MSG_INFO("OpenGL Error."))) return false;

	return true;
}


