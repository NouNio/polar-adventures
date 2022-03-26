/*
* Copyright 2020 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/
#pragma once


#include <string>
#include <GL/glew.h>
#include "Utils.h"
#include "general/stb_image.h"

/*!
 * 2D texture
 */
class TwoDTexture
{
protected:
	GLuint _handle;
	bool _init;

public:
	/*!
	 * Creates a texture from a file
	 * @param file: path to the texture file (a DSS image)
	 */
	TwoDTexture(std::string file) {

		file = "assets/textures/" + file;
		int width, height, nrChannels;
		unsigned char* texture = stbi_load(file.c_str(), &width, &height, &nrChannels, 0);
		glGenTextures(1, &_handle);
		glBindTexture(GL_TEXTURE_2D, _handle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
	};
	~TwoDTexture() { _handle = 0; };
	TwoDTexture(unsigned int handle) :
			_handle(handle) {}
	void setTexture(std::shared_ptr<TwoDTexture> texture) {
		_handle = texture->_handle;
	}

	/*!
	 * Activates the texture unit and binds this texture
	 * @param unit: the texture unit
	 */
	void bind(unsigned int unit) {
		glActiveTexture(GL_TEXTURE0+unit);
		glBindTexture(GL_TEXTURE_2D, _handle);
	
	};
};
