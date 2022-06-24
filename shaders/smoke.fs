#version 460 core

in float fs_alpha;

out vec4 colour;

void main() {
	colour = vec4(0.0, 0.0, 0.0, fs_alpha);
}