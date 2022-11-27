/**************************************************
Image is a class for a Image object.

Part of Final Project, Ray Tracing.
*****************************************************/
#include <vector>
#include <glm/glm.hpp>

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

    void initialize() {
        int n = width * height;
        glm::vec3 entry = glm::vec3(0.0f);
        pixels.resize(n, entry);
    }

    void init() {
        glGenFramebuffers(1,&fbo);
        glGenTextures(1,&tbo);
    }
private:
    unsigned int fbo;
    unsigned int tbo;
};

#endif 