#version 330 core
layout (location = 0) in vec4 vertex;  // first two items are position, the last two items are texture coords

out vec2 TexCoords;

uniform mat4 proj; 

void main()
{
	gl_Position = proj * vec4(vertex.xy, 1.0, 1.0);
	TexCoords = vertex.zw;
}