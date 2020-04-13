#version 330

uniform sampler2D positionTexture;
uniform sampler2D normalTexture;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

uniform vec2 screenSize;

void main() {
	vec2 uv = gl_FragCoord.xy / screenSize;

	float ambientIntensity = 0.3;
	float usesLighting = texture(diffuseTexture, uv).w;

	if (usesLighting <= 0.0) {
		ambientIntensity = 1.0;
	}
	gl_FragColor.xyz = texture(diffuseTexture, uv).xyz * ambientIntensity;
}