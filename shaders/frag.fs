#version 460 core

in vec3 fs_normal;
in vec3 fs_eye;
in vec2 fs_uv;

out vec3 colour;

uniform sampler2D sampler;

uniform vec3 lightDir;
uniform vec3 lightColour;
uniform vec3 ambientColour;

uniform vec3 Ks;
uniform float Ns;

void main() {
	vec3 Kd = texture(sampler, vec2(fs_uv.s, 1.0 - fs_uv.t)).rgb;
	colour = ambientColour * Kd;
	vec3 n = normalize(fs_normal);
	vec3 l = normalize(lightDir);
	float intensity = dot(n, l);
	if(intensity > 0.0) {
		colour += intensity * lightColour * Kd;
	}

	vec3 r = dot(l,n) * n * 2 - l;
	intensity = dot(r, normalize(fs_eye));
	if(intensity > 0.0) {
		colour += pow(intensity, Ns) * lightColour * Ks;
	}
}