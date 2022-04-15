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

void main() {	
color=vec4(vec3(0),1);
float[3][3] sobel= {
{1,2,1},
{0,0,0},
{-1,.2,.1}
};
	
	vec2 size= textureSize(diffuseTexture, 0);
	size= 1.0/size;
	int xhalf = sobel.length()/2;

	vec4 horizontal = vec4(0.0,0.0,0.0,1.0);
	vec4 vertical= vec4(0.0,0.0,0.0,1.0);
	for(int i=0; i<=sobel.length(); i++){
	int yhalf= sobel[i].length()/2;
		for(int j=0; j<=sobel[i].length(); j++){
			vec2 coords=vert.uv;
				coords.x=coords.x + (size.x*(i-xhalf));
				coords.y=coords.y + (size.y*(i-yhalf));
				horizontal+=texture(diffuseTexture, coords)*sobel[i][j];
				vertical+=texture(diffuseTexture, coords)*sobel[j][i];
	}
	}
	color.rgb = vec3(sqrt(dot(vertical, vertical)+dot(horizontal, horizontal)));
	//invert colours
	color.r=(1.0-color.r);
	color.g=(1.0-color.g);
	color.b=(1.0-color.b);
	}