/**************************************************
Image is a class for a Image object.

Part of Final Project, Ray Tracing.
*****************************************************/
#include <vector>
#include <glm/glm.hpp>
#include <OpenGL/gl3.h>

#ifndef __IMAGE_H__
#define __IMAGE_H__

class Image {
public:
    int width;
    int height;
    std::vector<glm::vec3> pixels;

    Image(int w, int h) {
        width = w;
        height = h;
        void initialize();
    }

    void init() {
        int n = width * height;
        glm::vec3 entry = glm::vec3(0.0f);
        pixels.resize(n, entry);
        
        glGenFramebuffers(1,&fbo);
        glGenTextures(1,&tbo);
    }

    void draw() {
        glBindTexture(GL_TEXTURE_2D, tbo);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, &pixels[0][0]);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_READ_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tbo, 0);// attach texture and the read frame
        // glBindFramebuffer(GL_WRITE_FRAMEBUFFER, 0); // if not already so
        glBlitFramebuffer(0,0,width,height,0,0,width,height,GL_COLOR_BUFFER_BIT,
                            GL_NEAREST); // copy framebuffer from read to write
    }
private:
    unsigned int fbo;
    unsigned int tbo;
};

#endif 