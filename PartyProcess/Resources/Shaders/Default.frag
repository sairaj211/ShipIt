#version 330 core

in vec2 TexCoord;

uniform sampler2D diffuseTexture;

uniform vec4 diffuse;
uniform vec3 specular;
uniform float shininess;

void main() {
	gl_FragColor = texture(diffuseTexture, TexCoord);
}