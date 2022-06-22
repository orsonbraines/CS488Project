#version 460 core

in vec3 fs_normal;

out vec3 colour;

uniform vec3 lightDir;
uniform vec3 diffuseColour;
uniform vec3 ambientColour;

void main() {
	colour = ambientColour;
	float intensity = dot(normalize(fs_normal), normalize(lightDir));
	if(intensity > 0.0) {
		colour += intensity * diffuseColour;
	}
}