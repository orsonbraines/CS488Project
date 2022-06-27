#version 460 core

in vec3 fs_normal;
in vec3 fs_eye;
in vec4 fs_sunSpacePos;
in vec3 fs_fl;

out vec4 colour_out;

uniform vec3 sunlightDir;
uniform vec3 sunlightColour;

uniform vec3 flColour;
uniform vec3 flDir;
uniform float flSoftCutoff;
uniform float flHardCutoff;

uniform vec3 ambientColour;

uniform vec3 Kd;
uniform vec3 Ks;
uniform float Ns;

uniform sampler2D shadow;

float shadowRatio() {
	const float shadowBias = 0.001;
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

vec3 shade(vec3 n, vec3 l, vec3 instensity, float sRatio) {
	vec3 colour = vec3(0.0);	
	float aRatio = dot(n, l);
	if(aRatio > 0) {
		colour += sRatio * aRatio * instensity * Kd;
	}

	vec3 r = dot(l,n) * n * 2 - l;
	aRatio = dot(r, normalize(fs_eye));
	if(aRatio > 0.0) {
		colour += sRatio * pow(aRatio, Ns) * instensity * Ks;
	}
	return colour;
}

vec3 sunShade() {
	float sRatio = shadowRatio();
	vec3 n = normalize(fs_normal);
	vec3 l = normalize(sunlightDir);
	return shade(n, l, sunlightColour, sRatio);
}

vec3 flShade() {
	vec3 n = normalize(fs_normal);
	vec3 l = normalize(fs_fl);
	float dist2 = dot(fs_fl, fs_fl);
	float dist = sqrt(dist2);
	float edgeRatio = clamp((dot(l, normalize(flDir)) - flHardCutoff) / (flSoftCutoff - flHardCutoff), 0.0, 1.0);
	float attenuation = 1.0 + 0.07 * dist + 0.017 * dist2;
	vec3 intensity = edgeRatio * flColour / attenuation;
	return shade(n, l, intensity, 1.0);
}

void main() {
	vec3 colour = ambientColour * Kd;
	colour += sunShade();
	colour += flShade();
	colour_out = vec4(colour, 1.0);
}