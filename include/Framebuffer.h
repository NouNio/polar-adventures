#include <GL/glew.h>
#include <gl/GLU.h>
#include <GLFW/glfw3.h>
#include <Mesh.h>
class Framebuffer {
//private:
	
public:
	unsigned int handle;
	unsigned int color;
	unsigned int normal;
	unsigned int depth;
	unsigned int postprocessor;
	unsigned int edge;
	Framebuffer(int window_width, int window_height)
	{
		/*
	_handle=0;
	color=0;
	normal=0;
	depth=0;
	_post_processor=0;
	edge=0;


	*/
		//generate textures and framebuffers
		
			glGenTextures(1, &color);
			glGenTextures(1, &normal);
			glGenTextures(1, &depth);
			glGenTextures(1, &edge);
			glGenFramebuffers(1, &handle);
			glGenFramebuffers(1, &postprocessor);
			 

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
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, window_width, window_height, 0, GL_DEPTH_COMPONENT32, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the edge filter would otherwise sample repeated texture values!
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
			glBindTexture(GL_TEXTURE_2D, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth, 0);


		}
		//finish fbo initialization
		{

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				//EXIT_WITH_ERROR("failed to create framebuffer");
			}
			glClearColor(0, 0, 0, 0);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glBindTexture(GL_TEXTURE_2D, 0);
			GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_DEPTH_ATTACHMENT};
		
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
		glDeleteFramebuffers(1, &handle);
		glDeleteFramebuffers(1, &postprocessor);

	}
	static Mesh renderMesh() {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices = { 0,1,2,0,2,3 };
		Material material;
		std::vector<float> bound = { 0,0 };
		return Mesh(vertices, indices, material, bound, bound, bound);
		//	    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Material material, Texture texture,
		//std::vector<float> xBound, std::vector<float> yBound, std::vector<float> zBound, bool withTexture)
	}


};