#version 430 core
in vec3 pos;

layout(location=0) out vec4 color;
layout(location=1) out vec4 normal;

//uniform vec3 materialCoefficients; // x = ambient, y = diffuse, z = specular 
uniform samplerCube environment;
uniform vec3 materialAmbient; 
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;

uniform float specularAlpha;
uniform sampler2D diffuseTexture;

uniform vec3 originpoint;

void main(){
color=vec4(texture(environment, pos).rgb, 1);
normal=vec4(0,0,0,1);
}