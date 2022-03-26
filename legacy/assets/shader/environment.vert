#version 430 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

out vec3 pos;
out gl_PerVertex
{
    vec4 gl_Position;
};

uniform vec3 camera_world;

uniform mat4 modelMatrix;
uniform mat4 viewProjMatrix;



uniform vec3 originpoint;
 void main(){
 pos=vec4(vec4(position, 1.0)*viewProjMatrix).rgb;
 gl_Position= vec4(position,1.0);
 }