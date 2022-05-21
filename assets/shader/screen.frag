#version 430 core
/*
* Copyright 2019 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/

layout(location=0)out vec4 color;



in vec2 TexCoords;




//uniform vec3 materialCoefficients; // x = ambient, y = diffuse, z = specular 


uniform sampler2D diffuseTexture;
uniform sampler2D edge;

uniform float brightness;

void main() {	

	vec3 texColor = texture(diffuseTexture, TexCoords).rgb;
	 // ambient
	if(distance(vec3(0),texture(edge, TexCoords).rgb)>1.7320507165){
	texColor=vec3(1);
	}
	// ambient
	//texColor = vec3(1)-texColor;
	texColor*=brightness;

	color=vec4(texColor,1.0);
	//color.rgb=vec3(1)-color.rgb;
	//color=vec4(1.0,0.0,0.0,1.0);

	}

