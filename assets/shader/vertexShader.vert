#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;  // the postion of the vertex in world coordinates
out vec3 Normal;  // perpendicular to the vertex "surface" 

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;  // bring the normal vector to world space, make sure it is scaled correctly, i.e. use normal matrix
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}