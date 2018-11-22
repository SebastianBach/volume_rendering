#version 410

layout (location = 0) in vec3 VertexPosition;

uniform mat4 u_mvp;
uniform mat4 u_modelMatrix;

smooth out vec4 s_worldSpacePos;

void main()
{
	s_worldSpacePos = u_modelMatrix * vec4(VertexPosition,1.0);
	
    gl_Position = u_mvp * vec4(VertexPosition,1.0);
}

