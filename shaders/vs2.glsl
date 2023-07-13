#version 460 core

layout(location = 0) in vec2 pos;

layout(location = 0) out vec2 uv;

void main() {
	uv = pos;
	gl_Position = vec4(pos, 0, 1.0f);
}