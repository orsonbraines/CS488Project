#version 460 core

in vec3 fs_normal;
in vec3 fs_eye;
in vec4 fs_sunSpacePos;

out vec4 colour_out;

uniform vec3 lightDir;
uniform vec3 lightColour;
uniform vec3 ambientColour;

uniform vec3 Kd;
uniform vec3 Ks;
uniform float Ns;

uniform sampler2D shadow;

float shadowRatio() {
	const float shadowBias = 0.0001;
	// perspective divide and map to [0,1]
	vec3 projectedSunPos = (fs_sunSpacePos.xyz / fs_sunSpacePos.w) * 0.5 + 0.5;
	// If the depth is outside of the shadow map range we consider it to be in light
	float sunPosDepth = min(1.0, projectedSunPos.z);
	float samplesInShadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadow, 0);
	for(int i = -1; i <= 1; ++i) {
		for(int j = -1; j <= 1; ++j) {
			float shadowDepth = texture(shadow, projectedSunPos.xy + vec2(i,j) * texelSize).r;
			samplesInShadow += sunPosDepth < shadowDepth + shadowBias ? 1.0 : 0.0; 
		}
	}
	return samplesInShadow / 9.0;
} 

void main() {
	vec3 colour = ambientColour * Kd;
	float sRatio = shadowRatio();
	vec3 n = normalize(fs_normal);
	vec3 l = normalize(lightDir);
	float intensity = dot(n, l);
	colour += sRatio * max(intensity, 0.0) * lightColour * Kd;

	vec3 r = dot(l,n) * n * 2 - l;
	intensity = dot(r, normalize(fs_eye));
	if(intensity > 0.0) {
		colour += sRatio * pow(intensity, Ns) * lightColour * Ks;
	}

	colour_out = vec4(colour, 1.0);
}