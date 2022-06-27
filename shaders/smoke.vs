#version 460 core

layout(location = 0) in vec3 pos;
layout(location = 1) in float vs_alpha;
layout(location = 2) in vec2 geomPos;
layout(location = 3) in float staticAlpha;

out float fs_alpha;

uniform mat4 PV;
uniform mat4 Vinv;
uniform float radius;

void main() {
	gl_Position = PV * (vec4(pos, 1.0) + Vinv * radius * vec4(geomPos, 0.0, 1.0));
	fs_alpha = vs_alpha * staticAlpha;
}