#include "program.h"

#include "log.h"
#include <string>
#include <iostream>
#include <fstream>
#include <iosfwd>

#include "glad/glad.h"
#include <string>

#include <glm/gtc/type_ptr.hpp>

// glGetUniformLocation returns -1 if the given name could not be found
// see https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glGetUniformLocation.xhtml
static const GLint LOCATION_FAIL = -1;

//---------------------------------------------------------------------------
/// Utlity function to load a file.
/// @param[in]	filename		The file location.
/// @param[out]	text			The string to contain the file content.
/// @return						False if an error occured.
//---------------------------------------------------------------------------
static bool LoadFile(const char* filename, std::string& text)
{
	if (IsNullptr(filename, MSG_INFO("Invalid filename argument"))) return false;

	std::ifstream fp;

	fp.open(filename, std::ios_base::in);

	if (fp)
	{
		std::string line;
		while (std::getline(fp, line))
		{
			text.append(line);
			text.append("\r\n");
		}
	}
	else
	{
		DataMessage(MSG_INFO(filename));
		ErrorMessage(MSG_INFO("Could not open file."));
		return false;
	}

	return true;
}

//---------------------------------------------------------------------------
/// Utility function that returns an error message.
/// @param[in]	name	The name of an uiform variable.
/// @return				The formatted error string.
//---------------------------------------------------------------------------
static std::string GetUniformErrorString(const char* name)
{
	std::string str;
	str.append("Could not access uniform location \"");
	str.append(name);
	str.append("\"");

	return str;
}

ShaderProgram::ShaderProgram()
{
	_program = 0;
	_isLinked = false;
	_vertexShader = 0;
	_fragmentShader = 0;
}

ShaderProgram::~ShaderProgram()
{

}

bool ShaderProgram::Init()
{
	if (IsNotValue(_program, 0U, MSG_INFO("Programm already created."))) return false;

	_program = glCreateProgram();

	if (IsNull(_program, MSG_INFO("Could not create program."))) return false;

	return true;
}

bool ShaderProgram::LoadVertexShader(const char* filename)
{
	if (IsNullptr(filename, MSG_INFO("Illegal filename argument."))) return false;
	if (IsNull(_program, MSG_INFO("Program not set."))) return false;

	std::string text;

	if (IsFalse(LoadFile(filename, text), MSG_INFO("Could not load file.")))
		return false;

	const bool res = MakeShader(GL_VERTEX_SHADER, text, _vertexShader);
	if (IsFalse(res, MSG_INFO("Could not make vertex shader.")))
		return false;

	glAttachShader(_program, _vertexShader);

	return true;
}

bool ShaderProgram::LoadFragmentShader(const char* head, const char* body)
{
	if (IsNullptr(head, MSG_INFO("No head file set."))) return false;
	if (IsNullptr(body, MSG_INFO("No body file set."))) return false;
	if (IsNull(_program, MSG_INFO("Program not set."))) return false;

	std::string headText;

	if (IsFalse(LoadFile(head, headText), MSG_INFO("Could not load head file.")))
		return false;

	std::string bodyText;

	if (IsFalse(LoadFile(body, bodyText), MSG_INFO("Could not load body file.")))
		return false;

	const std::string fullText = headText + bodyText;
	
	const bool res = MakeShader(GL_FRAGMENT_SHADER, fullText, _fragmentShader);
	if (IsFalse(res, MSG_INFO("Could not make fragment shader.")))
		return false;

	glAttachShader(_program, _fragmentShader);

	return true;
}

bool ShaderProgram::LoadFragmentShader(const char* filename)
{
	if (IsNullptr(filename, MSG_INFO("Invalid filename argument."))) return false;
	if (IsNull(_program, MSG_INFO("Program not set."))) return false;

	std::string text;

	if (IsFalse(LoadFile(filename, text), MSG_INFO("Could not load File.")))
		return false;

	const bool res = MakeShader(GL_FRAGMENT_SHADER, text, _fragmentShader);
	
	if (IsFalse(res, MSG_INFO("Could make fragment shader.")))
		return false;

	glAttachShader(_program, _fragmentShader);

	return true;
}

bool ShaderProgram::MakeShader(unsigned int type, const std::string& text, unsigned int& store)
{
	if (IsNull(text.size(), MSG_INFO("Shader source is empty."))) return false;

	// create new shader
	const GLuint shader = glCreateShader(type);
	if (IsNull(shader, MSG_INFO("Could not create shader."))) return false;

	// copy shader source code
	const char * textPtr = text.c_str();
	glShaderSource(shader, 1, &textPtr, NULL);

	// compile
	glCompileShader(shader);

	//check whether the shader loads fine
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE) 
	{
		// get size of error message
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		// allocate memory for error message
		GLchar * const infoLog = new GLchar[infoLogLength];
		if (IsNullptr(infoLog, MSG_INFO("Could not allocate memory for error message."))) return false;
		// get error message
		glGetShaderInfoLog(shader, infoLogLength, NULL, infoLog);

		// print error message
		ErrorMessage(MSG_INFO(infoLog));

		// free error message memory
		delete[] infoLog;

		return false;
	}

	// store shader ID on success
	store = shader;

	return true;
}

bool ShaderProgram::Link()
{
	if (IsNull(_program, MSG_INFO("Program not set."))) return false;
	if (IsNotValue(_isLinked, false, MSG_INFO("Programm already linked."))) return false;

	// link program
	glLinkProgram(_program);

	// get link status
	int status = 0;
	glGetProgramiv(_program, GL_LINK_STATUS, &status);

	// check link status
	if (IsValue(status, GL_FALSE, MSG_INFO("Could not link program."))) return false;

	_isLinked = true;

	return true;
}

bool ShaderProgram::SetUniform(const char *name, const glm::mat4 & m)
{
	if (IsNull(_program, MSG_INFO("Program not set."))) return false;

	const GLint location = GetUniformLocation(name);
	if (IsValue(location, LOCATION_FAIL, MSG_INFO(GetUniformErrorString(name).c_str()))) return false;

	glUniformMatrix4fv(location, 1, GL_FALSE, &m[0][0]);

	return true;
}

bool ShaderProgram::SetUniform(const char* name, const glm::vec3 & v)
{
	if (IsNull(_program, MSG_INFO("Program not set."))) return false;

	const GLint location = GetUniformLocation(name);
	if (IsValue(location, LOCATION_FAIL, MSG_INFO(GetUniformErrorString(name).c_str()))) return false;

	glUniform3fv(location, 1, &v[0]);

	return true;
}

bool ShaderProgram::SetUniform(const char* name, const glm::float32& v)
{
	if (IsNull(_program, MSG_INFO("Program not set."))) return false;

	const GLint location = GetUniformLocation(name);
	if (IsValue(location, LOCATION_FAIL, MSG_INFO(GetUniformErrorString(name).c_str()))) return false;

	glUniform1f(location, v);

	return true;
}

bool ShaderProgram::SetUniform(const char* name, unsigned int v)
{
	if (IsNull(_program, MSG_INFO("Program not set."))) return false;

	const GLint location = GetUniformLocation(name);
	if (IsValue(location, LOCATION_FAIL, MSG_INFO(GetUniformErrorString(name).c_str()))) return false;

	glUniform1i(location, v);
	return true;
}

bool ShaderProgram::SetUniform(const char* name, const glm::vec3* const v, int count)
{
	if (IsNull(_program, MSG_INFO("Program not set."))) return false;
	if (IsNullptr(v, MSG_INFO("Invalid argument."))) return false;
	if (IsNull(count, MSG_INFO("Invalid argument count."))) return false;

	const GLint location = GetUniformLocation(name);
	if (IsValue(location, LOCATION_FAIL, MSG_INFO(GetUniformErrorString(name).c_str()))) return false;

	glUniform3fv(location, count, glm::value_ptr(v[0]));
	return true;
}

GLint ShaderProgram::GetUniformLocation(const char* name)
{
	return glGetUniformLocation(_program, name);
}

bool ShaderProgram::Use() const
{
	if (IsNull(_program, MSG_INFO("Program not set."))) return false;
	if (IsFalse(_isLinked, MSG_INFO("Program not linked."))) return false;

	glUseProgram(_program);

	return true;
}

void ShaderProgram::End() const
{
	glUseProgram(0);
}
