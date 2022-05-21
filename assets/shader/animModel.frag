#version 330 core
layout(location=0) out vec4 FragColor;
layout(location=1) out vec4 normal;
layout(location=2) out vec4 depth;

in vec2 TexCoords;
in vec3 n;
in vec3 position;
uniform sampler2D texture_diffuse;

void main()
{    
    FragColor = texture(texture_diffuse, TexCoords);
    normal= vec4(normalize(n),1);
    depth=vec4(vec3(length(position)),1);
}