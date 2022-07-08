#version 460 core

uniform int id;

out vec4 id_out;

void main() {
	id_out = vec4((id & 0xff) / 255.0, 0.0, 0.0, 1.0);
}