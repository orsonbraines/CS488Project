#version 460 core

in vec2 fs_uv;

out vec4 colour;

uniform sampler2D colourIn;
uniform sampler2D depthIn;
uniform float nearClip, farClip;
uniform float kBlurry;
uniform float focusDistance;
const vec2 samples[13] = {
	vec2(0.0, 0.0),
	vec2(-0.326212, -0.40581),
	vec2(-0.840144, -0.07358),
	vec2(-0.695914, 0.457137),
	vec2(-0.203345, 0.620716),
	vec2(0.96234, -0.194983),
	vec2(0.473434, -0.480026),
	vec2(0.519456, 0.767022),
	vec2(0.185461, -0.893124),
	vec2(0.507431, 0.064425),
	vec2(0.89642, 0.412458),
	vec2(-0.32194, -0.932615),
	vec2(-0.791559, -0.59771)
};
const int numSamples = 13;

void main() {
	float z = texture(depthIn, fs_uv).r;
	// undo the projection to find the depth in camera space
	z = z * 2.0 - 1.0;
	z = 2.0 * nearClip * farClip / (farClip + nearClip - z * (farClip - nearClip));

	float bluriness = clamp(kBlurry * abs(z - focusDistance) / (z * focusDistance), 0.0, 0.05);

	for(int i=0; i < numSamples; ++i) {
		colour += texture(colourIn, fs_uv + bluriness * samples[i]);
	}

	colour /= float(numSamples);
	//colour = vec4(bluriness, bluriness, bluriness, 1.0);
}