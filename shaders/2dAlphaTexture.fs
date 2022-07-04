#version 460 core

uniform sampler2D sampler;
uniform vec3 colour;

in vec2 fs_uv;

out vec4 colour_out;

void main() {
	colour_out = vec4(colour, texture(sampler, fs_uv).r);
}