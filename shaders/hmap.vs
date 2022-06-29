#version 460 core

layout(location = 0) in vec2 pos;

out vec3 fs_normal;
out vec3 fs_eye;
out vec4 fs_sunSpacePos;
out vec3 fs_fl;
out vec4 fs_flSpacePos;
out float alpha;

uniform sampler2D sampler;
uniform mat4 camPVM;
uniform mat4 sunPVM;
uniform mat4 flPVM;
uniform mat4 M;

uniform mat3 normalMatrix;
uniform vec3 u_eye;
uniform vec3 flPos;

void main() {
	float z = texture(sampler, pos).r;
	gl_Position = camPVM * vec4(pos.x, pos.y, z, 1.0);

	vec2 h = vec2(0.01);
	float dzdx = (texture(sampler, pos + vec2(h.x, 0)).r - texture(sampler, pos - vec2(h.x, 0)).r) / (2.0 * h.x);
	float dzdy = (texture(sampler, pos + vec2(0, h.y)).r - texture(sampler, pos - vec2(0, h.y)).r) / (2.0 * h.y);

	fs_normal = normalMatrix * cross(vec3(1, 0, dzdx), vec3(0, 1, dzdy));
	fs_eye = u_eye - vec3(M * vec4(pos.x, pos.y, z, 1.0));
	fs_sunSpacePos = sunPVM * vec4(pos.x, pos.y, z, 1.0);
	fs_fl = flPos - vec3(M * vec4(pos.x, pos.y, z, 1.0));
	fs_flSpacePos = flPVM * vec4(pos.x, pos.y, z, 1.0);
	alpha = 1.0f;
}
