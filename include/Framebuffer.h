#include <GL/glew.h>
#include <gl/GLU.h>
#include <GLFW/glfw3.h>
#include <Mesh.h>
#include <FileManager.h>
class Framebuffer {
//private:
	
public:
	unsigned int handle;

	unsigned int color;
	unsigned int normal;
	unsigned int depth;
	unsigned int postprocessor;
	unsigned int edge;
	unsigned int rbo;
	Mesh renderMesh;
	Shader processor;
	Shader combination;
	FileManager fm  =  FileManager();
	Framebuffer(FileManager &fm, int window_width, int window_height) ://fm(&fm),
		renderMesh({ Vertex(glm::vec3(-1,-1,0), glm::vec2(0,0)),Vertex(glm::vec3(1,-1,0),glm::vec2(1,0)),Vertex(glm::vec3(1,1,0),glm::vec2(1,1)),Vertex(glm::vec3(-1,1,0),glm::vec2(0,1)) }, { 0,1,2,0,2,3 }, Material(), { 0.0f,0.0f }, { 0.0f,0.0f }, { 0.0f,0.0f }), //processor(this->fm->getShaderPath("pass_on"), this->fm->getShaderPath("sobel")),
		combination(fm.getShaderPath("passOn.vert", true), fm.getShaderPath("sobel.frag",true)),
		processor(fm.getShaderPath("passOn.vert", true), fm.getShaderPath("screen.frag",true)) {
		//this->fm = &fm;
			glGenTextures(1, &color);
			glGenTextures(1, &normal);
			glGenTextures(1, &depth);
			glGenTextures(1, &edge);
			glGenFramebuffers(1, &handle);
			glGenFramebuffers(1, &postprocessor);
			glGenRenderbuffers(1, &rbo);
			 

		bindBuffer();
		//setup color texture
		{
			glBindTexture(GL_TEXTURE_2D, color);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// attach texture to framebuffer
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);
		}
		//setup normal texture
		{
			glBindTexture(GL_TEXTURE_2D, normal);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the edge filter would otherwise sample repeated texture values!
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			// attach texture to framebuffer
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normal, 0);
		}
		//setup depth texture
		{
			glBindTexture(GL_TEXTURE_2D, depth);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_R, window_width, window_height, 0, GL_R, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the edge filter would otherwise sample repeated texture values!
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
			glBindTexture(GL_TEXTURE_2D, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, depth, 0);


		}

		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window_width, window_height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
		//finish fbo initialization
		{

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				std::cout<<"fbo init failed for fbo1" << std::endl;
			}
			glClearColor(0, 0, 0, 0);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glBindTexture(GL_TEXTURE_2D, 0);
			GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
		
			glDrawBuffers(3, attachments);

		}



		glBindFramebuffer(GL_FRAMEBUFFER, postprocessor);

		//init edge texture
		{

			glBindTexture(GL_TEXTURE_2D, edge);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// attach texture to framebuffer
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, edge, 0);
			GLenum attachments[] = { GL_COLOR_ATTACHMENT0};
			glDrawBuffers(1,  attachments);
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			}




			glBindFramebuffer(GL_FRAMEBUFFER, 0);

		};






	}
	//*/
	void bindBuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, handle);
	}
	void doImageProcessing() {
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		//*
		//bindPostProcessor();
		processor.use();
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(processor.ID, "diffuseTexture"),0);  // set the appropriate texture sampler variable in the fragment shader
		glBindTexture(GL_TEXTURE_2D, normal);
		glActiveTexture(GL_TEXTURE1);
		glUniform1i(glGetUniformLocation(processor.ID, "depthText"), 0);  // set the appropriate texture sampler variable in the fragment shader
		glBindTexture(GL_TEXTURE_2D, normal);
		renderMesh.draw(processor);
		//*/
		//*
		unbind();
		combination.use();
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(processor.ID, "diffuseTexture"), 0);  // set the appropriate texture sampler variable in the fragment shader
		glBindTexture(GL_TEXTURE_2D, color);
		glActiveTexture(GL_TEXTURE1);
		glUniform1i(glGetUniformLocation(processor.ID, "edge"), 0);  // set the appropriate texture sampler variable in the fragment shader
		glBindTexture(GL_TEXTURE_2D, normal);
		renderMesh.draw(combination);
		glUseProgram(0);
		glEnable(GL_DEPTH_TEST);
		//*/

	}
	void bindPostProcessor() {
		glBindFramebuffer(GL_FRAMEBUFFER, postprocessor);
	}
	unsigned int getDepth() {
		return depth;
	}

	unsigned int getNormal() {
		return normal;
	}
	unsigned int getColor() {
		return color;
	}
	unsigned getEdge() {
		return edge;
	}
	static void unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	~Framebuffer() {
		glDeleteTextures(1, &color);
		glDeleteTextures(1, &normal);
		glDeleteTextures(1, &depth);
		glDeleteTextures(1, &edge);
		glDeleteProgram(processor.ID);
		glDeleteProgram(combination.ID);
		glDeleteFramebuffers(1, &handle);
		glDeleteFramebuffers(1, &postprocessor);
		glDeleteRenderbuffers(1, &rbo);

	}


};