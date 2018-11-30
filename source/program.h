#ifndef VOLUME_DEMO_PROGRAM_H__
#define VOLUME_DEMO_PROGRAM_H__

#include <string>
#include "glm.hpp"
#include "glad/glad.h"

//---------------------------------------------------------------------------
/// A ShaderProgram represents an OpenGL shader.
//---------------------------------------------------------------------------
class ShaderProgram
{
public:
	//---------------------------------------------------------------------------
	/// Constructor.
	//---------------------------------------------------------------------------
	ShaderProgram();

	//---------------------------------------------------------------------------
	/// Destructor.
	//---------------------------------------------------------------------------
	~ShaderProgram();

	//---------------------------------------------------------------------------
	/// Prepares the shader program.
	/// @return				False if an error occurred.
	//---------------------------------------------------------------------------
	bool Init();

	//---------------------------------------------------------------------------
	/// Loads the vertex shader from the given file and creates the vertex shader.
	/// @param[in]	filename	The file to load.
	/// @return					False if an error occurred.
	//---------------------------------------------------------------------------
	bool LoadVertexShader(const char* filename);

	//---------------------------------------------------------------------------
	/// Loads the fragment shader combining the given files and creates the fragment shader.
	/// @paran[in]	head	The file containing the shader head.
	/// @param[in]	body	The file containing the shader body.
	/// @return				False if an error occurred.
	//---------------------------------------------------------------------------
	bool LoadFragmentShader(const char* head, const char* body);

	//---------------------------------------------------------------------------
	/// Loads the fragment shader from the given file and creates the fragment shader.
	/// @param[in]	filename	The file to load.
	/// @return					False if an error occurred.
	//---------------------------------------------------------------------------
	bool LoadFragmentShader(const char* filename);

	//---------------------------------------------------------------------------
	/// Links the shader program.
	/// @return				False if an error occurred.
	//---------------------------------------------------------------------------
	bool Link();

	//---------------------------------------------------------------------------
	/// Sets an uniform variable.
	/// @param[in]	name	The name of the uniform variable.
	/// @param[in]	m		The matrix to set.
	/// @return				False if an error occurred.
	//---------------------------------------------------------------------------
	bool SetUniform(const char* name, const glm::mat4 & m);

	//---------------------------------------------------------------------------
	/// Sets an uniform variable.
	/// @param[in]	name	The name of the uniform variable.
	/// @param[in]	v		The vector to set.
	/// @return				False if an error occurred.
	//---------------------------------------------------------------------------
	bool SetUniform(const char* name, const glm::vec3 & v);

	//---------------------------------------------------------------------------
	/// Sets an uniform variable.
	/// @param[in]	name	The name of the uniform variable.
	/// @param[in]	v		The float value to set.
	/// @return				False if an error occurred.
	//---------------------------------------------------------------------------
	bool SetUniform(const char* name, const glm::float32& v);

	//---------------------------------------------------------------------------
	/// Sets an uniform variable.
	/// @param[in]	name	The name of the uniform variable.
	/// @param[in]	v		The unsigned int value to set.
	/// @return				False if an error occurred.
	//---------------------------------------------------------------------------
	bool SetUniform(const char* name, unsigned int v);

	//---------------------------------------------------------------------------
	/// Sets an uniform variable.
	/// @param[in]	name	The name of the uniform variable.
	/// @param[in]	v		The array of vectors to set.
	/// @param[in]	count	The total count of float components in the given vector.
	/// @return				False if an error occurred.
	//---------------------------------------------------------------------------
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

	//---------------------------------------------------------------------------
	/// Returns the location of the given uniform variable.
	/// @param[in]	name	The name of the uniform variable.
	/// @return				The location of the variable.
	//---------------------------------------------------------------------------
	GLint GetUniformLocation(const char* name);

	//---------------------------------------------------------------------------
	/// Creates a new shader.
	/// @param[in]	type	The shader type. See glCreateShader().
	/// @param[in]	text	The shader source.
	/// @param[out]	store	The shader ID.	
	/// @return				False if an error occurred.
	//---------------------------------------------------------------------------
	bool MakeShader(unsigned int type, const std::string& text, unsigned int& store);

private:

	bool _isLinked;					///> True if the shader program is linked.
	unsigned int _program;			///> The program ID.
	unsigned int _vertexShader;		///> The vertex shader ID.
	unsigned int _fragmentShader;	///> The fragment shader ID.
};


#endif	// VOLUME_DEMO_PROGRAM_H__
