#version 330 core
in vec3 FragPos;
in vec3 Normal;   
in vec2 TexCoords;
flat in ivec4 BoneIDs;
in vec4 Weights;

out vec4 FragColor;
//out vec4 normal;


uniform sampler2D texture_diffuse;


void main() {
	vec3 norm = normalize(Normal);                // ambiguously the vector "Normal" is not normalized yet (necesserily), only perpendicular
    //normal=vec4(norm,1.0);
    //FragColor = normal;

	FragColor = texture(texture_diffuse, TexCoords);
}