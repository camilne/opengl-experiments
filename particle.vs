#version 330

layout (location = 0) in vec4 offsetSize;
layout (location = 1) in vec4 v_color;

uniform mat4 MV;

out Vertex {
	float size;
	vec4 color;
} vertex;

void main() {
	gl_Position = MV * vec4(offsetSize.xyz, 1.0);
	vertex.size = offsetSize.w;
	vertex.color = v_color;
}