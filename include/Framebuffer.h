#include <GL/glew.h>
#include <gl/GLU.h>
#include <GLFW/glfw3.h>
class Framebuffer {
public:
	Framebuffer(int window_width, int window_height, unsigned int edge[], unsigned int colorBuffers[], unsigned int attachments[])
	{
		glGenFramebuffers(1, &_handle);
	};




	/*
	int initializeFBOs(int window_width, int window_height, unsigned int fbo, unsigned int blur[], unsigned int colorBuffers[], unsigned int attachments[], unsigned int rbo, unsigned int blurText) {
		{
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);

			//Textures for fbo

			{
				glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				// attach texture to framebuffer
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffers[0], 0);

				glBindTexture(GL_TEXTURE_2D, colorBuffers[1]);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				// attach texture to framebuffer
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, colorBuffers[1], 0);


				glBindTexture(GL_TEXTURE_2D, 0);
			}

			glDrawBuffers(2, attachments);




			//more fbo initialization
			{
				glBindRenderbuffer(GL_RENDERBUFFER, rbo);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window_width, window_height);
				glBindRenderbuffer(GL_RENDERBUFFER, 0);

				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
					EXIT_WITH_ERROR("failed to create framebuffer");
				}
				glClearColor(0, 0, 0, 0);
				glEnable(GL_DEPTH_TEST);
				glEnable(GL_CULL_FACE);
				glBindTexture(GL_TEXTURE_2D, 0);
				glDrawBuffers(2, attachments);
			}
			//Initialize blurring fb-s


			glBindFramebuffer(GL_FRAMEBUFFER, blur[0]);

			//init
			{

				glBindTexture(GL_TEXTURE_2D, blurText);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				// attach texture to framebuffer
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurText, 0);
				glDrawBuffers(1, &attachments[0]);
				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
					EXIT_WITH_ERROR("failed to create framebuffer");
				}


				glBindFramebuffer(GL_FRAMEBUFFER, blur[1]);
				// attach texture to framebuffer
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffers[1], 0);
				glDrawBuffers(1, &attachments[0]);
				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
					EXIT_WITH_ERROR("failed to create framebuffer");
				}



				



				glBindFramebuffer(GL_FRAMEBUFFER, 0);


			}
		}

	}
	//*/
	void bindBuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, _handle);
	}
	static void unbind() {
		glBindFramebuffer(Framebuffer, 0)
	}
private:
	unsigned int _handle;

};