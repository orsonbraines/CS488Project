#version 460 core

in vec3 texDir;

out vec4 colour;

uniform samplerCube skybox;
uniform float brightness;

void main() {
	colour = brightness * texture(skybox, texDir);
	colour.a = 1.0;
}