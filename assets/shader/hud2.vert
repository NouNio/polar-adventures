#version 330 core
in vec2 vertex;  // first two items are position, the last two items are texture coords
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
uniform mat4 proj; 
uniform vec2 scale;
uniform vec2 xyoffset;
bool front;

void main()
{
float zoff = 0.5;
if(front) zoff =1.0;
	gl_Position = proj * (vec4((aPos.xy*scale)+xyoffset, zoff, 1.0));
}