#version 330 core

#define N_BONES 60

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in ivec4 aBoneIDs;
layout (location = 4) in vec4 aWeigths;

out vec3 FragPos;  // the postion of the vertex in world coordinates
out vec3 Normal;  // perpendicular to the vertex "surface" 
out vec2 TexCoords;
flat out ivec4 BoneIDs;
out vec4 Weights;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 BoneTransforms[N_BONES];

void main()
{
    mat4 boneTransform = BoneTransforms[aBoneIDs[0]] * aWeigths[0];
    boneTransform += BoneTransforms[aBoneIDs[1]] * aWeigths[1];
    boneTransform += BoneTransforms[aBoneIDs[2]] * aWeigths[2];
    boneTransform += BoneTransforms[aBoneIDs[3]] * aWeigths[3];

    vec4 PosL = boneTransform * vec4(aPos, 1.0);
    FragPos = vec3(model * PosL);
    //FragPos = vec3(model* vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;  // bring the normal vector to world space, make sure it is scaled correctly, i.e. use normal matrix
    TexCoords = aTexCoords;
    BoneIDs = aBoneIDs;
    Weights = aWeigths;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}