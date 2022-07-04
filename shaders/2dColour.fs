#version 460 core

uniform vec4 colour;

out vec4 colour_out;

void main() {
	colour_out = colour;
}