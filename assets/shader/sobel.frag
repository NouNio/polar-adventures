#version 430 core

out vec4 color;


in vec2 TexCoords;




//uniform vec3 materialCoefficients; // x = ambient, y = diffuse, z = specular

uniform sampler2D diffuseTexture;
uniform sampler2D depthText;

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
			vec2 coords=TexCoords;
				coords.x=coords.x + (size.x*(i-xhalf));
				coords.y=coords.y + (size.y*(i-yhalf));
				horizontal+=texture(diffuseTexture, coords)*sobel[i][j];
				vertical+=texture(diffuseTexture, coords)*sobel[j][i];
	}
	}
	color.rgb = vec3(sqrt(dot(vertical, vertical)+dot(horizontal, horizontal)));
	//invert colours
	}