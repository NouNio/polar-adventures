#version 430 core
/*
* Copyright 2019 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

out VertexData {
	vec3 position_world;
	vec3 normal_world;
	vec2 uv;
} vert;

out gl_PerVertex
{
    vec4 gl_Position;
};

uniform vec3 camera_world;
uniform float externalrotation;
uniform mat4 modelMatrix;
uniform mat4 viewProjMatrix;



uniform vec3 originpoint;
mat4 rotation(float angle){
mat4 result=mat4(1);
mat4 scale=mat4(1);
if (angle!=0.0){
angle=radians(angle);
while(angle>=radians(180)){
angle-=radians(180);}
if(angle<radians(90)){
scale=mat4(10/11.0)*(angle/radians(90));
scale[3][3]=1;}
else if(angle>=radians(90)){
scale=mat4(1.1)*((radians(180)-angle)/radians(90));
scale[3][3]=1;
}
result[1][1]=cos(angle);
result[2][2]=cos(angle);
result[1][2]=-sin(angle);
result[2][1]=sin(angle);}
return result*scale;
}
void main() {
	mat4 model=modelMatrix*rotation(externalrotation);
	mat3 normalMatrix= mat3(transpose(inverse(model)));
	vert.normal_world = normalMatrix * normal;
	vert.uv = uv;
	vec3 p=position;
	/*
	if(externalrotation=!0.0&&length(p)<1){
	if(p.x>0){
	p.x+=1;}
	if(p.x<0){
	p.x-=1;
	}
	if(p.y>0){
	p.y+=1;}
	if(p.x<0){
	p.y-=1;
	}
		if(p.z>0){
	p.z+=1;}
	if(p.z<0){
	p.z-=1;
	}
	}//*/
	vec4 pos = model * vec4(p, 1);

	vec3 position_world_=vec3(pos.x, pos.y, pos.z);

			vert.position_world = position_world_.xyz;
			position_world_= vert.position_world;

			if(externalrotation!=0.0){

	}

	gl_Position = viewProjMatrix *vec4( position_world_,1);

}