#version 460 core

in vec2 fs_st;

out vec4 colour;

uniform sampler2D sampler;

void main() {
	colour = texture(sampler, vec2(fs_st.s, 1.0 - fs_st.t));
}