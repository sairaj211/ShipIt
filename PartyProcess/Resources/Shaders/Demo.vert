#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textureCoordinate;
layout (location = 3) in vec3 tangent;

out vec3 normals;
out vec3 worldPos;
out vec2 texCoord;
out mat3 TBN;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() {
	worldPos = vec3(modelMatrix * vec4(position, 1.0f));
	mat4 normalMatrix = transpose(inverse(modelMatrix));
	normals = normalize((normalMatrix * vec4(normal, 0.0)).xyz);
	vec3 tangents = normalize((normalMatrix * vec4(tangent, 0.0)).xyz);
	tangents = normalize(tangents - dot(tangents, normals) * normals);
	vec3 bitangents = normalize(cross(tangents, normals));
	TBN = mat3(tangents, bitangents, normals);
	texCoord = textureCoordinate;

	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0f);
}