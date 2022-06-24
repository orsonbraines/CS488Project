#version 460 core

layout(location = 0) in vec3 pos;
layout(location = 1) in float vs_alpha;

out float fs_alpha;

uniform mat4 PV;

void main() {
	gl_Position = PV * vec4(pos, 1.0);
	fs_alpha = vs_alpha;
}