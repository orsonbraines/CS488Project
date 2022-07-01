#version 460 core

in vec2 fs_uv;

out vec4 colour;

uniform sampler2D colourIn;
uniform int isHorizontal;

const float weights[] = {
 8.880585e-04,
 1.586107e-03,
 2.721770e-03,
 4.487440e-03,
 7.108437e-03,
 1.081877e-02,
 1.582012e-02,
 2.222644e-02,
 3.000255e-02,
 3.891121e-02,
 4.848635e-02,
 5.804870e-02,
 6.677190e-02,
 7.379436e-02,
 7.835755e-02,
 7.994048e-02,
 7.835755e-02,
 7.379436e-02,
 6.677190e-02,
 5.804870e-02,
 4.848635e-02,
 3.891121e-02,
 3.000255e-02,
 2.222644e-02,
 1.582012e-02,
 1.081877e-02,
 7.108437e-03,
 4.487440e-03,
 2.721770e-03,
 1.586107e-03,
 8.880585e-04
};

const int numSamples = 31;

void main() {
	vec2 texelSize = 1.0 / textureSize(colourIn, 0);
	vec2 d = isHorizontal == 1 ? vec2(texelSize.x, 0.0) : vec2(0.0, texelSize.y);
	colour = vec4(0.0);
	for(int i=0; i < numSamples; ++i) {
		int displacement = i - (numSamples / 2);
		colour += texture(colourIn, fs_uv + float(displacement) * d) * weights[i];
	}
}
