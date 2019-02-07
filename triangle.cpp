// OpenGL Involute gear simulation
// Stephen R Williams, Feb 2019
// License: GPL-3.0

#define GLEW_STATIC
#include <GL/glew.h>
#include <vector>
#include <array>
#include <stack>
#include <memory>
#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include "triangle.hpp"

// indices of neighbouring triangles, and vertices
void triangle::set(GLuint t0, GLuint t1, GLuint t2, GLuint v0, GLuint v1, GLuint v2)
{
    // 3 indices of neighbouring triangles
    neigh_tri.at(0) = t0; 
    neigh_tri.at(1) = t1; 
    neigh_tri.at(2) = t2; 
    // 3 indices to vetercies which form triangle
    index.at(0) = v0;
    index.at(1) = v1;
    index.at(2) = v2; 
}

void triangle::push_stack()
{
    lifo.push(neigh_tri);
    lifo.push(index); 
}


