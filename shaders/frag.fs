#version 460 core

uniform vec3 u_colour;

out vec3 colour;

void main() {
	colour = u_colour;
}