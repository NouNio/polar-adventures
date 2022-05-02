#include <GL/glew.h>
#include <gl/GLU.h>
#include <GLFW/glfw3.h>
#include <Mesh.h>
class Framebuffer {
public:
	Framebuffer(int window_width, int window_height, bool isPostProcessing = false)
	{
		//generate textures and framebuffers
		{
			glGenTextures(1, &color);
			glGenTextures(1, &normal);
			glGenTextures(1, &depth);
			glGenTextures(1, &edge);
			glGenFramebuffers(1, &_handle);
			glGenFramebuffers(1, &_post_processor); }

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
			glDrawBuffers(3, { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_DEPTH_ATTACHMENT });

		}



		glBindFramebuffer(GL_FRAMEBUFFER, _post_processor);

		//init edge texture
		{

			glBindTexture(GL_TEXTURE_2D, edge);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// attach texture to framebuffer
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, edge, 0);
			glDrawBuffers(1, GL_COLOR_ATTACHMENT0);
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			}




			glBindFramebuffer(GL_FRAMEBUFFER, 0);

		};






	}
	//*/
	void bindBuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, _handle);
	}
	void bindPostProcessor() {
		glBindFramebuffer(GL_FRAMEBUFFER, _post_processor);
	}
	unsigned int depth() {
		return depth;
	}
	unsigned int normal() {
		return normal;
	}
	unsigned int color() {
		return color;
	}
	unsigned edge() {
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
		glDeleteFramebuffers(1, &_handle);
		glDeleteFramebuffers(1, &_post_processor);

	}
	static Mesh renderMesh() {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		Material material;
		std::vector<float> bound = { 0,0 };
		return Mesh(vertices, indices, material, bound, bound, bound);
		//	std::vector<float> xBound, std::vector<float> yBound, std::vector<float> zBound,
	}
	//static Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Material material,
	//	std::vector<float> xBound, std::vector<float> yBound, std::vector<float> zBound, bool withTexture = false)
private:
	unsigned int _handle;
	unsigned int color;
	unsigned int normal;
	unsigned int depth;
	unsigned int _post_processor;
	unsigned int edge;

};