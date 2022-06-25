#version 460 core

uniform sampler2D sampler;

in vec2 fs_uv;

out vec4 colour;

void main() {
	colour = vec4(0,0,0, texture(sampler, fs_uv).r);
}