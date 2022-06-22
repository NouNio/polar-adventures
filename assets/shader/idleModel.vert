#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;


out vec2 TexCoords;
out vec3 n;
out vec3 coorN;
out vec3 position;
void main()
{
    vec4 totalPosition = vec4(pos,1.0f);

	coorN=norm;
    mat4 viewModel = view * model;
     position = vec3(viewModel * vec4(pos, 1.0));
    coorN = mat3(transpose(inverse(viewModel))) * norm; 
    gl_Position =  projection * viewModel * totalPosition;
	TexCoords = tex;
}