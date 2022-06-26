#version 460 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 vs_normal;

out vec3 fs_normal;
out vec3 fs_eye;
out vec4 fs_sunSpacePos;

uniform mat4 sunPVM;
uniform mat4 camPVM;
uniform mat4 M;
uniform mat3 normalMatrix;
uniform vec3 vs_eye;

void main() {
	gl_Position =  camPVM * vec4(pos, 1.0);
	fs_normal = normalMatrix * vs_normal;
	fs_eye = vs_eye - vec3(M * vec4(pos, 1.0));
	fs_sunSpacePos = sunPVM * vec4(pos, 1.0);
}