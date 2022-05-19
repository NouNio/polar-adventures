//FOR SOME REASON THE SHADERS DIDNT WORKS SO I TOOK THIS AND THE meSH RENNDERNIG FROM LEARNOPENGL BECAUSE FBOs ARE SHIT
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}