#version 460 core

layout(location = 0) in vec2 pos;

out vec3 fs_normal;
out vec3 fs_eye;
out vec2 fs_uv;

uniform sampler2D sampler;
uniform mat4 PVM;
uniform mat4 M;

uniform mat3 normalMatrix;
uniform vec3 vs_eye;

void main() {
	fs_uv = pos;
	float z = texture(sampler, pos).r;
	gl_Position = PVM * vec4(pos.x, pos.y, z, 1.0);

	float dzdx = (texture(sampler, pos + vec2(0.01, 0)).r - texture(sampler, pos - vec2(0.01, 0)).r) / 0.02;
	float dzdy = (texture(sampler, pos + vec2(0, 0.01)).r - texture(sampler, pos - vec2(0, 0.01)).r) / 0.02;

	fs_normal = normalMatrix * cross(vec3(1, 0, dzdx), vec3(0, 1, dzdy));
	fs_eye = vs_eye - vec3(M * vec4(pos.x, pos.y, z, 1.0));
}