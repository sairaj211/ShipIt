#version 330

uniform samplerCube cubeMap;
uniform sampler2D modelTexture;
uniform sampler2D normalMap;
uniform vec4 diffuse;
uniform vec4 specular;
uniform bool UsesTexture;
uniform bool UsesLighting;
uniform bool UsesCubeMap;
uniform bool UsesNormalMap;

in vec3 normals;
in vec4 worldPos;
in vec2 texCoord;
in vec3 cubeTexCoord;
in mat3 TBN;

void main() {
	// Get Texture data
	vec4 textureColor = vec4(0.0);
	if (UsesTexture) {
		if (UsesCubeMap) {
			textureColor = texture(cubeMap, cubeTexCoord);
		}
		else {
			textureColor = texture(modelTexture, texCoord);
		}
	}

	vec3 normal = normals;
	if (UsesNormalMap) {
		vec3 normalMapNormal = 2.0 * texture(normalMap, texCoord).xyz - vec3(1.0, 1.0, 1.0);
		normal = normalize(TBN * normalMapNormal);
	}

	// World Position
	gl_FragData[0] = worldPos;

	// Normal
	gl_FragData[1] = vec4(normal, 0.0);

	// Diffuse Color
	if (UsesTexture) {
		gl_FragData[2] = vec4(textureColor.xyz, UsesLighting);
	}
	else {
		gl_FragData[2] = vec4(diffuse.xyz, UsesLighting);
		if (diffuse.w < 0.01) {
			discard;
		}
	}

	// Specular Color / Shininess
	gl_FragData[3] = specular;
}