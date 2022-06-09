#version 330 core
layout (location = 0) in vec2 vertex;  // first two items are position, the last two items are texture coords

uniform mat4 proj; 
uniform float scale;
uniform vec2 xyoffset;

void main()
{
	gl_Position = proj * (vec4((vertex.xy*scale)+xyoffset, 0.0, 1.0));
}