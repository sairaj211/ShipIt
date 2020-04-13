#version 330 core

uniform sampler2D modelTexture;
uniform vec4 diffuse;
uniform bool UsesTexture;

in vec2 TexCoord;

void main()
{
	bool shouldDiscard = true;
	if (UsesTexture) {
		shouldDiscard = shouldDiscard && (texture(modelTexture, TexCoord).a < 1.0);
		shouldDiscard = shouldDiscard && (diffuse.a < 0.01);
	}
	else {
		shouldDiscard = shouldDiscard && (diffuse.a < 1.0);
	}

	if (shouldDiscard) {
		discard;
	}
}