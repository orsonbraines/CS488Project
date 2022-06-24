#version 460 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 vs_normal;

out vec3 fs_normal;
out vec3 fs_eye;

uniform mat4 PVM;
uniform mat4 M;
uniform mat3 normalMatrix;
uniform vec3 vs_eye;

void main() {
	gl_Position =  PVM * vec4(pos, 1.0);
	fs_normal = normalMatrix * vs_normal;
	fs_eye = vs_eye - vec3(M * vec4(pos, 1.0));
}