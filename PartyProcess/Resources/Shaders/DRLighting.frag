#version 330 core

uniform sampler2D positionTexture;
uniform sampler2D normalTexture;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowTexture;

uniform vec2 screenSize;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 lightDirection;
uniform vec3 eyePos;
uniform int numLights;
uniform mat4 shadowMatrix;


void main() {
    vec3 diffuseTerm;
    vec3 specularTerm;
	vec4 shadowCoord;
	vec3 shadowIndex;
	vec3 adjustedLightColor = lightColor / numLights;

	vec2 uv = gl_FragCoord.xy / screenSize;
	
	// Shadow Setup
	shadowCoord = shadowMatrix * texture(positionTexture, uv);
	shadowIndex = shadowCoord.xyz / shadowCoord.w;
	shadowIndex = shadowIndex * 0.5 + 0.5;

	float lightDepth = texture(shadowTexture, shadowIndex.xy).r;
	float pixelDepth = shadowIndex.z;


	vec3 diffuseColor = texture(diffuseTexture, uv).xyz;
	float usesLighting = texture(diffuseTexture, uv).w;

	//Diffuse 
	vec3 worldPos = texture(positionTexture, uv).xyz;
	vec3 lightDir = normalize(lightDirection);
	vec3 norm = normalize(texture(normalTexture, uv).xyz);
	float diff = max(dot(norm,-lightDir),0.0);
	diffuseTerm = diff * diffuseColor * adjustedLightColor;

	//Specular
	vec3 viewdir = normalize(eyePos - worldPos);
	vec3 reflectdir = normalize(reflect(lightDir,norm));
	float spec = pow(max(dot(viewdir,reflectdir),0.0),texture(specularTexture, uv).w);
	specularTerm = spec * texture(specularTexture, uv).xyz * adjustedLightColor;

	// Compute Shadow
	float delta = 1.0;
	float epsi = 0.00005; //max(0.01 * (1.0 - dot(norm, lightDir)), 0.001);
	if (shadowCoord.w > 0.0f && shadowIndex.x >= 0.0f && shadowIndex.x <= 1.0f && shadowIndex.y >= 0.0f && shadowIndex.y <= 1.0f) {
		if (pixelDepth - epsi > lightDepth) {
			delta = 0.0;
		}
	}

	if (usesLighting > 0.0) {
		gl_FragColor.xyz = delta * (specularTerm + diffuseTerm);
	}
	else {
		gl_FragColor = vec4(0.0);
	}
}