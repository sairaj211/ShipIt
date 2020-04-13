#version 330 core

in vec2 TexCoords;
in vec4 parColor;
out vec4 color;

uniform sampler2D sprite;
uniform float alpha;

uniform bool textureFlag;

void main()
{

	if(textureFlag)
	{
		color = texture(sprite, TexCoords) * parColor;
		
	}
	else
	{
		color = parColor;
	}
	//to be set from code
	//alpha = texture(sprite, TexCoords) * parColor; //* ParticleColor   ;
}