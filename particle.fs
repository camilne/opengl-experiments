#version 330

in vec4 f_color;
in vec2 f_textureCoords;

out vec4 final_color;

uniform sampler2D particleTexture;

void main() {
	final_color = f_color * texture(particleTexture, f_textureCoords.xy);
}