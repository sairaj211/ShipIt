#version 330

uniform sampler2D colorTexture;

uniform bool antiAliasing;
uniform vec2 screenSize;

uniform float spanMax;
uniform float reduceMin;
uniform float reduceMultiplier;

uniform float fadeMultiplier;

float GetLuma(vec3 color) {
	return dot(color, vec3(0.299, 0.587, 0.114));
}

vec4 FXAA(vec3 color, vec2 uv) {
	vec2 inverseScreenSize = 1.0 / screenSize;
	float centerLuma = GetLuma(color);
	float upLeftLuma = GetLuma(texture(colorTexture, uv + vec2(-1.0, -1.0) * inverseScreenSize).xyz);
	float upRightLuma = GetLuma(texture(colorTexture, uv + vec2(1.0, -1.0) * inverseScreenSize).xyz);
	float downLeftLuma = GetLuma(texture(colorTexture, uv + vec2(-1.0, 1.0) * inverseScreenSize).xyz);
	float downRightLuma = GetLuma(texture(colorTexture, uv + vec2(1.0, 1.0) * inverseScreenSize).xyz);

	float maxLuma = max(centerLuma, max(max(upLeftLuma, upRightLuma), max(downLeftLuma, downRightLuma)));
	float minLuma = min(centerLuma, min(min(upLeftLuma, upRightLuma), min(downLeftLuma, downRightLuma)));

	// Get Direction
	vec2 direction = vec2(-((upLeftLuma + upRightLuma) - (downLeftLuma + downRightLuma)), ((upLeftLuma + downLeftLuma) - (upRightLuma + downRightLuma)));

	// Normalize the smallest value of the direction to 1
	float directionReduce = max((upLeftLuma + upRightLuma + downLeftLuma + downRightLuma) * (reduceMultiplier * 0.25), reduceMin);
	float directionAdjustment = 1.0 / (min(abs(direction.x), abs(direction.y)) + directionReduce);

	// Clamp Direction
	direction = clamp(direction * directionAdjustment, vec2(-spanMax), vec2(spanMax)) * inverseScreenSize;

	// Take one sample on each side of the direction
	vec3 result0 = 0.5 * (texture(colorTexture, uv + (direction * vec2(1.0 / 3.0 - 0.5))).xyz + texture(colorTexture, uv + (direction * vec2(2.0 / 3.0 - 0.5))).xyz);

	// Take two more samples at the ends
	vec3 result1 = result0 * 0.5 + 0.25 * (texture(colorTexture, uv + (direction * vec2(0.0 / 3.0 - 0.5))).xyz + texture(colorTexture, uv + (direction * vec2(3.0 / 3.0 - 0.5))).xyz);

	// Check if result1 was sampled out of luma range
	float result1Luma = GetLuma(result1);
	if (result1Luma < minLuma || result1Luma > maxLuma) {
		return vec4(result0, 1.0);
	}
	else {
		return vec4(result1, 1.0);
	}
}

void main() {
	vec2 uv = gl_FragCoord.xy / screenSize;

	if (antiAliasing) {
		gl_FragColor = fadeMultiplier * FXAA(texture(colorTexture, uv).xyz, uv);
	}
	else {
		gl_FragColor = fadeMultiplier * texture(colorTexture, uv);
	}
}