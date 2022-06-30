#version 460 core

in vec2 fs_uv;

out vec4 colour;

uniform sampler2D sharpColourIn;
uniform sampler2D blurryColourIn;
uniform sampler2D depthIn;
uniform float nearClip, farClip;
uniform float kBlurry;
uniform float focusDistance;

void main() {
	float z = texture(depthIn, fs_uv).r;
	// undo the projection to find the depth in camera space
	z = z * 2.0 - 1.0;
	z = 2.0 * nearClip * farClip / (farClip + nearClip - z * (farClip - nearClip));

	float blurriness = clamp(kBlurry * abs(z - focusDistance) / (z * focusDistance), 0.0, 1.00);

	colour = mix(texture(sharpColourIn, fs_uv), texture(blurryColourIn, fs_uv), blurriness);
}