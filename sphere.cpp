// OpenGL Involute gear simulation
// Stephen R Williams, Feb 2019
// License: GPL-3.0

#define GLEW_STATIC
#include <GL/glew.h>
#include <vector>
#include <array>
#include <stack>
#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#define is_sphere_cpp
#include "sphere.hpp"
#include "sphereObj.hpp"

// API interface for an openGL sphere builder
// Stephen R Williams, Jan 2019

static const GLuint maxn = 6;
static std::unique_ptr<sphereObj> mySphere;
static std::vector<GLfloat> vec2;

// build vertex and triangle vectors for order n sphere
void sphere::build(GLuint order)
{   
    if(order > maxn){
        std::ostringstream oss;
        oss << "Error: sphere::build(" << order << "), n must not be greater than " << maxn;
        std::string str =  oss.str();
        throw std::runtime_error(str);
    }
    if(mySphere){
        mySphere.reset();
    }
    mySphere = std::make_unique<sphereObj>(order);
    //mySphere -> setVects(mySphere);
}

const std::vector<GLfloat>& sphere::GetVerts()
{
    if(!mySphere) throw std::runtime_error("Error: sphere::GetVerts() called before sphere::build()");
    return mySphere -> GetVerts();
}

const std::vector<GLfloat>& sphere::GetVertsNorms()
{
    if(!mySphere) throw std::runtime_error("Error: sphere::GetVerts2() called before sphere::build()");
    GLuint len = mySphere -> GetVerts().size();
    vec2.reserve(2 * len);
    
    GLuint i = 0;
    std::array<GLfloat, 3> arry;
    auto vec = mySphere -> GetVerts();
    for(auto r: vec){
        arry.at(i++) = r; 
        if(i==3){
            i = 0;
            for(auto x: arry) vec2.push_back(x);
            for(auto x: arry) vec2.push_back(x);
        }   
    }
    if(i != 0) throw std::runtime_error("Error: sphere::GetVerts2(), vector length not a multiple of 3");
    return vec2;
}

const std::vector<GLuint>& sphere::GetInds()
{
    if(!mySphere) throw std::runtime_error("Error: sphere::GetInds() called before sphere::build()");
    return mySphere -> GetInds();
}

GLuint sphere::GetNInds()
{
    if(!mySphere) throw std::runtime_error("Error: sphere::GetNInds() called before sphere::build()");
    return mySphere -> GetNInds();
}


// release all memory buffers and loose all data
void sphere::release()
{
    if(mySphere) mySphere.reset();
}




