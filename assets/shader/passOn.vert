//FOR SOME REASON THE SHADERS DIDNT WORKS SO I TOOK THIS AND THE meSH RENNDERNIG FROM LEARNOPENGL BECAUSE FBOs ARE SHIT, and what am i to do wiht this, this is the simplest shader ever, and i already had literally the same shit last year, i guess the glPosition got lost or something...
layout (location = 0) in vec3 Pos;

layout (location = 1) in vec2 Coords;

out vec2 TexCoords;

void main()
{
    TexCoords = Coords;
    gl_Position = vec4(Pos, 1.0);
}