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

uniform mat4 viewProjMatrix;
uniform mat3 normalMatrix;


void main() {

	 vert.position_world=position;
	 vert.normal_world=normal;
	 vert.uv=uv;
	gl_Position = vec4(vert.position_world,1);
}