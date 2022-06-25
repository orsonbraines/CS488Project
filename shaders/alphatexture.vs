#version 460 core

uniform mat3 M;

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 vs_uv;

out vec2 fs_uv;

void main() {
	gl_Position = vec4(M * vec3(pos, 1.0), 1.0);
	fs_uv = vs_uv;
}