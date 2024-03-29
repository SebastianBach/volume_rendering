#include "renderengine.h"

#include "modeling.h"
#include "log.h"
#include <glm/gtc/matrix_transform.hpp>
#include <gl/GLU.h>
#include <glm/gtx/color_space.hpp>

// z-position of all objects
static constexpr auto Z_POS = -1.0f;

/// Maximum number of objects.
static constexpr auto MAX_OBJECT_COUNT = 18;

ObjectArray::ObjectArray()
{
    _count        = 0;
    _countChanged = false;
    _userObject   = {};

    // see https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glGet.xhtml
    static_assert(MAX_OBJECT_COUNT <= 256,
                  "Only array size of 256 is guaranteed.");
}

ObjectArray::~ObjectArray() = default;

bool ObjectArray::AddObject(glm::vec3& pos, glm::vec3& color, int& index)
{
    // check
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
    // check
    if (_count == MAX_OBJECT_COUNT)
        return false;

    glm::vec3 null{0.0};

    _pos.push_back(null);
    _colors.push_back(null);

    _count++;
    _countChanged = true;

    return true;
}

unsigned int ObjectArray::GetObjectCount() const
{
    return _count;
}

bool ObjectArray::SetDynamicObject(float x, float y)
{
    _userObject.x = x;
    _userObject.y = y;
    _userObject.z = Z_POS;

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

int ObjectArray::GetDataSize() const
{
    return _count * 3;
}

void ObjectArray::Animation(float step)
{
    if (_count < 2)
        return;

    auto       hue     = 180.0f;
    const auto hueStep = 360.0f / (float(_count));

    for (int i = 0; i < _count; ++i)
    {
        // colors
        if (_countChanged)
        {
            // calculate color wheel
            const float     h = fmod(hue, 360.0f);
            const glm::vec3 hsv(h, 1.0, 1.0);
            const glm::vec3 rgb = glm::rgbColor(hsv);
            _colors[i]          = rgb;

            hue += hueStep;
        }

        const auto currentPos = _pos[i];
        const auto distance   = _userObject - currentPos;

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
            auto scale = (6.28f / (float(_count))) * float(i);

            auto offset = (step * .01f) + scale;
            auto x      = sin(offset) * 2.0f;
            auto y      = (cos(offset) * 0.7f) + 0.25f;

            glm::vec3 targetPos;
            targetPos.x = x;
            targetPos.y = y;
            targetPos.z = Z_POS;

            movement = targetPos - currentPos;

            movement.x *= 0.1f;
            movement.y *= 0.1f;
            movement.z *= 0.1f;
        }

        const auto newPos = currentPos + movement;

        _pos[i] = newPos;
    }

    if (_countChanged)
        _countChanged = false;
}

template <class... Args>
static auto SetUniform(ShaderProgram& prog, const char* name, Args... args)
{
    const auto uniformResult = prog.SetUniform(name, (args)...);

    auto ComposeErrorMessage = [name]() -> std::string
    {
        std::string message("Failed to set uniform variable: ");
        message.append(name);
        return message;
    };

    if (IsFalse(uniformResult, MSG_INFO(ComposeErrorMessage())))
        return false;

    return true;
}

template <typename F> static auto OglError(const char*, F&& f)
{
    const auto error = glGetError();

    if (error != GL_NO_ERROR)
    {
        error_sys_intern::ErrorInfo info;
        f(info);

        std::string errorStr;
        errorStr.append(info._msgStr);

        const auto  gl_error = glGetError();
        const auto* err      = gluErrorString(gl_error);

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
    _step         = 0.0;
    _settings     = {};
}

RenderEngine::~RenderEngine() = default;

bool RenderEngine::Init()
{
    if (IsNull(gladLoadGL(), MSG_INFO("Could not load OGL functions.")))
        return false;

    // check OGL version / extensions
    DataMessage(MSG_INFO("Vendor:"));
    DataMessage(MSG_INFO((const char*)glGetString(GL_VENDOR)));
    DataMessage(MSG_INFO(":"));
    DataMessage(MSG_INFO((const char*)glGetString(GL_RENDERER)));
    DataMessage(MSG_INFO("GL Version:"));
    DataMessage(MSG_INFO((const char*)glGetString(GL_VERSION)));
    DataMessage(MSG_INFO("GLSL Version:"));
    DataMessage(
        MSG_INFO((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION)));

    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, 1280, 720);

    if (OglError(MSG_INFO("OGL Init failed.")))
        return false;

    return true;
}

#include <cmath>
#include <iostream>

bool RenderEngine::CreateScene()
{
    // geometry
    if (IsFalse(CreatePlane(_viewPlane),
                MSG_INFO("Could not create view plane.")))
        return false;
    if (IsFalse(CreatePlane(_ground),
                MSG_INFO("Could not create ground plane.")))
        return false;

    if (OglError(MSG_INFO("Geometry creation failed.")))
        return false;

    // view shader
    if (IsFalse(_shader.Init(), MSG_INFO("Shader setup failed.")))
        return false;

    const auto fragmentShader = _shader.LoadFragmentShader(
        "shader/fragment_head.glsl", "shader/volume_body.glsl");
    if (IsFalse(fragmentShader, MSG_INFO("Could not load fragment shader.")))
        return false;
    const auto vertexShader = _shader.LoadVertexShader("shader/vertex.glsl");
    if (IsFalse(vertexShader, MSG_INFO("Could not load vertex shader.")))
        return false;
    if (IsFalse(_shader.Link(), MSG_INFO("Could not link shader.")))
        return false;

    if (OglError(MSG_INFO("Shader creation failed.")))
        return false;

    // ground shader
    if (IsFalse(_groundShader.Init(), MSG_INFO("Shader setup failed.")))
        return false;
    if (IsFalse(_groundShader.LoadFragmentShader("shader/fragment_head.glsl",
                                                 "shader/ground_body.glsl"),
                MSG_INFO("Could not load fragment shader.")))
        return false;
    if (IsFalse(_groundShader.LoadVertexShader("shader/vertex.glsl"),
                MSG_INFO("Could not load vertex shader.")))
        return false;
    if (IsFalse(_groundShader.Link(), MSG_INFO("Could not link shader.")))
        return false;

    if (OglError(MSG_INFO("Ground shader creation failed.")))
        return false;

    // noise texture

    if (IsFalse(CreateNoiseTexture(),
                MSG_INFO("Could not create noise texture.")))
        return false;
    if (OglError(MSG_INFO("Texture creation failed.")))
        return false;

    // create six objects
    const auto startCount = 6;
    for (auto i = 0; i < startCount; ++i)
    {
        if (IsFalse(_objects.AddObject(), MSG_INFO("Could not add object.")))
            return false;
    }

    // define standard matrices
    const auto camPos = glm::vec3(0, 0, 2);
    const auto viewMatrix =
        glm::lookAt(camPos, glm::vec3(0, 0, 0), glm::vec3(0.0f, 1.0f, 0.0f));

    const auto projectionMatrix =
        glm::perspectiveFov(1.0f, 1280.0f, 720.0f, 0.1f, 5.0f);

    auto viewPlaneModelMatrix = glm::mat4(1.0f);
    viewPlaneModelMatrix =
        glm::translate(viewPlaneModelMatrix, glm::vec3(-2, -0.75, 0));
    viewPlaneModelMatrix = glm::scale(viewPlaneModelMatrix, glm::vec3(4, 2, 2));

    auto groundPlaneModelMatrix = glm::mat4(1.0f);
    groundPlaneModelMatrix =
        glm::translate(groundPlaneModelMatrix, glm::vec3(-3, -1.5, -2));
    groundPlaneModelMatrix =
        glm::scale(groundPlaneModelMatrix, glm::vec3(10, 2, 2));
    groundPlaneModelMatrix = glm::rotate(
        groundPlaneModelMatrix, glm::radians(90.0f), glm::vec3(1, 0, 0));

    {
        if (IsFalse(_shader.Use(), MSG_INFO("Could not use shader.")))
            return false;

        const auto mv  = viewMatrix * viewPlaneModelMatrix;
        const auto MVP = projectionMatrix * mv;

        if (!SetUniform(_shader, "u_mvp", MVP))
            return false;
        if (!SetUniform(_shader, "u_modelMatrix", viewPlaneModelMatrix))
            return false;
        if (!SetUniform(_shader, "u_camPos", camPos))
            return false;
        if (!SetUniform(_shader, "u_noiseTexture", 0u))
            return false;

        ShaderProgram::End();
    }

    {
        // draw ground plane
        const glm::mat4 MVground  = viewMatrix * groundPlaneModelMatrix;
        const glm::mat4 MVPground = projectionMatrix * MVground;

        if (IsFalse(_groundShader.Use(),
                    MSG_INFO("Could not enable ground shader")))
            return false;
        if (IsFalse(_groundShader.SetUniform("u_mvp", MVPground),
                    MSG_INFO("Could not set MVP.")))
            return false;
        if (IsFalse(_groundShader.SetUniform("u_modelMatrix",
                                             groundPlaneModelMatrix),
                    MSG_INFO("Could not set model matrix.")))
            return false;
        if (!SetUniform(_groundShader, "u_camPos", camPos))
            return false;

        ShaderProgram::End();
    }

    InfoMessage(MSG_INFO(("Scene setup done...")));

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

    _objects.SetDynamicObject(settings._dynamicObjectX,
                              settings._dynamicObjectY);

    if (settings._removeObject)
        _objects.RemoveLastObject();

    if (settings._addObject)
        _objects.AddObject();

    // add object from mouse click
    if (settings._addObjectClick)
    {
        if (_objects.GetObjectCount() >= MAX_OBJECT_COUNT)
            return;

        glm::vec3 pos;
        pos.x = settings._dynamicObjectX;
        pos.y = settings._dynamicObjectY;
        pos.z = Z_POS;

        // color will be set in Animation()
        glm::vec3 color{0.0};
        auto      index = 0;

        if (IsFalse(_objects.AddObject(pos, color, index),
                    MSG_INFO("Could not add object")))
            return;
    }

    _objects.Animation(_step);
}

#include <iostream>

bool RenderEngine::Render()
{
    // set up buffers
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // enable alpha handling
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const auto* posData     = _objects.GetPositionData();
    const auto* colorData   = _objects.GetColorData();
    const auto  posDataSize = _objects.GetDataSize();
    const auto  objectCnt   = _objects.GetObjectCount();

    {
        if (IsFalse(_shader.Use(), MSG_INFO("Could not use shader.")))
            return false;

        if (!SetUniform(_shader, "u_shadingMode", _settings._renderMode))
            return false;
        if (!SetUniform(_shader, "u_animation", _step))
            return false;
        if (!SetUniform(_shader, "u_noise", _settings.GetNoise()))
            return false;
        if (!SetUniform(_shader, "u_objectCnt", objectCnt))
            return false;
        if (!SetUniform(_shader, "u_objectPos", posData, posDataSize))
            return false;
        if (!SetUniform(_shader, "u_objectColor", colorData, posDataSize))
            return false;

        if (IsFalse(_viewPlane.Draw(), MSG_INFO("Could not draw view plane.")))
            return false;

        ShaderProgram::End();
    }

    {
        if (IsFalse(_groundShader.Use(),
                    MSG_INFO("Could not enable ground shader")))
            return false;

        if (!SetUniform(_groundShader, "u_animation", _step))
            return false;
        if (!SetUniform(_groundShader, "u_shadingMode", _settings._renderMode))
            return false;
        if (!SetUniform(_groundShader, "u_objectPos", posData, posDataSize))
            return false;
        if (!SetUniform(_groundShader, "u_objectColor", colorData, posDataSize))
            return false;
        if (!SetUniform(_groundShader, "u_objectCnt", objectCnt))
            return false;
        if (!SetUniform(_groundShader, "u_noise", _settings.GetNoise()))
            return false;

        if (IsFalse(_ground.Draw(), MSG_INFO("Could not draw ground.")))
            return false;

        ShaderProgram::End();
    }

    if (OglError(MSG_INFO("Rendering failed.")))
        return false;

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
    D. Wolff, OpenGL 4 shading language cookbook, Birmingham: Packt Publishing,
    2013.

    Chapter "Creating a noise texture using GLM"
    */

    glGenTextures(1, &_noiseTexture);
    if (IsNull(_noiseTexture, MSG_INFO("Could not create OGL texture.")))
        return false;

    const auto width  = 256;
    const auto height = 256;

    static_assert(width > 0, "Illegal value for width");
    static_assert(height > 0, "Illegal value for width");

    // TODO: change to one-component bitmap
    const auto components = 4;

    // allocate data
    const auto dataCnt = width * height * components;
    auto*      data    = new GLubyte[dataCnt];

    if (IsNullptr(data,
                  MSG_INFO("Could not allocate memory for noise texture.")))
        return false;

    const auto widthF  = float(width);
    const auto heightF = float(height);
    const auto scale   = 6.5f;

    for (auto y = 0; y < height; ++y)
    {
        const auto yf = float(y) / heightF;

        for (auto x = 0; x < width; ++x)
        {
            const auto xf = float(x) / widthF;

            // todo: we could also store different noises in different channels
            const glm::vec2 pos{xf * scale, yf * scale};
            const auto      noise   = glm::perlin(pos);
            const auto      result  = (noise + 1.0f) * .5f;
            const auto      byteRes = GLubyte(result * 255.9f);

            const auto componentOffset = (y * width + x) * components;

            for (auto c = 0; c < components; ++c)
            {
                const auto componentIndex = componentOffset + c;
                data[componentIndex]      = byteRes;
            }
        }
    }

    // setup OpenGL texture and bind to texture unit 0

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _noiseTexture);
    // set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    // copy texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);

    delete[] data;

    return true;
}
