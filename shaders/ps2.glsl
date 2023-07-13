#version 460 core

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D colorTex; // unused
layout(binding = 1) uniform sampler2D normalTex; // unused

layout(binding = 2) uniform PointLight { // unused
	vec3 lightViewPos;
};

void main() {
	FragColor = vec4(uv, 1, 1);
}