#version 460 core

in vec2 fs_uv;

out vec4 colour;

uniform sampler2D colourIn;
uniform sampler2D depthIn;
uniform float nearFocus, farFocus;

void main() {
	float z = texture(depthIn, fs_uv).r;
	if(z > nearFocus && z < farFocus) {
		colour = texture(colourIn, fs_uv);
	}
	else {
		vec2 texelSize = 1.0 / textureSize(colourIn, 0);
		colour = vec4(0,0,0,0);
		for(int i = -3; i<=3; ++i) {
			for(int j = -3; j<=3; ++j) {
				colour += texture(colourIn, fs_uv + vec2(i,j) * texelSize);
			}
		}
		colour /= 49.0;
	}
}