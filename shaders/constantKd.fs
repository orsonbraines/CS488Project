#version 460 core

in vec3 fs_normal;
in vec3 fs_eye;
in vec4 fs_sunSpacePos;

out vec3 colour;

uniform vec3 lightDir;
uniform vec3 lightColour;
uniform vec3 ambientColour;

uniform vec3 Kd;
uniform vec3 Ks;
uniform float Ns;

uniform sampler2D shadow;

void main() {
	colour = ambientColour * Kd;
	// shadow
	const float shadowBias = 0.0001;
	// perspective divide and map to [0,1]
	vec3 projectedSunPos = (fs_sunSpacePos.xyz / fs_sunSpacePos.w) * 0.5 + 0.5;
	float sunPosDepth = min(1.0, projectedSunPos.z);
	//float sunPosDepth = projectedSunPos.z;
	float shadowDepth = texture(shadow, projectedSunPos.xy).r;
	if(sunPosDepth <= shadowDepth + shadowBias) {
		vec3 n = normalize(fs_normal);
		vec3 l = normalize(lightDir);
		float intensity = dot(n, l);
		colour += max(intensity, 0.0) * lightColour * Kd;

		vec3 r = dot(l,n) * n * 2 - l;
		intensity = dot(r, normalize(fs_eye));
		if(intensity > 0.0) {
			colour += pow(intensity, Ns) * lightColour * Ks;
		}
	}
}