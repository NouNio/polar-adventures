#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>  // one could also use GLAD here
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// this code is inspired by learnopengl.com as this is my main resource for opengl information
// other resources my have been influential as well, this file howver should describes my way of implementing the theoretical concepts

// the procedure can roughly be described in 4 steps
// 1. load the shader files and extract the code
// 2. compile the shaders
// 3. attach the shaders to the program and link the program
// 4. delete the shaders

// constants
#define INFO_LOG_SIZE 1024
const std::string VERTEX = "VERTEX";
const std::string FRAGMENT = "FRAGMENT";
const std::string PROGRAM = "PROGRAM";
const std::string SEPARATOR = "\n -- --------------------------------------------------- -- ";

class Shader
{
public:
    unsigned int ID;
    std::string path;

    Shader(std::string vertexShaderPath, std::string fragmentShaderPath)
    {
        // 1. first we need to read the source code in the shader files, basically char by char
        std::string vertexShaderCode = readShaderFile(vertexShaderPath);
        std::string fragmentShaderCode = readShaderFile(fragmentShaderPath);
        this->path = vertexShaderPath;
        
        // 2. compile the shaders and 3. link the program (+ 4. delete the shaders)
        compileShadersAndLinkProgram(vertexShaderCode.c_str(), fragmentShaderCode.c_str());
    }


    void use() const
    {
        glUseProgram(this->ID);
    }


    // the following functions are for setting respective uniforms in the shader, by specifing the name and value of the uniform to take
    void setBool(const std::string& name, bool val) const
    {
        glUniform1i(glGetUniformLocation(this->ID, name.c_str()), (int)val); // transform val to 0 or 1
    }


    void setInt(const std::string& name, int val) const
    {
        glUniform1i(glGetUniformLocation(this->ID, name.c_str()), val);
    }


    void setFloat(const std::string& name, float val) const
    {
        glUniform1f(glGetUniformLocation(this->ID, name.c_str()), val);
    }


    void setVec2(const std::string& name, const glm::vec2& val) const
    {
        glUniform2fv(glGetUniformLocation(this->ID, name.c_str()), 1, &val[0]);
    }


    void setVec2(const std::string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(this->ID, name.c_str()), x, y);
    }


    void setVec3(const std::string& name, const glm::vec3& val) const
    {
        glUniform3fv(glGetUniformLocation(this->ID, name.c_str()), 1, &val[0]);
    }


    void setVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(this->ID, name.c_str()), x, y, z);
    }


    void setVec4(const std::string& name, const glm::vec4& val) const
    {
        glUniform4fv(glGetUniformLocation(this->ID, name.c_str()), 1, &val[0]);
    }


    void setVec4(const std::string& name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(this->ID, name.c_str()), x, y, z, w);
    }


    void setMat2(const std::string& name, const glm::mat2& mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(this->ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }


    void setMat3(const std::string& name, const glm::mat3& mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(this->ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }


    void setMat4(const std::string& name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(this->ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    // read the shader file and return the contained source code
    std::string readShaderFile(std::string shaderPath)
    {
        std::string shaderCode;
        std::ifstream shaderFile;
        std::stringstream shaderStream;
        shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);  // make ifstream object throw the respective exceptions

        try  // try to open the specified file
        {
            shaderFile.open(shaderPath);
            shaderStream << shaderFile.rdbuf();  // transfer the file buffers into the stream objects
            shaderFile.close();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "There was a problem reading this file: "<<shaderPath << e.what() << std::endl;
        }
        return shaderStream.str();  // now we can transform the stream object into string objects
    }


    // compile the shaders, link to the program and delete the shaders
    void compileShadersAndLinkProgram(const char* vertexShaderPtr, const char* fragmentShaderPtr)
    {
        // 2. compile the shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);          // create openGL shader
        glShaderSource(vertex, 1, &vertexShaderPtr, NULL);  // give it the shader code from the shader file
        glCompileShader(vertex);                            // compile
        checkPrintCompileErrors(vertex, VERTEX);            // check errors
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fragmentShaderPtr, NULL);
        glCompileShader(fragment);
        checkPrintCompileErrors(fragment, FRAGMENT);

        // 3. attach and link the shaders to the shader Program
        this->ID = glCreateProgram();
        glAttachShader(this->ID, vertex);
        glAttachShader(this->ID, fragment);
        glLinkProgram(this->ID);
        checkPrintCompileErrors(this->ID, PROGRAM);

        // 4. we can delete the shaders now (after linking)
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }


    // check and print comile errors based on the given thrown error
    void checkPrintCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[INFO_LOG_SIZE];
        if (type != PROGRAM)
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, INFO_LOG_SIZE, NULL, infoLog);
                std::cout << "There was an error compiling this shader: " << type << "  path: " << this->path << "\n" << infoLog << SEPARATOR << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, INFO_LOG_SIZE, NULL, infoLog);
                std::cout << "There was an error linking the program: " << type << "\n" << this->path << infoLog << SEPARATOR << std::endl;
            }
        }
    }
};
#endif

