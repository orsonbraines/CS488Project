#version 460 core

layout(location = 0) in vec2 pos;

out vec3 fs_normal;
out vec3 fs_eye;
out vec4 fs_sunSpacePos;
out vec3 fs_fl;
out vec4 fs_flSpacePos;
out float fs_alpha;

uniform sampler2D sampler;
uniform mat4 camPVM;
uniform mat4 sunPVM;
uniform mat4 flPVM;
uniform mat4 M;

uniform mat3 normalMatrix;
uniform vec3 u_eye;
uniform vec3 flPos;

uniform float plane;
uniform float maxAlphaDepth;

void main() {
	float z = texture(sampler, pos).r;
	fs_alpha = clamp((plane - z) / maxAlphaDepth, 0.0, 1.0); 
	vec4 modelPos = vec4(pos.x, pos.y, plane, 1.0);
	vec3 worldPos = vec3(M * modelPos);
	gl_Position = camPVM * modelPos;

	fs_normal = vec3(0,1,0);
	fs_eye = u_eye - worldPos;
	fs_sunSpacePos = sunPVM * modelPos;
	fs_fl = flPos - worldPos;
	fs_flSpacePos = flPVM * modelPos;
}