#version 330 core

layout(location = 0) in vec3 position; 
layout(location = 1) in vec2 textCoords;
layout(location = 2) in mat4 instancedMat;
layout(location = 6) in vec4 instancedCol;

//uniform vec4 ParticleColor;
//uniform mat4 u_Model;

uniform mat4 u_View;
uniform mat4 u_Proj;

out vec2 TexCoords;
out vec4 parColor;

void main()
{
	TexCoords = vec2(textCoords.x, -textCoords.y);
	parColor = instancedCol;
	gl_Position = u_Proj * u_View * instancedMat * vec4((position), 1.0);
}