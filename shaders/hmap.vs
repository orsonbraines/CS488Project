#version 460 core

layout(location = 0) in vec2 pos;

//out vec3 fs_normal;
//out vec3 fs_eye;
out vec2 fs_uv;

uniform sampler2D sampler;
uniform mat4 PV;
//uniform mat3 normalMatrix;
//uniform vec3 vs_eye;

void main() {
	fs_uv = pos;
	float y = texture(sampler, pos).r;
	gl_Position = PV * vec4(pos.x, y, pos.y, 1.0);
}