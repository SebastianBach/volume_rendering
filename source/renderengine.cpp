#include "renderengine.h"

// see https://github.com/Dav1dde/glad
#include <glad/glad.h>

#include "log.h"
#include <string>
#include <cstdlib>
#include <gl/GLU.h>

#include "modeling.h"

#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/color_space.hpp>

ObjectArray::ObjectArray()
{
	_count = 0;
	_countChanged = false;
}

ObjectArray::~ObjectArray()
{

}

bool ObjectArray::AddObject(glm::vec3& pos, glm::vec3& color, int& index)
{
	if (_count == MAX_OBJECT_COUNT)
		return false;

	_pos.push_back(pos);
	_colors.push_back(color);

	index = _count;
	_count++;

	_countChanged = true;

	return true;
}

bool ObjectArray::AddObject()
{
	if (_count == MAX_OBJECT_COUNT)
		return false;

	glm::vec3 null(0.0);

	_pos.push_back(null);
	_colors.push_back(null);

	_count++;

	_countChanged = true;

	return true;
}

unsigned int  ObjectArray::GetObjectCount()
{
	return _count;
}

bool ObjectArray::SetObjectPos(int index, glm::vec3& pos)
{
	if (IsFalse(IsIndexOK(index), MSG_INFO("Index out of range")))
		return false;

	_pos[index] = pos;

	return true;
}

bool ObjectArray::SetDynamicObject(float x, float y)
{
	if (_count == 0)
		return false;

	glm::vec3 pos;
	pos.x = x;
	pos.y = y;
	pos.z = -1;

	_userObject = pos;

	return true;
}

bool ObjectArray::RemoveLastObject()
{
	if (_count <= 1)
		return false;

	_pos.pop_back();
	_colors.pop_back();
	_count--;

	_countChanged = true;

	return true;
}

glm::vec3* ObjectArray::GetPositionData()
{
	return &_pos.front();
}

glm::vec3* ObjectArray::GetColorData()
{
	return &_colors.front();
}

int ObjectArray::GetDataSize()
{
	return _count * 3;
}

void ObjectArray::Animation(float step)
{
	if (_count < 2)
		return;

	float hue = 180.0f;
	float hueStep = 360.0f / (float(_count));

	for (int i = 0; i < _count; ++i)
	{
		// colors
		if (_countChanged)
		{
			const float h = fmod(hue, 360.0f);
			const glm::vec3 hsv(h, 1.0, 1.0);
			const glm::vec3 rgb = glm::rgbColor(hsv);
			_colors[i] = rgb;

			hue += hueStep;
		}


		const glm::vec3 currentPos = _pos[i];
		const glm::vec3 distance = _userObject - currentPos;

		glm::vec3 movement(0.0);

		if (glm::length(distance) < 0.2)
		{
			movement = distance;
			movement.x *= 0.5f;
			movement.y *= 0.5f;
			movement.z *= 0.5f;
		}
		else
		{
			float scale = (6.28f / (float(_count))) * float(i);

			float offset = (step*.01f) + scale;
			float x = sin(offset) * 2.0f;
			float y = (cos(offset) * 0.7f) + 0.25f;

			glm::vec3 targetPos;
			targetPos.x = x;
			targetPos.y = y;
			targetPos.z = -1.0;

			movement = targetPos - currentPos;

			movement.x *= 0.1f;
			movement.y *= 0.1f;
			movement.z *= 0.1f;
		}

		const glm::vec3 newPos = currentPos + movement;

		_pos[i] = newPos;
	}

	if(_countChanged)
		_countChanged = false;
}

bool ObjectArray::IsIndexOK(int index)
{
	if (index >= _count)
	{
		return false;
	}

	return true;
}


template<class ...Args> static bool SetUniform(ShaderProgram& prog, const char* name, Args... args)
{
	 const bool uniformResult = prog.SetUniform(name, (args)...);

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


template<typename F> static bool OglError(const char* function, F && f)
{
	const auto error = glGetError();

	if (error != GL_NO_ERROR)
	{
		error_sys_intern::ErrorInfo info;
		f(info);

		std::string errorStr;
		errorStr.append(info._msgStr);

		const auto error = glGetError();
		const unsigned char* err = gluErrorString(error);

		errorStr.append(" : ");
		errorStr.append((const char*)err);

		ErrorMessage(MSG_INFO(errorStr));
		return true;
	}

	return false;
}

RenderEngine::RenderEngine()
{
	_noiseTexture = 0;
	_step = 0.0;
}

RenderEngine::~RenderEngine()
{

}

bool RenderEngine::Init()
{
	if (IsNull(gladLoadGL(), MSG_INFO("Could not load OGL functions."))) return false;

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

	if (OglError(MSG_INFO("OGL Init failed."))) return false;

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
	// geometry
	if (IsFalse(CreatePlane(_viewPlane), MSG_INFO("Could not create view plane."))) return false;
	if (IsFalse(CreatePlane(_ground), MSG_INFO("Could not create ground plane."))) return false;

	if (OglError(MSG_INFO("Geometry creation failed."))) return false;

	// view shader
	if (IsFalse(_shader.Init(), MSG_INFO("Shader setup failed."))) return false;

	const bool fragmentShader = _shader.LoadFragmentShader("shader/fragment_head.glsl", "shader/volume_body.glsl");
	if (IsFalse(fragmentShader, MSG_INFO("Could not load fragment shader."))) return false;
	const bool vertexShader = _shader.LoadVertexShader("shader/vertex.glsl");
	if (IsFalse(vertexShader, MSG_INFO("Could not load vertex shader."))) return false;
	if (IsFalse(_shader.Link(), MSG_INFO("Could not link shader."))) return false;

	if (OglError(MSG_INFO("Shader creation failed."))) return false;

	// ground shader
	if (IsFalse(_groundShader.Init(), MSG_INFO("Shader setup failed."))) return false;
	if (IsFalse(_groundShader.LoadFragmentShader("shader/fragment_head.glsl", "shader/ground_body.glsl"), MSG_INFO("Could not load fragment shader."))) return false;
	if (IsFalse(_groundShader.LoadVertexShader("shader/vertex.glsl"), MSG_INFO("Could not load vertex shader."))) return false;
	if (IsFalse(_groundShader.Link(), MSG_INFO("Could not link shader."))) return false;

	if (OglError(MSG_INFO("Ground shader creation failed."))) return false;

	// noise texture

	if (IsFalse(CreateNoiseTexture(), MSG_INFO("Could not create noise texture."))) return false;
	if (OglError(MSG_INFO("Texture creation failed."))) return false;

	// create six objects
	_objects.AddObject();
	_objects.AddObject();
	_objects.AddObject();
	_objects.AddObject();
	_objects.AddObject();
	_objects.AddObject();


	// define standard matrcies
	_camPos = glm::vec3(0, 0, 2);
	_viewMatrix = glm::lookAt(_camPos, glm::vec3(0, 0, 0), glm::vec3(0.0f, 1.0f, 0.0f));

	_projectionMatrix = glm::perspectiveFov(1.0f, 1280.0f, 720.0f, 0.1f, 5.0f);

	_viewPlaneModelMatrix = glm::mat4(1.0f);
	_viewPlaneModelMatrix = glm::translate(_viewPlaneModelMatrix, glm::vec3(-2, -0.75, 0));
	_viewPlaneModelMatrix = glm::scale(_viewPlaneModelMatrix, glm::vec3(4, 2, 2));


	_groundPlaneModelMatrix = glm::mat4(1.0f);
	_groundPlaneModelMatrix = glm::translate(_groundPlaneModelMatrix, glm::vec3(-3, -1.5, -2));
	_groundPlaneModelMatrix = glm::scale(_groundPlaneModelMatrix, glm::vec3(10, 2, 2));
	_groundPlaneModelMatrix = glm::rotate(_groundPlaneModelMatrix, glm::radians(90.0f), glm::vec3(1, 0, 0));

	{
		if (IsFalse(_shader.Use(), MSG_INFO("Could not use shader."))) return false;

		const glm::mat4 mv = _viewMatrix * _viewPlaneModelMatrix;
		const glm::mat4 MVP = _projectionMatrix * mv;

		if (!SetUniform(_shader, "u_mvp", MVP)) return false;
		if (!SetUniform(_shader, "u_modelMatrix", _viewPlaneModelMatrix)) return false;
		if (!SetUniform(_shader, "u_camPos", _camPos)) return false;
		if (!SetUniform(_shader, "u_noiseTexture", unsigned int(0))) return false;

		_shader.End();
	}

	{
		// draw ground plane
		const glm::mat4 MVground = _viewMatrix * _groundPlaneModelMatrix;
		const glm::mat4 MVPground = _projectionMatrix * MVground;

		if (IsFalse(_groundShader.Use(), MSG_INFO("Could not enable ground shader"))) return false;
		if (IsFalse(_groundShader.SetUniform("u_mvp", MVPground), MSG_INFO("Could not set MVP."))) return false;
		if (IsFalse(_groundShader.SetUniform("u_modelMatrix", _groundPlaneModelMatrix), MSG_INFO("Could not set model matrix."))) return false;
		if (!SetUniform(_groundShader, "u_camPos", _camPos)) return false;

		_groundShader.End();
	}



	DataMessage(MSG_INFO(("Scene setup done...")));

	return true;
}

void RenderEngine::UpdateScene(const SceneSettings& settings)
{
	_settings = settings;

	if (settings._timeStep)
	{
		_step = _step + 1.0f;
	}

	_step = _step + _settings._timeOff;

	_objects.Animation(_step);

	_objects.SetDynamicObject(settings._dynamicObjectX, settings._dynamicObjectY);

	if (settings._removeObject)
		_objects.RemoveLastObject();

	if (settings._addObject)
		_objects.AddObject();

	if (settings._addSphere)
	{
		if (_objects.GetObjectCount() >= MAX_OBJECT_COUNT)
			return;

		glm::vec3 pos;
		pos.x = settings._dynamicObjectX;
		pos.y = settings._dynamicObjectY;
		pos.z = -1.0;

		glm::vec3 color;
		color.x = 1.0;
		color.y = 0.0;
		color.z = 1.0;

		int index = 0;
		if (IsFalse(_objects.AddObject(pos, color, index), MSG_INFO("Could not add object")))
			return;
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

	const glm::vec3* const posData = _objects.GetPositionData();
	const glm::vec3* const colorData = _objects.GetColorData();
	const int posDataSize = _objects.GetDataSize();
	const unsigned int objectCnt = _objects.GetObjectCount();

	// todo: make sub-function
	{
		if (IsFalse(_shader.Use(), MSG_INFO("Could not use shader."))) return false;

		if (!SetUniform(_shader, "u_shadingMode", _settings._renderMode)) return false;
		if (!SetUniform(_shader, "u_animation", _step)) return false;
		if (!SetUniform(_shader, "u_noise", _settings._noise)) return false;
		if (!SetUniform(_shader, "u_objectCnt", objectCnt)) return false;
		if (!SetUniform(_shader, "u_objectPos", posData, posDataSize)) return false;
		if (!SetUniform(_shader, "u_objectColor", colorData, posDataSize)) return false;

		if (IsFalse(_viewPlane.Draw(), MSG_INFO("Could not draw view plane."))) return false;

		_shader.End();
	}
	
	// todo: make sub-function
	{
		if (IsFalse(_groundShader.Use(), MSG_INFO("Could not enable ground shader"))) return false;
		if (!SetUniform(_groundShader, "u_animation", _step)) return false;
		if (!SetUniform(_groundShader, "u_shadingMode", _settings._renderMode)) return false;	
		if (!SetUniform(_groundShader, "u_objectPos", posData, posDataSize)) return false;
		if (!SetUniform(_groundShader, "u_objectColor", colorData, posDataSize)) return false;
		if (!SetUniform(_groundShader, "u_objectCnt", objectCnt)) return false;
		if (!SetUniform(_groundShader, "u_noise", _settings._noise)) return false;

		if (IsFalse(_ground.Draw(), MSG_INFO("Could not draw ground."))) return false;

		_groundShader.End();
	}

	if (OglError(MSG_INFO("Rendering failed."))) return false;

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

	return true;
}


