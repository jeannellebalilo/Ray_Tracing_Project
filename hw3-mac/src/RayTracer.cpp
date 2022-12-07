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
#include "Material.h"
#include "Triangle.h"
#include "RayTracer.h"
#include <time.h>

using namespace RayTracer;

void RayTracer::Raytrace(Camera cam, RTScene &scene, Image &image){
    int w = image.width;
    int h = image.height;
    clock_t start = clock();
    for (int j=0; j<h; j++){
     for (int i=0; i<w; i++){
         Ray ray = RayThruPixel( cam, i, j, w, h );
         Intersection hit = Intersect( ray, scene );
         image.pixels[j*w + i] = FindColor( hit, scene, 4); 
        }
        std::cout<<"Traycing compleetion:" << float(float((j))/float((h))) * 100.0f<< "%"  << std::endl;
     }
    clock_t end = clock();
    double elapsed = double(end - start)/CLOCKS_PER_SEC;
    std::cout<< "Render Finished. time taken: "<< elapsed << " seconds" << std::endl;
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
    
    glm::vec4 p0 = glm::vec4(triangle->P[0], 1.0f);
    glm::vec4 p1 = glm::vec4(triangle->P[1], 1.0f);
    glm::vec4 p2 = glm::vec4(triangle->P[2], 1.0f);
    glm::vec3 n0 = glm::vec3(triangle->N[0]);
    glm::vec3 n1 = glm::vec3(triangle->N[1]);
    glm::vec3 n2 = glm::vec3(triangle->N[2]);
    glm::vec4 d = glm::vec4(-ray.dir, 0.0f);
    glm::mat4 mat = glm::mat4(p0,p1,p2,d);
    glm::vec4 p = glm::vec4(ray.p0, 1.0f);
    glm::vec4 lamba = glm::inverse(mat) * p;
    float t = lamba.w;
    glm::vec3 iPoint = lamba.x * p0 + lamba.y * p1 + lamba.z * p2;
    glm::vec3 n = glm::normalize(lamba.x * n0 + lamba.y * n1 + lamba.z * n2);
   
   
    if(lamba.x < 0 || lamba.y < 0 || lamba.z < 0 || t < 0){
         return Intersection();
    }
    
    //std::cout<< std::endl<<"Found not hit:"<< t << std::endl;   

    Intersection inter = Intersection(iPoint, n, ray.dir, triangle, t);
    //std::cout<< std::endl<<"Found hit:"<< t << std::endl;   
    return inter;
   
    
}

Intersection RayTracer::Intersect(Ray ray, RTScene &scene){
    float mindist = INFINITY;
    Intersection hit;
    hit.dist = INFINITY;
    for (int i=0; i<scene.triangle_soup.size(); i++){ // Find closest intersection; test all objects
        Intersection hit_temp = Intersect(ray, &scene.triangle_soup[i]);
        if (hit_temp.dist < mindist){ // closer than previous hit
            mindist = hit_temp.dist;
            hit = hit_temp;
        }
    }
    mindist = INFINITY;
    return hit;
}

// Shade the light color seen by the in-coming ray
glm::vec3 RayTracer::FindColor(Intersection hit, RTScene &scene, int recursion_depth){
    
    //Material *m = hit.triangle->material;
    if(hit.triangle==nullptr || hit.dist == INFINITY ){
        return glm::vec3(0.0f, 0.0f, 0.0f);
    }
    glm::vec3 fragColor = hit.triangle->material->emision;
    for(auto const& [key, light] : scene.light){
        if(recursion_depth>0){
            Ray reflectedRay = Ray();
            glm::vec3 dir = 2.0f * glm::dot(hit.N, hit.V) * hit.N - hit.V;
            reflectedRay.dir = dir;
            reflectedRay.p0 = hit.P;
            Intersection rHit = Intersect(reflectedRay, scene);
            if(rHit.triangle == nullptr || rHit.dist == INFINITY){
                  return FindColor(hit, scene, 0);
            }else{
                fragColor += glm::vec3(hit.triangle->material->specular) * FindColor(rHit, scene, recursion_depth - 1);
            }
        }
        Ray ray = Ray();
        ray.p0 = hit.P + 0.2f * hit.N;
        glm::vec3 dir = (glm::vec3(light->position) - ray.p0 );
        float distance = glm::length(glm::vec3(light->position)-ray.p0 );
        ray.dir = dir;
        Intersection intr = Intersect(ray, scene);
        if(intr.dist < distance){
            continue;
        }else{
            fragColor += glm::vec3(hit.triangle->material->ambient);
            fragColor += glm::vec3(hit.triangle->material->diffuse);
            // std::cout<< "fragColor after shadow: " << "r: "<< fragColor.x << " g: " << fragColor.y << " b: " << fragColor.z;
        }
        
    
    }
    
    return fragColor;

}