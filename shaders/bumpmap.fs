#version 460 core

in vec3 fs_normal;
in vec3 fs_eye;
in vec2 fs_uv;
in vec3 fs_udir;
in vec3 fs_vdir;
in vec4 fs_sunSpacePos;
in vec3 fs_fl;
in vec4 fs_flSpacePos;
in float fs_alpha;

out vec4 colour_out;

uniform sampler2D heightfield;
uniform sampler2D sunShadow;
uniform sampler2D flShadow;

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

float shadowRatio(sampler2D shadow, vec4 lightSpacePos, float shadowBias) {
	// perspective divide and map to [0,1]
	vec3 projectedLightPos = (lightSpacePos.xyz / lightSpacePos.w) * 0.5 + 0.5;
	float lightPosDepth = min(1.0, projectedLightPos.z);
	float samplesInLight = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadow, 0);
	for(int i = -1; i <= 1; ++i) {
		for(int j = -1; j <= 1; ++j) {
			float shadowDepth = texture(shadow, projectedLightPos.xy + vec2(i,j) * texelSize).r;
			samplesInLight += lightPosDepth < shadowDepth + shadowBias ? 1.0 : 0.0; 
		}
	}
	return samplesInLight / 9.0;
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

vec3 sunShade(vec3 n) {
	float sRatio = shadowRatio(sunShadow, fs_sunSpacePos, 0.001);
	vec3 l = normalize(sunlightDir);
	return shade(n, l, sunlightColour, sRatio);
}

vec3 flShade(vec3 n) {
	// the bias is more finicky for the flashlight because depth values aren't linear
	float sRatio = shadowRatio(flShadow, fs_flSpacePos, 0.001);
	vec3 l = normalize(fs_fl);
	float dist2 = dot(fs_fl, fs_fl);
	float dist = sqrt(dist2);
	float edgeRatio = clamp((dot(l, normalize(flDir)) - flHardCutoff) / (flSoftCutoff - flHardCutoff), 0.0, 1.0);
	float attenuation = 1.0 + 0.07 * dist + 0.017 * dist2;
	vec3 intensity = edgeRatio * flColour / attenuation;
	return shade(n, l, intensity, sRatio);
}

void main() {
	vec3 colour = ambientColour * Kd;
	vec3 n = normalize(fs_normal);

	float dhdu = (texture(heightfield, fs_uv + vec2(0.01, 0)).r - texture(heightfield, fs_uv - vec2(0.01, 0)).r) * 3.0;
	float dhdv = (texture(heightfield, fs_uv + vec2(0, 0.01)).r - texture(heightfield, fs_uv - vec2(0, 0.01)).r) * 3.0;
	n = normalize(n - dhdu * normalize(fs_udir) - dhdv * normalize(fs_vdir));

	colour += sunShade(n);
	colour += flShade(n);
	
	colour_out = vec4(colour, fs_alpha);
}