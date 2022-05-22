#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;


out vec2 TexCoords;

void main()
{
    vec4 totalPosition = vec4(pos,1.0f);
	
    mat4 viewModel = view * model;
    gl_Position =  projection * viewModel * totalPosition;
	TexCoords = tex;
}