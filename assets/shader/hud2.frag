#version 330 core

out vec4 color;
uniform float transparency;
uniform vec3 colour;  // for altering the text color of the HUD

void main()
{
//.r stored the textures data, i.e. the alpha value
	color = vec4(colour, (1.0-transparency));                  // vary the color by the alpha, also makes the background transparent where there is no pixel of the glyph and vice versa
}