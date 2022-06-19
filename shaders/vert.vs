#version 460 core

layout(location = 0) in vec3 pos;

uniform mat4 PVM;

void main() {
	gl_Position =  PVM * vec4(pos, 1.0);
}