#version 460 core

out gl_PerVertex {
  vec4 gl_Position;
  float gl_ClipDistance[1];
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 vs_normal;
layout(location = 2) in vec2 vs_uv;
layout(location = 3) in vec3 vs_udir;
layout(location = 4) in vec3 vs_vdir;

out vec3 fs_normal;
out vec2 fs_uv;
out vec3 fs_udir;
out vec3 fs_vdir;
out vec3 fs_eye;
out vec4 fs_sunSpacePos;
out vec3 fs_fl;
out vec4 fs_flSpacePos;
out float fs_alpha;

uniform mat4 M;
uniform mat4 sunPVM;
uniform mat4 flPVM;
uniform mat4 camPVM;
uniform mat3 normalMatrix;
uniform vec3 u_eye;
uniform vec3 flPos;
uniform float u_alpha;
uniform float plane;

void main() {
	gl_Position =  camPVM * vec4(pos, 1.0);
	gl_ClipDistance[0] = (M * vec4(pos, 1.0)).y - plane;
	fs_normal = normalMatrix * vs_normal;
	fs_uv = vs_uv;
	fs_udir = vec3(M * vec4(vs_udir, 0.0));
	fs_vdir = vec3(M * vec4(vs_vdir, 0.0));
	fs_eye = u_eye - vec3(M * vec4(pos, 1.0));
	fs_sunSpacePos = sunPVM * vec4(pos, 1.0);
	fs_fl = flPos - vec3(M * vec4(pos, 1.0));
	fs_flSpacePos = flPVM * vec4(pos, 1.0);
	fs_alpha = u_alpha;
}