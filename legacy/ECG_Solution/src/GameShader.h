#pragma once
/*
* Copyright 2020 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/
#pragma once

#include <GL\glew.h>
#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include "Utils.h"


/*!
 * Shader class that encapsulates all shader access
 */
class GameShader
{
protected:


	/*!
	 * Path to vertex shader and fragment shader
	 */
	std::string _vs, _fs, _gs;

	/*!
	 * Whether an internal shader is being used
	 */
	bool _useFileAsSource;

	/*!
	 * Stores the shader location names with their location IDs
	 */
	std::unordered_map<std::string, GLint> _locations;

	/*!
	 * Loads the specified vertex and fragment shaders
	 * (usually called in the constructor)
	 */
	GLuint loadShaders();

	/*!
	 * Loads a shader from a given file and compiles it
	 * @param file: path to the shader
	 * @param shaderType: type of the shader (e.g. GL_VERTEX_SHADER or GL_FRAGMENT_SHADER)
	 * @param handle: shader handle
	 * @return if the shader could be loaded
	 */

	/*
	std::ifstream vs("assets\\shaders\\vertex.txt");
    std::string srcvs((std::istreambuf_iterator<char>(vs)),std::istreambuf_iterator<char>());
    std::ifstream fs("assets\\shaders\\fragment.txt");
    std::string srcfs((std::istreambuf_iterator<char>(fs)),std::istreambuf_iterator<char>());
	*/
	GLenum loadShader(std::string filename, GLenum shaderType) {
		std::ifstream input("assets\\shader\\"+filename);
		std::string src((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>()); 
		GLenum id = glCreateShader(shaderType);
		const char* source = src.c_str();
		glShaderSource(id, 1, &source, NULL);

		glCompileShader(id);
		GLint status = 0;
		glGetShaderiv(id, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) {
			EXIT_WITH_ERROR("compiling shader failed");
		}
		return id;
	};


	/*



	/*!
	 * @param uniform: uniform string in shader
	 * @return the location ID of the uniform
	 */
	GLint getUniformLocation(std::string uniform) {
	return glGetUniformLocation(_handle, uniform.c_str());
	};

public:

	/*!
 * The shader program handle
 */
	GLuint _handle;


	/*!
	 * Default constructor of a simple color shader
	 */
	GameShader() {};

	/*!
	 * Shader constructor with specified vertex and fragment shader
	 * Loads and compiles the shader
	 * @param vs: path to the vertex shader
	 * @param fs: path to the fragment shader
	 */
	GameShader(std::string vs, std::string fs) {
		_vs = vs;
		_fs = fs;
		GLint isLinked = 0;
		GLenum shader = glCreateProgram();
		GLenum vertex = loadShader(vs, GL_VERTEX_SHADER);
		GLenum fragment = loadShader(fs, GL_FRAGMENT_SHADER);
		glAttachShader(shader, vertex);
		glAttachShader(shader, fragment);
		glProgramParameteri(shader, GL_PROGRAM_SEPARABLE, GL_TRUE);
		glLinkProgram(shader);
		logShaderErrors(shader);

		glGetProgramiv(shader, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			glDeleteProgram(shader);
			glDeleteShader(vertex);
			glDeleteShader(fragment);
			//EXIT_WITH_ERROR("linking shader failed");
		}

		glValidateProgram(shader);
		glDetachShader(shader, vertex);
		glDetachShader(shader, fragment);

		_handle = shader;
	
	};

	~GameShader() {
	};

	/*!
	 * Uses the shader with glUseProgram
	 */
	void use() { glUseProgram(_handle);};

	/*!
	 * Un-uses the shader
	 */
	void unuse() { glUseProgram(0); };

	/*!
	 * Sets an integer uniform in the shader
	 * @param uniform: the name of the uniform
	 * @param i: the value to be set
	 */
	void setUniform(std::string uniform, const int i)
	{
		GLint pos = getUniformLocation(uniform);
		glUniform1i(pos, i);
	
	};
	/*!
	 * Sets an integer uniform in the shader
	 * @param location: location ID of the uniform
	 * @param i: the value to be set
	 */
	void setUniform(GLint location, const int i) {
		glUniform1i(location, i);
	};
	/*!
	 * Sets an unsigned integer uniform in the shader
	 * @param uniform: the name of the uniform
	 * @param i: the value to be set
	 */
	void setUniform(std::string uniform, const unsigned int i) {
		GLint pos = getUniformLocation(uniform);
		glUniform1ui(pos, i);
	};
	/*!
	 * Sets an unsigned integer uniform in the shader
	 * @param location: location ID of the uniform
	 * @param i: the value to be set
	 */
	void setUniform(GLint location, const unsigned int i) {
		glUniform1ui(location, i);
	};
	/*!
	 * Sets a float uniform in the shader
	 * @param uniform: the name of the uniform
	 * @param f: the value to be set
	 */
	void setUniform(std::string uniform, const float f) {
		GLint pos = getUniformLocation(uniform);
		glUniform1f(pos, f);
	};
	/*!
	 * Sets a float uniform in the shader
	 * @param location: location ID of the uniform
	 * @param f: the value to be set
	 */
	void setUniform(GLint location, const float f) {
		glUniform1f(location, f);
	
	};
	/*!
	 * Sets a 4x4 matrix uniform in the shader
	 * @param uniform: the name of the uniform
	 * @param mat: the value to be set
	 */
	void setUniform(std::string uniform, const glm::mat4& mat) {
		GLint pos = getUniformLocation(uniform);
		glUniformMatrix4fv(pos, 1, GL_FALSE, glm::value_ptr(mat));
	};
	/*!
	 * Sets a 4x4 matrix uniform in the shader
	 * @param location: location ID of the uniform
	 * @param mat: the value to be set
	 */
	void setUniform(GLint location, const glm::mat4& mat) {
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
	};
	/*!
	 * Sets a 3x3 matrix uniform in the shader
	 * @param uniform: the name of the uniform
	 * @param mat: the value to be set
	 */
	void setUniform(std::string uniform, const glm::mat3& mat) {
		GLint pos = getUniformLocation(uniform);
		glUniformMatrix3fv(pos, 1, GL_FALSE, glm::value_ptr(mat));
	};
	/*!
	 * Sets a 3x3 matrix uniform in the shader
	 * @param location: location ID of the uniform
	 * @param mat: the value to be set
	 */
	void setUniform(GLint location, const glm::mat3& mat) {
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(mat));
	};
	/*!
	 * Sets a 2D vector uniform in the shader
	 * @param uniform: the name of the uniform
	 * @param vec: the value to be set
	 */
	void setUniform(std::string uniform, const glm::vec2& vec) {
		GLint pos= getUniformLocation(uniform);
		glUniform2f(pos, vec.x, vec.y);
	
	};
	/*!
	 * Sets a 2D vector uniform in the shader
	 * @param location: location ID of the uniform
	 * @param vec: the value to be set
	 */
	void setUniform(GLint location, const glm::vec2& vec) {
		glUniform2f(location, vec.x, vec.y);
	};
	/*!
	 * Sets a 3D vector uniform in the shader
	 * @param uniform: the name of the uniform
	 * @param vec: the value to be set
	 */
	void setUniform(std::string uniform, const glm::vec3& vec) {
		GLint pos = getUniformLocation(uniform);
		glUniform3f(pos, vec.x, vec.y, vec.z);
	};
	/*!
	 * Sets a 3D vector uniform in the shader
	 * @param location: location ID of the uniform
	 * @param vec: the value to be set
	 */
	void setUniform(GLint location, const glm::vec3& vec) {
		glUniform3f(location, vec.x, vec.y, vec.z);
	};
	/*!
	 * Sets a 4D vector uniform in the shader
	 * @param uniform: the name of the uniform
	 * @param vec: the value to be set
	 */
	void setUniform(std::string uniform, const glm::vec4& vec) {
		GLint pos = getUniformLocation(uniform);
		glUniform4f(pos, vec.x, vec.y, vec.z, vec.w);
	};
	/*!
	 * Sets a 4D vector uniform in the shader
	 * @param location: location ID of the uniform
	 * @param vec: the value to be set
	 */
	void setUniform(GLint location, const glm::vec4& vec) {
		glUniform4f(location, vec.x, vec.y, vec.z, vec.w);
	
	};
	/*!
	 * Sets a uniform array property
	 * @param arr: name of the uniform array
	 * @param i: index of the value to be set
	 * @param prop: property name
	 * @param vec: the value to be set
	 */
	//void setUniformArr(std::string arr, unsigned int i, std::string prop, const glm::vec3& vec);
	/*!
	 * Sets a uniform array property
	 * @param arr: name of the uniform array
	 * @param i: index of the value to be set
	 * @param prop: property name
	 * @param f: the value to be set
	 */
	//void setUniformArr(std::string arr, unsigned int i, std::string prop, const float f);

	private:
		// Log ShaderErrors.
		void logShaderErrors(GLenum shader) {
			GLint success;
			GLchar infoLog[1024];
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR: " << infoLog << "\n" << std::endl;
			}
		}
}; 

