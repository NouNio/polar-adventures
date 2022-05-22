#version 330 core
layout(location=0) out vec4 FragColor;
layout(location=1) out vec4 normal;
layout(location=2) out vec4 depth;

in vec2 TexCoords;
in vec3 n;
in vec3 position;
uniform sampler2D texture_diffuse;
uniform samplerCube skybox;
uniform vec3 viewPos;
void main()
{    
    FragColor = texture(texture_diffuse, TexCoords);
    vec3 norm=normalize(n);
         //vec3 viewDir=normalize(position-viewPos);
       //  vec3 reflectDir=reflect(viewDir, norm);
     //vec3 reflection  = texture(skybox, reflectDir).rgb;

      //if (FragColor.r<0.35){
      //FragColor.rgb=mix(FragColor.rgb, reflection, 0.8);
      //FragColor.rgb+=reflection;
      //}
    normal= vec4(norm,1);
    depth=vec4(vec3(0),1);
}