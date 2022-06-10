#pragma once
#ifndef HUD_H
#define HUD_H

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>
#include <vector>
#include <map>
#include <cmath>

#include <Shader.h>
#include <Physics.h>
#include <KinematicPlayer.h>
#include <Constants.h>
#include <sstream>


/* ------------------------------------------------------------------------------------ */
// the HUD is inspired by and closely following the tutorial from [1], 
// as also mentioned in Tips & Tricks
/* ------------------------------------------------------------------------------------ */

extern std::vector<Snowball*> collectedSnowballs;
extern double score;
extern int currRenderedObjects;

struct Glyph {
	unsigned int TextureID;  // ID handle of the glyph texture
	glm::ivec2 Size;         // size of the glyph
	glm::ivec2 Bearing;      // offset from the basline to the top and bottom
	unsigned int Advance;    // offset to the next character
};

class HUD
{
public:
	HUD(std::string path)
	{
		this->fontPath = path.c_str();
		initFreetype();
		loadFont();
		FT_Set_Pixel_Sizes(face, 0, pixelHeight);  // width = 0 makes it be computed from the provided height
		fillAlphabet();
		freeFTResources();
		enableGLBlend();
		createVAOandVBO();
	}

	void renderLine(Shader& shader, std::string text, float x, float y, glm::vec3 textColor= glm::vec3(0.1f, 0.6f, 0.9f))
	{
		// activate corresponding render state	
		shader.use();
		glUniform3f(glGetUniformLocation(shader.ID, "textColor"), textColor.x, textColor.y, textColor.z);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(VAO);

		// iterate through all characters
		std::string::const_iterator c;
		for (c = text.begin(); c != text.end(); c++)
		{
			Glyph ch = Alphabet[*c];

			float xpos = x + ch.Bearing.x * textScale;
			float ypos = y - (ch.Size.y - ch.Bearing.y) * textScale;

			float w = ch.Size.x * textScale;
			float h = ch.Size.y * textScale;
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
			x += (ch.Advance >> 6) * textScale; // bitshift by 6 to get value in pixels (2^6 = 64)
		}
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}


	void renderAll(Shader& shader, float x, float y)
	{
		for (unsigned int i = 0; i < messages.size(); i++)
		{
			renderLine(shader, messages[i], x, y - i * newLineOffset);   // set each message 20 below the first
		}
	}

	void renderNumbers(Shader& shader, float x, float y, std::vector<int> numbers, std::vector<glm::vec2> offsets, std::vector<glm::vec3> colours, float scale)
	{
		float ts = textScale;
		textScale = 0.25;
		for (unsigned int i = 0; i < numbers.size(); i++)
		{
			std::string text= std::to_string(numbers[i]);
			switch (numbers[i]) {
			case 0:
				text = "TOP";
				break;
			case 1:
				text = "BACK";
				break;
			case 2:
				text = "RIGHT";
				break;
			case 3:
				text = "LEFT";
				break;
			case 4:
				text = "FRONT";
				break;
			case 5:
				text = "BOTTOM";
				break;
			}
		
			//text = std::to_string(numbers[i]);
			glm::vec2 offset = offsets[i]*scale;
			if (i == 3) { offset += glm::vec2(scale * 0.25); 
			textScale /= 2.0;
			}
			renderLine(shader, text, x + offset.x, y + offset.y, colours[i]);   // set each message 20 below the first
			if (i == 3) {
				textScale *= 2.0;
			}
		}
		textScale = ts;

	}

	void renderEndOfGame(Shader& shader, float x, float y) {
		renderLine(shader, "YOUR SCORE: " +  std::to_string(lround(score)), x, y - 0 * newLineOffset);
		renderLine(shader, "HIGH SCORE: " + std::to_string(highScore), x, y - 1 * newLineOffset);
	}


	void update(Camera* camera, double FPS, double msPerFrame, Physics* pHandler, KinematicPlayer* controller)
	{
		this->messages[1] = "Snowballs in pocket: " + std::to_string(controller->getSnowBallAmmo());
		this->messages[2] = "Snowballs in  world: " +  std::to_string(snowballs.size());
		this->messages[3] = "Points: " +  std::to_string(lround(score));
		this->messages[4] = "Player.pos X: " +  std::to_string(lround(controller->getPos().x));
		this->messages[5] = "Player.pos Y: " +  std::to_string(lround(controller->getPos().y));
		this->messages[6] = "Player.pos Z: " +  std::to_string(lround(controller->getPos().z));
		this->messages[7] = "FPS : " +  std::to_string(lround(FPS));
		this->messages[8] = "ms / frame: " +  std::to_string(llround(msPerFrame));
		this->messages[9] = "Number rendered objects: " +  std::to_string(currRenderedObjects);
		this->messages[10] = "Number RigidBodies: " +  std::to_string(pHandler->getNumBodies());
	}


private:
	const char* fontPath;
	unsigned int nSnowballs = 0;
	unsigned int pixelHeight = 48;
	float textScale = 0.5;
	const float newLineOffset = 25.0;
	const glm::vec3 textColor = glm::vec3(0.1f, 0.6f, 0.9f);
	FT_Library ft;
	FT_Face face;
	std::map<char, Glyph> Alphabet;
	unsigned int VAO, VBO;
	std::vector<std::string> messages = { "----- Data -----", 
									 "Snowballs in pocket: ",
									 "Snowballs in  world: ",
									 "Points: ",
		                             "Player.pos X: ", "Player.pos Y: ", "Player.pos Z: ",
	                                 "FPS: ", "ms / frame: ", 
									 "Number rendered objects: ",
								     "Num RigidBodies: "};


	void initFreetype()
	{
		if (FT_Init_FreeType(&ft))
		{
			std::cout << "There was an error initializing the FreeType." << std::endl;;
		}
	}


	void loadFont(){
		if (FT_New_Face(ft, fontPath, 0, &face))
		{
			std::cout << "There was an error loading the font: " << fontPath << std::endl;
		}
	}


	void fillAlphabet(){
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
