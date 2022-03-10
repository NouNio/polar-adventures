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

layout(location=0) out vec4 color;
layout(location=1) out vec4 glow;
uniform vec3 camera_world;

//uniform vec3 materialCoefficients; // x = ambient, y = diffuse, z = specular 

uniform vec3 materialAmbient; 
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;

uniform float specularAlpha;
uniform sampler2D diffuseTexture;
uniform samplerCube lightmap;
uniform bool isStatic;
uniform vec3 originpoint;
uniform bool glowing;
uniform struct DirectionalLight {
	vec3 color;
	vec3 direction;
} dirL;


//TODO: change this with 1D texture though not necessarily necessary
float discretize(float f){
if(f>=0.0&&f<0.2){
f=0.2;}
else if(f>=0.2&&f<0.4){
f=0.4;}
else if(f>=0.4&&f<0.6){
f=0.6;}
else if(f>=0.6&&f<0.8){
f=0.8;}
else if(f>=0.8&&f<1.0){
f=1.0;}

return f;
}

vec3 phong(vec3 n, vec3 l, vec3 v, vec3 diffuseC, vec3 diffuseF, vec3 specularC, vec3 specularF, float alpha, bool attenuate, vec3 attenuation) {
	float d = length(l);
	l = normalize(l);
	float att = 1.0;	
	if(attenuate) att = 1.0f / (attenuation.x + d * attenuation.y + d * d * attenuation.z);
	vec3 r = reflect(-l, n);
	vec3 light;

	diffuseF=diffuseF*att;
	specularF=specularF*att;
	if(!isStatic){
	light.r= (diffuseC.r * max(0, dot(n, l))); 
	light.g= (diffuseC.g * max(0, dot(n, l)));
	light.b= (diffuseC.b * max(0, dot(n, l)));
	//translate to hsv, but only v needed, v is max of any rgb value
	float value= max(light.r, light.g);
	value=max(value, light.b);
	value=discretize(value);
	light=light*value;
	}
	if(isStatic){
	//light+=texture(lightmap, normal_world).rgb;
	}
	light.b+=specularF.b * specularC.b * pow(max(0, dot(r, v)), alpha);
		light.r +=specularF.r * specularC.r * pow(max(0, dot(r, v)), alpha);
	light.g+= specularF.g * specularC.g * pow(max(0, dot(r, v)), alpha);
	
	return light;
}



void main() {	

	vec3 cam=-camera_world;


	vec3 n = normalize(vert.normal_world);
	vec3 v = normalize(cam- vert.position_world);
	
	vec3 texColor = texture(diffuseTexture, vert.uv).rgb;
	color= vec4(texColor.r * materialAmbient.r, texColor.g * materialAmbient.g,texColor.b * materialAmbient.b,1) ; // ambient
	if(glowing==true){
	glow=color;
	}
	else{glow=vec4(vec3(0),1);}
	// add directional light contribution
	color.rgb += phong(n, -dirL.direction, v, dirL.color * texColor, materialDiffuse, dirL.color, materialSpecular, specularAlpha, false, vec3(0));
			

	if(dot(v, n)<cos(radians(90))){color.rgb= vec3(0,1,1);}
}

