#version 330

uniform sampler2D silhouetteTexture;

uniform vec2 screenSize;
uniform int w;

void main() {
	vec2 uv = gl_FragCoord.xy / screenSize;
	vec3 outlineColor = texture(silhouetteTexture, uv).xyz;
	if (outlineColor != vec3(1.0)) {
		vec2 inverseScreenSize = 1.0 / screenSize;

		for (int i = -w; i <= w; ++i) {
			for (int j = -w; j <= w; ++j) {
				if (i == 0 && j == 0)
					continue;

				vec2 offset = vec2(i, j) * inverseScreenSize;

				if (texture(silhouetteTexture, uv + offset).xyz == vec3(1.0)) {
					gl_FragColor = vec4(outlineColor, 1.0);
					return;
				}
			}
		}
	}
	discard;
}