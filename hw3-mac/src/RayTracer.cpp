/**************************************************
Obj is subclass class of Geometry
that loads an obj file.
 This obj file loader only supports vertex positions and normals:
 v   x y z
 vn nx ny nz
 f 123//456
 i.e. there is no texture.
*****************************************************/
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>
#else
#endif

#include "Ray.h"
#include "Triangle.h"
#include "RayTracer.h"

using namespace RayTracer;

void RayTracer::Raytrace(Camera cam, RTScene scene, Image &image){
    int w = image.width;
    int h = image.height;
    for (int j=0; j<h; j++){
     for (int i=0; i<w; i++){
         Ray ray = RayThruPixel( cam, i, j, w, h );
         Intersection hit = Intersect( ray, scene );
         image.pixels[i] = FindColor( hit ); // <- not sure how far recursion depth should be yet
        }
     }
}

Ray RayTracer::RayThruPixel(Camera cam, int i, int j, int width, int height){
    Ray ray;
    ray.p0 = cam.eye;
    float alpha = 2 * ((i + 0.5) / width) - 1;
    float beta = 1 - 2 * ((j + 0.5) / height);
    float a = width / height;
    glm::mat4 camMat = glm::inverse(cam.view);
    glm::vec3 u = glm::vec3(camMat[0][0], camMat[0][1], camMat[0][2]);
    glm::vec3 v = glm::vec3(camMat[1][0], camMat[1][1], camMat[1][2]);
    glm::vec3 w = glm::vec3(camMat[2][0], camMat[2][1], camMat[2][2]);
    ray.dir = normalize((alpha*a*tan(cam.fovy/2)*u) + (beta*tan(cam.fovy/2)*v) - w);
    return ray;
}

Intersection RayTracer::Intersect(Ray ray, Triangle* triangle){
    glm::mat4 triMat;
    triMat[0] = glm::vec4(triangle->P[0], 1.0f);
    triMat[1] = glm::vec4(triangle->P[1], 1.0f);
    triMat[2] = glm::vec4(triangle->P[2], 1.0f);
    triMat[3] = glm::vec4(-(ray.dir), 0.0f);

    glm::vec4 rayPos = glm::vec4(ray.p0, 1.0f);

    // vector that represents lambdas, and t
    glm::vec4 lamT = glm::inverse(triMat) * rayPos;

    glm::vec3 q = ray.p0 + lamT[3] * ray.dir;
    glm::vec3 n = glm::normalize((lamT[0] * triangle->N[0]) + (lamT[1] * triangle->N[1]) + (lamT[2] * triangle->N[2]));
    float d = sqrt(pow((q[0] - ray.p0[0]), 2) + pow((q[1] - ray.p0[1]), 2) + pow((q[2] - ray.p0[2]), 2));

    return Intersection(q, n, -(ray.dir), triangle, d);  
}

Intersection RayTracer::Intersect(Ray ray, RTScene scene){
    float mindist = INFINITY;
    Intersection hit;
    for (Triangle* t : scene.triangle_soup){ // Find closest intersection; test all objects
        Intersection hit_temp = Intersect(ray, t);
        if (hit_temp.dist < mindist){ // closer than previous hit
            mindist = hit_temp.dist;
            hit = hit_temp;
        }
    }
    return hit;
}

// Shade the light color seen by the in-coming ray
glm::vec3 RayTracer::FindColor(Intersection hit){
    /* 
        Intersection variables:
        P = position of intersection
        N = surface normal
        V = direction to incoming ray
        triangle
        d = distance to source of ray
    */


}