#version 330 core
in vec2 TexCoords;

out vec4 color;

uniform sampler2D text;  // mono color bitmap from FreeType loading the font file
uniform vec3 textColor;  // for altering the text color of the HUD
uniform float brightness;
void main()
{
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);  //.r stored the textures data, i.e. the alpha value
	color = vec4(textColor*brightness, 1.0) * sampled;                          // vary the color by the alpha, also makes the background transparent where there is no pixel of the glyph and vice versa
}