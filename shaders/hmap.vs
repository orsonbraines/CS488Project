#version 460 core

out gl_PerVertex {
  vec4 gl_Position;
  float gl_ClipDistance[1];
};

layout(location = 0) in vec2 pos;

out vec3 fs_normal;
out vec3 fs_eye;
out vec4 fs_sunSpacePos;
out vec3 fs_fl;
out vec4 fs_flSpacePos;
out float fs_alpha;

uniform sampler2D sampler;
uniform mat4 camPVM;
uniform mat4 sunPVM;
uniform mat4 flPVM;
uniform mat4 M;

uniform mat3 normalMatrix;
uniform vec3 u_eye;
uniform vec3 flPos;
uniform float u_alpha;
uniform float plane;

void main() {
	float z = texture(sampler, pos).r;
	vec4 modelPos = vec4(pos.x, pos.y, z, 1.0);
	vec3 worldPos = vec3(M * modelPos);

	gl_Position = camPVM * vec4(pos.x, pos.y, z, 1.0);
	gl_ClipDistance[0] = worldPos.y - plane;

	vec2 h = vec2(0.01);
	float dzdx = (texture(sampler, pos + vec2(h.x, 0)).r - texture(sampler, pos - vec2(h.x, 0)).r) / (2.0 * h.x);
	float dzdy = (texture(sampler, pos + vec2(0, h.y)).r - texture(sampler, pos - vec2(0, h.y)).r) / (2.0 * h.y);

	fs_normal = normalMatrix * cross(vec3(1, 0, dzdx), vec3(0, 1, dzdy));
	fs_eye = u_eye - worldPos;
	fs_sunSpacePos = sunPVM * modelPos;
	fs_fl = flPos - worldPos;
	fs_flSpacePos = flPVM * modelPos;
	fs_alpha = u_alpha;
}
