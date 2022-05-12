#version 330

in vec3 TexCoords;

layout(location=0) out vec4 FragColor;
layout(location=1) out vec4 normal;
layout(location=2) out float depth;
uniform samplerCube skybox;

void main() {
	FragColor = texture(skybox, TexCoords);
}