/*
For some reason the standard mesh treatment did not work, put a bunch of exceptions in there, something about swapbuffers and google helped nothing:
so taken from learnopengl.com, I'd have preferred the mesh solution because that makes a shitton more of sense but whatever,im not putting up with the texture nonsense when opengl is being dumb its being fucking dumb
*/

unsigned int VAO = 0;
unsigned int vbo;
void renderQuad()
{
    if (VAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &vbo);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

static void initialize(int window_width, int window_height, unsigned int& color, unsigned int& normal, unsigned int& depth, unsigned int& edge, unsigned int& handle, unsigned int& postprocessor, unsigned int& rbo, GLenum attachments[]) //fm(&fm),
{
    //this->fm = &fm;



    glGenTextures(1, &depth);





    glGenFramebuffers(1, &handle);
    glBindFramebuffer(GL_FRAMEBUFFER, handle);
    //setup color texture
    glGenTextures(1, &color);
    {
        glBindTexture(GL_TEXTURE_2D, color);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // attach texture to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);
    }
    //setup normal texture
    glGenTextures(1, &normal);
    {
        glBindTexture(GL_TEXTURE_2D, normal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8_SNORM, window_width, window_height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the edge filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // attach texture to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normal, 0);
    }
    //setup depth texture
    //*
    {
        glBindTexture(GL_TEXTURE_2D, depth);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8_SNORM,
            window_width, window_height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the edge filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, depth, 0);
        glBindTexture(GL_TEXTURE_2D, 0);

    }//*/
    //glBindTexture(GL_TEXTURE_2D, 0);
    //GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    //glDrawBuffers(2, attachments);
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window_width, window_height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);


    //finish fbo initialization



    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "fbo init failed for fbo1" << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
    }
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDrawBuffers(2, attachments);




    glGenFramebuffers(1, &postprocessor);
    glBindFramebuffer(GL_FRAMEBUFFER, postprocessor);
    glGenTextures(1, &edge);
    //init edge texture


    glBindTexture(GL_TEXTURE_2D, edge);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // attach texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, edge, 0);
    GLenum attachment[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachment);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "fbo init failed for fbo2" << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
    }




    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//*/

static void doImageProcessing(unsigned int& color, unsigned int& normal, unsigned int& depth, unsigned int& edge, unsigned int& handle, unsigned int& postprocessor, Shader& processor, Shader& combination)
{
    //*
    glBindFramebuffer(GL_FRAMEBUFFER, postprocessor);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    processor.use();
    glUniform1i(glGetUniformLocation(processor.ID, "diffuseTexture"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, normal);
    // set the appropriate texture sampler variable in the fragment shader
    glUniform1i(glGetUniformLocation(processor.ID, "depthText"), 1);
    glActiveTexture(GL_TEXTURE1);
    // set the appropriate texture sampler variable in the fragment shader
    glBindTexture(GL_TEXTURE_2D, depth);
    renderQuad();
    //*/
    //*
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    combination.use();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);


    glUniform1i(glGetUniformLocation(combination.ID, "diffuseTexture"), 0);
    glActiveTexture(GL_TEXTURE0);
    // set the appropriate texture sampler variable in the fragment shader
    glBindTexture(GL_TEXTURE_2D, color);
    glUniform1i(glGetUniformLocation(combination.ID, "edge"), 1);
    glActiveTexture(GL_TEXTURE1);
    // set the appropriate texture sampler variable in the fragment shader

    glBindTexture(GL_TEXTURE_2D, edge);
    renderQuad();
    glActiveTexture(GL_TEXTURE0);
    glActiveTexture(GL_TEXTURE1);
    //glUseProgram(0);
    //bindBuffer();
    //glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    //bindPostProcessor();
    //glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    //unbind();
    //glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    //*/

}
void clearAll(unsigned int& fbo, unsigned int& postprocessor)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, postprocessor);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}
static void deleteBuffers(unsigned int& color, unsigned int& normal, unsigned int& depth, unsigned int& edge, unsigned int& handle, unsigned int& postprocessor, unsigned int& rbo)
{
    glDeleteTextures(1, &color);
    glDeleteTextures(1, &normal);
    glDeleteTextures(1, &depth);
    glDeleteTextures(1, &edge);
    glDeleteFramebuffers(1, &handle);
    glDeleteFramebuffers(1, &postprocessor);
    glDeleteRenderbuffers(1, &rbo);

}


