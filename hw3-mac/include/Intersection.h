#include "Triangle.h"

class Intersection {
public:
    glm::vec3 P; // position of intersection
    glm::vec3 N; // surface normal
    glm::vec3 V; // direction to incoming ray
    Triangle* triangle; // pointer to geometric primitive (and material info)
    float dist; // distance to the source of ray

    Intersection(){}

    Intersection(glm::vec3 pos, glm::vec3 norm, glm::vec3 rayDir, Triangle* t, float d) {
        P = pos;
        N = norm;
        V = rayDir;
        triangle = t;
        dist = d;
    }
};