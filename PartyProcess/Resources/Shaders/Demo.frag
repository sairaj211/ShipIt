#version 330 core

uniform sampler2D modelTexture;
uniform sampler2D normalMap;

uniform vec4 diffuse;
uniform vec3 specular;
uniform float shininess;
uniform bool UsesTexture;
uniform bool UsesLighting;
uniform bool UsesNormalMap;

in vec3 normals;
in vec3 worldPos;
in vec2 texCoord;
in mat3 TBN;

uniform vec3 lightPos;
uniform vec3 eyePos;

void main() {
	vec3 diffuseTerm;
    vec3 specularTerm;

	vec4 diffuseColor = diffuse;
	if (UsesTexture) {
		diffuseColor = texture(modelTexture, texCoord);
	}

	if (UsesLighting && (diffuseColor.a >= 1.0 || UsesTexture)) {
		// Calculate Normal
		vec3 norm = normals;
		if (UsesNormalMap) {
			vec3 normalMapNormal = 2.0 * texture(normalMap, texCoord).xyz - vec3(1.0, 1.0, 1.0);
			norm = normalize(TBN * normalMapNormal);
		}

		//Diffuse
		vec3 lightdir = normalize(worldPos - lightPos);
		float diff = max(dot(norm,-lightdir),0.0);
		diffuseTerm = diff * diffuseColor.xyz;

		//Specular
		vec3 viewdir = normalize(eyePos - worldPos);
		vec3 reflectdir = normalize(reflect(lightdir,norm));
		float spec = pow(max(dot(viewdir,reflectdir),0.0),shininess);
		specularTerm = spec * specular;

		gl_FragColor = vec4(diffuseTerm + specularTerm, 1.0f);
	}
	else {
		gl_FragColor = diffuseColor;
	}
}