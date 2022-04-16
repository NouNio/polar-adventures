#version 430 core
/*
* Copyright 2019 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/

in VertexData {
	vec3 position_world;
	vec3 normal_world;
	vec2 uv;
} vert;

out vec4 color;



//uniform vec3 materialCoefficients; // x = ambient, y = diffuse, z = specular 

uniform vec3 materialAmbient; 
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float specularAlpha;

uniform sampler2D diffuseTexture;
uniform sampler2D edge;

uniform float brightness;

void main() {	

	vec3 texColor = texture(diffuseTexture, vert.uv).rgb;
	 // ambient
	texColor=texColor+ texture(bloom, vert.uv).rgb;
	// ambient
	texColor*=brightness;
	           color=vec4(texColor,1);

	}

