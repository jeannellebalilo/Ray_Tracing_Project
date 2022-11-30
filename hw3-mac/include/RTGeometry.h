/**************************************************
Geometry class modified for Ray Tracing project.
*****************************************************/
#include <vector>
#include "Triangle.h"

#ifndef __RTGEOMETRY_H__
#define __RTGEOMETRY_H__

class RTGeometry {
public:
    int count;
    std::vector<Triangle> elements;

    virtual void init(){};
    virtual void init(const char* s){};
};

#endif 
