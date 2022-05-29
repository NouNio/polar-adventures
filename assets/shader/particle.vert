#version 330 core

layout(location = 0) in vec3 squareVertices;
layout(location = 1) in vec4 xyzs;  // pos of the particle center and size of the quad
layout(location = 2) in vec4 col;

//out vec2 UV;
out vec4 particleCol;

uniform vec3 camRight;
uniform vec3 camUp;
uniform mat4 view;
uniform mat4 projection;


void main() {
	float particleSize = xyzs.w;
	vec3 particleCenter = xyzs.xyz;

	vec3 vertexPos = particleCenter + (camRight * squareVertices.x + camUp * squareVertices.y) * particleSize;

	gl_Position =  projection * view * vec4(vertexPos, 1.0f);

	//UV = squareVertices.xy + vec2(0.5, 0.5);
	particleCol = col;
}