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

	mat3 normalMatrix= mat3(transpose(inverse(modelMatrix)));
	vec4 position_world_ = modelMatrix * vec4(position, 1);
	vec3 normal_world = normalMatrix * normal;
	vec3 n = normalize(normal_world);
	vec3 v = normalize(camera_world - position_world_.rgb);
	 // ambient

	// add directional light contribution
	//color.rgb += phong(n, -dirL.direction, v, dirL.color * texColor, materialDiffuse, dirL.color, materialSpecular, specularAlpha, false, vec3(0));
		//vec3 phong(vec3 n, vec3 l, vec3 v, vec3 diffuseC, vec3 diffuseF, vec3 specularC, vec3 specularF, float alpha, bool attenuate, vec3 attenuation) {
	



	vec3 l = normalize(v);	
	pos = reflect(l, n);
		

	gl_Position = viewProjMatrix *vec4( position_world_.rgb, 1);
 }