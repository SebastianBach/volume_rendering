#ifndef VOLUME_DEMO_PROGRAM_H__
#define VOLUME_DEMO_PROGRAM_H__

#include <string>
#include "glm.hpp"
#include "glad/glad.h"

class ShaderProgram
{
public:
	ShaderProgram();
	~ShaderProgram();

	//---------------------------------------------------------------------------
	/// Prepares the shader program.
	/// @return				False if an error occurred.
	//---------------------------------------------------------------------------
	bool Init();

	//---------------------------------------------------------------------------
	/// Loads the vertex shader from the given file.
	/// @return				False if an error occurred.
	//---------------------------------------------------------------------------
	bool LoadVertexShader(const char* filename);

	//---------------------------------------------------------------------------
	/// Loads the fragment shader combinding the given files.
	/// @return				False if an error occurred.
	//---------------------------------------------------------------------------
	bool LoadFragmentShader(const char* head, const char* body);

	//---------------------------------------------------------------------------
	/// Loads the fragment shader from the given file.
	/// @return				False if an error occurred.
	//---------------------------------------------------------------------------
	bool LoadFragmentShader(const char* filename);

	//---------------------------------------------------------------------------
	/// Links the shader program.
	/// @return				False if an error occurred.
	//---------------------------------------------------------------------------
	bool Link();


	bool SetUniform(const char* name, const glm::mat4 & m);
	bool SetUniform(const char* name, const glm::vec3 & v);
	bool SetUniform(const char* name, const glm::float32& v);
	bool SetUniform(const char* name, unsigned int v);
	bool SetUniform(const char* name, const glm::vec3* const v, int count);

	//---------------------------------------------------------------------------
	/// Uses the shader.
	/// @return				False if an error occurred.
	//---------------------------------------------------------------------------
	bool Use() const;

	//---------------------------------------------------------------------------
	/// Stops using any shader.
	/// @return				False if an error occurred.
	//---------------------------------------------------------------------------
	void End() const;

private:

	GLint GetUniformLocation(const char* name);
	bool MakeShader(unsigned int type, const std::string& text, unsigned int& store);

private:

	bool _isLinked;
	unsigned int _program;
	unsigned int _vertexShader;
	unsigned int _fragmentShader;
};


#endif
