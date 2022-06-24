#version 460 core

in vec3 fs_normal;
in vec3 fs_eye;
in vec2 fs_uv;
in vec3 fs_udir;
in vec3 fs_vdir;

out vec3 colour;

uniform sampler2D heightfield;

uniform vec3 lightDir;
uniform vec3 lightColour;
uniform vec3 ambientColour;

uniform vec3 Kd;
uniform vec3 Ks;
uniform float Ns;

void main() {
	colour = ambientColour * Kd;
	vec3 n = normalize(fs_normal);
	vec3 l = normalize(lightDir);

	float dhdu = (texture(heightfield, fs_uv + vec2(0.01, 0)).r - texture(heightfield, fs_uv - vec2(0.01, 0)).r) * 3.0;
	float dhdv = (texture(heightfield, fs_uv + vec2(0, 0.01)).r - texture(heightfield, fs_uv - vec2(0, 0.01)).r) * 3.0;
	n = normalize(n - dhdu * normalize(fs_udir) - dhdv * normalize(fs_vdir));

	float intensity = dot(n, l);
	if(intensity > 0.0) {
		colour += intensity * lightColour * Kd;
	}

	vec3 r = dot(l,n) * n * 2 - l;
	intensity = dot(r, normalize(fs_eye));
	if(intensity > 0.0) {
		colour += pow(intensity, Ns) * lightColour * Ks;
	}
	
	//colour = texture(heightfield, fs_uv).rrr;
}