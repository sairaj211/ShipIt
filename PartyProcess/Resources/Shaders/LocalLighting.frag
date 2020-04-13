#version 330 core

uniform sampler2D positionTexture;
uniform sampler2D normalTexture;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

uniform float Radius;
uniform vec2 screenSize;
uniform vec3 lightColor;
uniform float intensity;
uniform vec3 lightPos;
uniform vec3 eyePos;

void main() {

    vec3 diffuseTerm;
    vec3 specularTerm;
	
	vec2 uv = gl_FragCoord.xy / screenSize;

	vec3 worldPos = texture(positionTexture, uv).xyz;
	vec3 lightDir = worldPos - lightPos;
	float dist = length(lightDir);
	lightDir = normalize(lightDir);

	vec3 diffuseColor = texture(diffuseTexture, uv).xyz;
	float usesLighting = texture(diffuseTexture, uv).w;

	//Diffuse 
	vec3 norm = normalize(texture(normalTexture, uv).xyz);
	float diff = max(dot(norm,-lightDir),0.0);
	diffuseTerm = diff * diffuseColor * lightColor;

	//Specular
	vec3 viewdir = normalize(eyePos - worldPos);
	vec3 reflectdir = normalize(reflect(lightDir,norm));
	float spec = pow(max(dot(viewdir,reflectdir),0.0),texture(specularTexture, uv).w);
	specularTerm = spec * texture(specularTexture, uv).xyz * lightColor;


	float normalizedDist = dist / Radius;
	float attenuation = clamp(1.0 / (1.0 + 25.0 * normalizedDist * normalizedDist), 0.0, 1.0) * clamp((1.0 - normalizedDist) * 5.0, 0.0, 1.0);

	if(dist < Radius)
	{
		if (usesLighting > 0.0) {
			gl_FragColor.xyz = intensity * attenuation * (diffuseTerm + specularTerm);
		}
		else {
			gl_FragColor = vec4(0.0);
		}
	}
	else
	{
		gl_FragColor.xyz = vec3(0.0, 0.0, 0.0);
	}
}