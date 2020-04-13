#version 330

uniform sampler2D modelTexture;
uniform vec4 diffuse;
uniform bool UsesTexture;

in vec2 texCoord;

void main() {
    if (UsesTexture) {
		gl_FragColor = texture(modelTexture, texCoord);
	}
	else {
		gl_FragColor = diffuse;
	}
}