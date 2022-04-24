#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in ivec4 aBoneIDs;
layout(location = 4) in vec4 aBoneWeights;

out vec2 TexCoords;


uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

#define MAX_BONES 52
#define MAX_BONES_PER_VERTEX 7
uniform mat4 BoneMatrices[MAX_BONES];


void main(){
	vec4 FragPos = vec4(0.0f);

	for(int i = 0; i < MAX_BONES_PER_VERTEX; i++){
		if(aBoneIDs[i] == -1)
			continue;
		if(aBoneIDs[i] >= MAX_BONES){
			FragPos = vec4(aPos, 1.0f);
			break;
		}
		vec4 LocalPos = BoneMatrices[aBoneIDs[i]] * vec4(aPos, 1.0f);
		FragPos += LocalPos * aBoneWeights[i];
		vec3 LocalNormal = mat3(BoneMatrices[aBoneIDs[i]]) * aNormal;
	}


	mat4 BoneTransform = BoneMatrices[aBoneIDs[0]] * aBoneWeights[0];
	BoneTransform     += BoneMatrices[aBoneIDs[1]] * aBoneWeights[1];
    BoneTransform     += BoneMatrices[aBoneIDs[2]] * aBoneWeights[2];
    BoneTransform     += BoneMatrices[aBoneIDs[3]] * aBoneWeights[3];


	mat4 VM = view * model;
	gl_Position = projection * VM * FragPos;
	TexCoords = aTexCoords;
}

