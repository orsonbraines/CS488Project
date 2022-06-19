#version 460 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 vs_st;

out vec2 fs_st;

uniform mat4 PVM;

void main() {
	gl_Position =  PVM * vec4(pos, 1.0);
	fs_st = vs_st;
}