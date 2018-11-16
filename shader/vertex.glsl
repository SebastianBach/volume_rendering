#version 410

layout (location = 0) in vec3 VertexPosition;

uniform mat4 MVP;
uniform mat4 ModelMatrix;

smooth out vec4 WorldSpacePos;

void main()
{
	WorldSpacePos = ModelMatrix * vec4(VertexPosition,1.0);
	
    gl_Position = MVP * vec4(VertexPosition,1.0);
}

