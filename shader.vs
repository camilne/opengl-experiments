#version 330 core

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_texCoords;

out vec3 fragPos;
out vec3 f_normal;
out vec2 f_texCoords;
out vec4 f_fragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main() {
	gl_Position = projection * view * model * vec4(v_position, 1.0);
	fragPos = vec3(model * vec4(v_position, 1.0));
	f_normal = transpose(inverse(mat3(model))) * v_normal;
	f_texCoords = v_texCoords;
	f_fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);
}