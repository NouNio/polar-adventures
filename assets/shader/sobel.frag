#version 430 core
//partly applied from https://gist.github.com/Hebali/6ebfc66106459aacee6a9fac029d0115, mostly because apparently float arrays don't work the way they should but fine.
//it should have worked. I have no idea why the hell it didn't. I'm guessing glsl being glsl again or something
out vec4 color;


in vec2 TexCoords;




//uniform vec3 materialCoefficients; // x = ambient, y = diffuse, z = specular

uniform sampler2D diffuseTexture;
uniform sampler2D depthText;



void main(void) 
{
const int kernelsize=3;
	vec4 n[kernelsize][kernelsize];
	int halfkernel = kernelsize/2;
	vec2 sizes= textureSize(diffuseTexture,0);
	sizes=1.0/sizes;
	sizes=sizes/halfkernel;
	for(int i=0; i<kernelsize;i++){
	for(int j=0; j<kernelsize;j++){
	vec2 coords = TexCoords + vec2((i-halfkernel)*sizes.x, (j-halfkernel)*sizes.y);
	n[i][j]= texture(diffuseTexture, coords);
	}
	}
	vec4 v[kernelsize][kernelsize];
		for(int i=0; i<kernelsize;i++){
	for(int j=0; j<kernelsize;j++){
	vec2 coords = TexCoords + vec2((i-halfkernel)*sizes.x, (j-halfkernel)*sizes.y);
	v[i][j]= texture(depthText, coords);
	}
	}
	vec4 sobel_edge_h=vec4(0);
	vec4 sobel_edge_h_=vec4(0);
	vec4 sobel_edge_v=vec4(0);
	vec4 sobel_edge_v_=vec4(0);

	vec3 kernelvalues =vec3(1.0,2.0,1.0);
	//For some reason that didn't work with an array of floats - because of some compile error
	//for some reason this wont work but fine
	//for(int i=0; i<kernelsize;i++){
		//	vec3 value= (i-halfkernel)*kernelvalues;
			//for(int j=0; j<kernelsize;j++){
			//sobel_edge_h+=(n[i][j]*value[j]);
			//sobel_edge_h_+=(v[i][j]*value[j]);

			//sobel_edge_v=+(n[j][i]*value[j]);
			//sobel_edge_v_+=(v[j][i]*value[j]);
	//}
	//}

	//if you dont want loops, i guess?
	 sobel_edge_h = (n[0][0] + (2.0*n[1][0]) + n[2][0])-(n[0][2] + (2.0*n[1][2]) + n[2][2]) ;
  	 sobel_edge_v = (n[0][0] + (2.0*n[0][1]) + n[0][2]) - (n[2][0] + (2.0*n[2][1]) + n[2][2]);
	 sobel_edge_h_ = (v[0][0] + (2.0*v[1][0]) + v[2][0])-(v[0][2] + (2.0*v[1][2]) + v[2][2]) ;
  	 sobel_edge_v_ = (v[0][0] + (2.0*v[0][1]) + v[0][2]) - (v[2][0] + (2.0*v[2][1]) + v[2][2]);
	vec3 sobel = vec3(sqrt(length(sobel_edge_h ) + length(sobel_edge_v)));
	vec3 sobel_ = vec3(sqrt(length(sobel_edge_h ) + length(sobel_edge_v)));
	sobel=sobel+sobel_;
	color = vec4( sobel.rgb, 1.0 );
}
	//invert colours
	//color.rgb= vec3(1.0);