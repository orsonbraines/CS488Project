#version 460 core

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


uniform mat4 M;
uniform mat4 PVM;
uniform mat3 normalMatrix;
uniform vec3 vs_eye;

void main() {
	gl_Position =  PVM * vec4(pos, 1.0);
	fs_normal = normalMatrix * vs_normal;
	fs_uv = vs_uv;
	fs_udir = vec3(M * vec4(vs_udir, 0.0));
	fs_vdir = vec3(M * vec4(vs_vdir, 0.0));
	fs_eye = vs_eye - vec3(M * vec4(pos, 1.0));
}