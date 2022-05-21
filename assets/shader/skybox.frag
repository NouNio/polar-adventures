#version 330

in vec3 TexCoords;

layout(location=0) out vec4 FragColor;
layout(location=1) out vec4 normal;
layout(location=2) out vec4 depth;
uniform samplerCube skybox;

void main() {
	FragColor = texture(skybox, TexCoords);
	normal=vec4(0,0,0,1);
	depth=vec4(1);
}