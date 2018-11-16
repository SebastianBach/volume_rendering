#include "program.h"

#include "log.h"
#include <string>
#include <iostream>
#include <fstream>
#include <iosfwd>

#include "glad/glad.h"
#include <string>

// glGetUniformLocation returns -1 if the given name could not be found
// see https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glGetUniformLocation.xhtml
#define LOCATION_FAIL -1


int ShaderProgram::GetUniformLocation(const char * name)
{
	return glGetUniformLocation(_program, name);
}

static bool LoadFile(const char* filename, std::string& text)
{
	if (IsNullptr(filename, "Invalid filename argument", ERROR_CONTEXT)) return false;

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
		DATA_MSG(filename);
		ERROR_MSG("Could not open file.");
		return false;
	}

	return true;
}

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
}

ShaderProgram::~ShaderProgram()
{

}

bool ShaderProgram::Init()
{
	if (IsNotValue(_program, 0U, "Programm already created.", ERROR_CONTEXT)) return false;

	_program = glCreateProgram();

	if (IsNull(_program, "Could not create program.", ERROR_CONTEXT)) return false;

	return true;
}

bool ShaderProgram::LoadVertexShader(const char* filename)
{
	if (IsNullptr(filename, "Illegal filename argument.", ERROR_CONTEXT)) return false;
	if (IsNull(_program, "Program not set.", ERROR_CONTEXT)) return false;

	std::string text;

	if (IsFalse(LoadFile(filename, text), "Could not load file.", ERROR_CONTEXT))
		return false;

	const bool res = MakeShader(GL_VERTEX_SHADER, text, _vertexShader);
	if (IsFalse(res, "Could not make shader.", ERROR_CONTEXT))
		return false;

	glAttachShader(_program, _vertexShader);

	return true;
}

bool ShaderProgram::LoadFragmentShader(const char* head, const char* body)
{
	if (IsNullptr(head, "No head file set.", ERROR_CONTEXT)) return false;
	if (IsNullptr(body, "No body file set.", ERROR_CONTEXT)) return false;
	if (IsNull(_program, "Program not set.", ERROR_CONTEXT)) return false;

	std::string headText;

	if (IsFalse(LoadFile(head, headText), "Could not load head file.", ERROR_CONTEXT))
		return false;

	std::string bodyText;

	if (IsFalse(LoadFile(body, bodyText), "Could not load body file.", ERROR_CONTEXT))
		return false;

	const std::string fullText = headText + bodyText;
	
	const bool res = MakeShader(GL_FRAGMENT_SHADER, fullText, _fragmentShader);
	if (IsFalse(res, "Could not make Fragment shader.", ERROR_CONTEXT))
		return false;

	glAttachShader(_program, _fragmentShader);

	return true;
}

bool ShaderProgram::LoadFragmentShader(const char* filename)
{
	if (IsNullptr(filename, "Invalid filename argument.", ERROR_CONTEXT)) return false;
	if (IsNull(_program, "Program not set.", ERROR_CONTEXT)) return false;

	std::string text;

	if (IsFalse(LoadFile(filename, text), "Could not load File.", ERROR_CONTEXT))
		return false;

	const bool res = MakeShader(GL_FRAGMENT_SHADER, text, _fragmentShader);
	
	if (IsFalse(res, "Could make shader.", ERROR_CONTEXT))
		return false;

	glAttachShader(_program, _fragmentShader);

	return true;
}

bool ShaderProgram::MakeShader(unsigned int type, const std::string& text, unsigned int& store)
{
	if (IsNull(text.size(), "Shader source is empty.", ERROR_CONTEXT)) return false;

	// create new shader
	const GLuint shader = glCreateShader(type);
	if (IsNull(shader, "Could not create shader.", ERROR_CONTEXT)) return false;

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
		GLchar *infoLog = new GLchar[infoLogLength];
		if (IsNullptr(infoLog, "Could not allocate memory for error message.", ERROR_CONTEXT)) return false;
		// get error message
		glGetShaderInfoLog(shader, infoLogLength, NULL, infoLog);

		// print shader source
		DATA_MSG(text.c_str());
		// print error message
		ERROR_MSG(infoLog);

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
	if (IsNull(_program, "Program not set.", ERROR_CONTEXT)) return false;
	if (IsNotValue(_isLinked, false, "Programm already linked.", ERROR_CONTEXT)) return false;

	// link program
	glLinkProgram(_program);

	// get link status
	int status = 0;
	glGetProgramiv(_program, GL_LINK_STATUS, &status);

	// check link status
	if (IsValue(status, GL_FALSE, "Could not link program.", ERROR_CONTEXT)) return false;

	_isLinked = true;

	return true;
}

bool ShaderProgram::SetUniform(const char *name, const glm::mat4 & m)
{
	if (IsNull(_program, "Program not set.", ERROR_CONTEXT)) return false;

	const GLint location = GetUniformLocation(name);
	if (IsValue(location, LOCATION_FAIL, GetUniformErrorString(name).c_str(), ERROR_CONTEXT)) return false;

	glUniformMatrix4fv(location, 1, GL_FALSE, &m[0][0]);

	return true;
}

bool ShaderProgram::SetUniform(const char* name, const glm::vec3 & v)
{
	if (IsNull(_program, "Program not set.", ERROR_CONTEXT)) return false;

	const GLint location = GetUniformLocation(name);
	if (IsValue(location, LOCATION_FAIL, GetUniformErrorString(name).c_str(), ERROR_CONTEXT)) return false;

	glUniform3fv(location, 1, &v[0]);

	return true;
}

bool ShaderProgram::SetUniform(const char* name, const glm::float32& v)
{
	if (IsNull(_program, "Program not set.", ERROR_CONTEXT)) return false;

	const GLint location = GetUniformLocation(name);
	if (IsValue(location, LOCATION_FAIL, GetUniformErrorString(name).c_str(), ERROR_CONTEXT)) return false;

	glUniform1f(location, v);

	return true;
}

bool ShaderProgram::SetUniform(const char* name, unsigned int v)
{
	if (IsNull(_program, "Program not set.", ERROR_CONTEXT)) return false;

	const GLint location = GetUniformLocation(name);
	if (IsValue(location, LOCATION_FAIL, GetUniformErrorString(name).c_str(), ERROR_CONTEXT)) return false;

	glUniform1i(location, v);
	return true;
}

bool ShaderProgram::Use() const
{
	if (IsNull(_program, "Program not set.", ERROR_CONTEXT)) return false;
	if (IsFalse(_isLinked, "Program not linked.", ERROR_CONTEXT)) return false;

	glUseProgram(_program);

	return true;
}

void ShaderProgram::End() const
{
	glUseProgram(0);
}
