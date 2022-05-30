#version 330 core

//in vec2 UV;
in vec4 particleCol;

out vec4 col;


void main(){
	col = particleCol;
}