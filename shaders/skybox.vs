#version 460 core

layout(location = 0) in vec3 pos;

out vec3 texDir;

uniform mat4 PV;

void main() {
	gl_Position = PV * vec4(pos, 1.0);
	gl_Position.z = gl_Position.w;
	texDir = vec3(pos.x, pos.y, -pos.z);
}