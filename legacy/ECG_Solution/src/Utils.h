/*
* Copyright 2020 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/
#pragma once

#include "INIReader.h"
#include <iostream>
#include <Windows.h>
#include <memory>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\glm.hpp>


#define EXIT_WITH_ERROR(err) \
        { \
                glfwTerminate(); \
                std::cout << "ERROR: " << err << std::endl; \
                system("PAUSE"); \
                return EXIT_FAILURE; \
        }


#define FOURCC_DXT1	MAKEFOURCC('D', 'X', 'T', '1')
#define FOURCC_DXT3	MAKEFOURCC('D', 'X', 'T', '3')
#define FOURCC_DXT5	MAKEFOURCC('D', 'X', 'T', '5')

//*!
 /* A loaded '.dss' image
 
class DDSImage {
public:
	unsigned char* data;
	unsigned int width;
	unsigned int height;
	unsigned int size;
	GLenum format;

	DDSImage() : data(nullptr), width(0), height(0), size(0), format(GL_NONE) {}
	DDSImage(const DDSImage& img) = delete;
	DDSImage(DDSImage&& img) : data(img.data), width(img.width), height(img.height), size(img.size), format(img.format) {
		img.data = nullptr;
	}
	DDSImage& operator=(const DDSImage& img) = delete;
	DDSImage& operator=(DDSImage&& img) {
		data = img.data;
		img.data = nullptr;
		width = img.width;
		height = img.height;
		size = img.size;
		format = img.format;
		return *this;
	};

	~DDSImage() { if (data != nullptr) { delete[] data; data = nullptr; } }
};


/* --------------------------------------------- */
// Framework functions
/* --------------------------------------------- */

/*!
 * Initializes the framework
 * Do not overwrite this function!
 */
//bool initFramework() { return true; };


/*!
 * Destroys the framework
 * Do not overwrite this function!
 */
//void destroyFramework(){};

/*!
 * Loads a '.dss' image from a file
 * @param file: the path to the image file
 * @return a loaded DSS image
 *
DDSImage loadDDS(const char* path) {

	unsigned char *header;

	unsigned int width;
	unsigned int height;
	unsigned int mipMapCount;

	unsigned int blockSize;
	unsigned int format;

	unsigned int w;
	unsigned int h;

	unsigned char* buffer = 0;

	// open the DDS file for binary reading and get file size
	FILE* f;
	DDSImage image = DDSImage();
	if ((f = fopen(path, "rb")) == 0)
		return image;
	fseek(f, 0, SEEK_END);
	long file_size = ftell(f);
	fseek(f, 0, SEEK_SET);

	// allocate new unsigned char space with 4 (file code) + 124 (header size) bytes
	// read in 128 bytes from the file
	header = (unsigned char*) malloc(128);
	fread(header, 1, 128, f);

	// compare the `DDS ` signature
	if (memcmp(header, "DDS ", 4) != 0)
		goto exit;

	// extract height, width, and amount of mipmaps - yes it is stored height then width
	height = (header[12]) | (header[13] << 8) | (header[14] << 16) | (header[15] << 24);
	width = (header[16]) | (header[17] << 8) | (header[18] << 16) | (header[19] << 24);
	mipMapCount = (header[28]) | (header[29] << 8) | (header[30] << 16) | (header[31] << 24);
	image.height = height;
	image.width = width;
	// figure out what format to use for what fourCC file type it is
	// block size is about physical chunk storage of compressed data in file (important)
	if (header[84] == 'D') {
		switch (header[87]) {
		case '1': // DXT1
			format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			blockSize = 8;
			break;
		case '3': // DXT3
			format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			blockSize = 16;
			break;
		case '5': // DXT5
			format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			blockSize = 16;
			break;
		case '0': // DX10
			// unsupported, else will error
			// as it adds sizeof(struct DDS_HEADER_DXT10) between pixels
			// so, buffer = malloc((file_size - 128) - sizeof(struct DDS_HEADER_DXT10));
		default: goto exit;
		}
		image.format = format;
	}
	else // BC4U/BC4S/ATI2/BC55/R8G8_B8G8/G8R8_G8B8/UYVY-packed/YUY2-packed unsupported
		goto exit;

	// allocate new unsigned char space with file_size - (file_code + header_size) magnitude
	// read rest of file
	buffer = (unsigned char*)malloc(file_size - 128);
	if (buffer == 0)
		goto exit;
	fread(buffer, 1, file_size, f);
	image.data = buffer;
	// prepare new incomplete texture

	// prepare some variables

	// easy macro to get out quick and uniform (minus like 15 lines of bulk)
exit:
	free(buffer);
	free(header);
	fclose(f);
};*/
