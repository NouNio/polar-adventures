#version 430 core
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
uniform bool vertical;
uniform vec3 originpoint;

void main() {	
color=vec4(vec3(0),1);
float[7] gauss= {0.004,0.054,0.242,0.399, 0.242, 0.054, 0.004};
	vec3 texColor = texture(diffuseTexture, vert.uv).rgb;
	
	vec2 size= textureSize(diffuseTexture, 0);
	size= 1.0/size;
	for(int i=0; i<=gauss.length()/2; i++){
	vec2 coords1=vert.uv;
	vec2 coords2=vert.uv;
	if(vertical){
	coords2.y=coords2.y - (size.y*i);
	coords1.y=coords1.y + (size.y*i);}
	else{
	coords2.x=coords2.x - (size.x*i);
	coords1.x=coords1.x + (size.x*i);}
	color+=texture(diffuseTexture, coords2)*gauss[3-i];
	color+=texture(diffuseTexture, coords1)*gauss[3+i];
	}

	 // ambient
	// ambient
	}