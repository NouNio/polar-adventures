#ifndef HUD_H
#define HUD_H

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>
#include <map>

#include <Shader.h>

struct Glyph {
	unsigned int TextureID;  // ID handle of the glyph texture
	glm::ivec2 Size;         // size of the glyph
	glm::ivec2 Bearing;      // offset from the basline to the top and bottom
	unsigned int Advance;    // offset to the next character
};

class HUD
{
public:
	HUD(const char* path)
	{
		this->fontPath = path;
		initFreetype();
		loadFont();
		FT_Set_Pixel_Sizes(face, 0, pixelHeight);  // width = 0 makes it be computed from the provided height
		fillAlphabet();
		freeFTResources();
		enableGLBlend();
		createVAOandVBO();
	}

	void render(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color)
	{
		// activate corresponding render state	
		shader.use();
		glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(VAO);

		// iterate through all characters
		std::string::const_iterator c;
		for (c = text.begin(); c != text.end(); c++)
		{
			Glyph ch = Alphabet[*c];

			float xpos = x + ch.Bearing.x * scale;
			float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

			float w = ch.Size.x * scale;
			float h = ch.Size.y * scale;
			// update VBO for each character
			float vertices[6][4] = {
				{ xpos,     ypos + h,   0.0f, 0.0f },
				{ xpos,     ypos,       0.0f, 1.0f },
				{ xpos + w, ypos,       1.0f, 1.0f },

				{ xpos,     ypos + h,   0.0f, 0.0f },
				{ xpos + w, ypos,       1.0f, 1.0f },
				{ xpos + w, ypos + h,   1.0f, 0.0f }
			};
			// render glyph texture over quad
			glBindTexture(GL_TEXTURE_2D, ch.TextureID);
			// update content of VBO memory
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			// render quad
			glDrawArrays(GL_TRIANGLES, 0, 6);
			// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
		}
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

private:
	unsigned int nSnowballs = 0;
	const char* fontPath;
	unsigned int pixelHeight = 48;
	FT_Library ft;
	FT_Face face;
	std::map<char, Glyph> Alphabet;
	unsigned int VAO, VBO;


	void initFreetype()
	{
		if (FT_Init_FreeType(&ft))
		{
			std::cout << "There was an error initializing the FreeType." << std::endl;;
		}
	}


	void loadFont()
	{
		if (FT_New_Face(ft, fontPath, 0, &face))
		{
			std::cout << "There was an error loading the font: " << fontPath << std::endl;
		}
	}


	void fillAlphabet()
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // disable byte-alignment restriction, cause we represent each chars color in one byte and usually ogl expects multiples of 4 bytes

		for (unsigned char c = 0; c < 128; c++)
		{
			// load character glyph 
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cout << "There was an error laoding the glyph: " << c << std::endl;
				continue;
			}

			unsigned int texture = generateTexture();
			setTextureOptions();

			// now store character for later use
			Glyph glyph = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};

			Alphabet.insert(std::pair<char, Glyph>(c, glyph));
		}
	}


	unsigned int generateTexture()
	{
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,						 // each byte of the greyscale bitmap corresponds to the texures red color val
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		return texture;
	}


	void setTextureOptions()
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}


	void freeFTResources()
	{
		FT_Done_Face(face);
		FT_Done_FreeType(ft);
	}


	void enableGLBlend()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}


	void createVAOandVBO()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
};

#endif