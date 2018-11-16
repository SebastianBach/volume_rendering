#ifndef VOLUME_DEMO_PROGRAM_H__
#define VOLUME_DEMO_PROGRAM_H__

#include <string>
#include "glm.hpp"

class ShaderProgram
{
public:
	ShaderProgram();
	~ShaderProgram();

	bool Init();
	bool LoadVertexShader(const char* filename);
	bool LoadFragmentShader(const char* head, const char* body);
	bool LoadFragmentShader(const char* filename);

	bool Link();


	bool SetUniform(const char* name, const glm::mat4 & m);
	bool SetUniform(const char* name, const glm::vec3 & v);
	bool SetUniform(const char* name, const glm::float32& v);
	bool SetUniform(const char* name, unsigned int v);

	bool Use() const;
	void End() const;
private:

	int GetUniformLocation(const char* name);
	bool MakeShader(unsigned int type, const std::string& text, unsigned int& store);

private:

	bool _isLinked;
	unsigned int _program;
	unsigned int _vertexShader;
	unsigned int _fragmentShader;
};


#endif
