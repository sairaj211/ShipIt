#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textureCoordinate;
layout (location = 3) in vec3 tangent;
layout (location = 4) in ivec4 boneIndices;
layout (location = 5) in vec4 boneWeights;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform mat4 affineAnimationMatrix;
uniform mat4 boneTransformations[100];

uniform bool isAnimated;

uniform vec3 scale;

uniform bool isBillboard;

out vec2 texCoord;

void main()
{
	vec4 bonedPos = vec4(position, 1.0f);

	if (isAnimated) {
		mat4 boneMatrix = boneTransformations[boneIndices[0]] * boneWeights[0];
		boneMatrix += boneTransformations[boneIndices[1]] * boneWeights[1];
		boneMatrix += boneTransformations[boneIndices[2]] * boneWeights[2];
		boneMatrix += boneTransformations[boneIndices[3]] * boneWeights[3];
	
		bonedPos = boneMatrix * bonedPos;
	}

	texCoord = textureCoordinate;

	vec4 worldPos = modelMatrix * affineAnimationMatrix * bonedPos;
	if (isBillboard) {
		vec3 cameraRight = vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
		vec3 cameraUp = vec3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);
		worldPos = vec4(vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]) + cameraRight * position.x * scale.x + cameraUp * position.y * scale.y, 1.0);
	}

	gl_Position = projectionMatrix * viewMatrix * worldPos;
}