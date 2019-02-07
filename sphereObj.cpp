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
#include <cmath>
#include "sphereObj.hpp"

// two short functions for use by the constructor's initiallisation list
auto nv(GLuint n){ // calculate the final number of vertices
    GLuint nverts = 6, ntri_half = 4;
    
    for(GLuint i=0; i<n; ++i){
        nverts += 3 * ntri_half;
        ntri_half *= 4;
    }
    return nverts;
};

auto nt(GLuint n){ // calculate the final number of triangles
    GLuint ntri = 8;
    
    for(GLuint i=0; i<n; ++i) ntri *= 4;
    return ntri;
};
////////////////////////////////////////////////////////////////////////


// Now the constructor
sphereObj::sphereObj(GLuint n):order(n), nVerts(nv(n)), nTri(nt(n))
{
    std::cout << "sphere: nverts = " << nVerts << ", ntri = " << nTri << std::endl;
    verts.resize(3 * nVerts);
    trigs.resize(nTri); 
    triCnt = triCntOld = 8;
    vertCnt = vertCntOld = 6;
    //triangle::initStatics();
    octahedron();  
    // next divide each triangle into four new triangles, and repeat 'order' times
    for(int i=0; i<order; ++i) triDivide();
}


void sphereObj::triDivide()
{
    // step 1: new vertices and new indices, put old indices on stack, call neighbours
    {
        GLuint i, j, neighTri;
        GLuint iv, jv;
        const GLuint key1[] = {0, 1, 0}, key2[] = {2, 2, 1}, keyN[] = {1, 0, 2};
        GLfloat x, y, z, r;
        
        for(i=0; i<triCntOld; ++i){
            auto &tr = trigs.at(i);
            tr.push_stack();
            for(j=0; j<3; ++j){
                // is this neighbour's index less than us??
                neighTri = tr.getNeigh_tri(j);
                if(neighTri < i){ // new vertex already made
                    auto &tr2 = trigs.at(neighTri);
                    iv = tr2.getIndex(keyN[j]);
                    tr.putIndex(j, iv);
                }
                else{
                    // first add 3 vertices to static vertex vector
                    // and store their indices
                    iv = tr.top_stack(key1[j]); // old vertex indices
                    jv = tr.top_stack(key2[j]);  
                    tr.putIndex(j, vertCnt);  
                    newVertex(iv, jv); // increments vertCnt
                }
            }          
        }
    }
    // new vertices done
    
    // step 2: new triangles, each existing triangle goes to the centre
    {
        for(int i=0; i<triCntOld; ++i){
            // set new neighbouring triangles
            auto &tr = trigs.at(i); 
            tr.putNeigh_tri(0, triCnt++); 
            tr.putNeigh_tri(1, triCnt++); 
            tr.putNeigh_tri(2, triCnt++); 
            // set vertex indices on new neighbouring triangles 
            setIndex(tr); // one stack pop so index is now on top 
        }
        // now the new triagles must find their neighbours
        for(int i=0; i<triCntOld; ++i){
            newNeighbours(i, 0);
            newNeighbours(i, 1);
            newNeighbours(i, 2);
        }
        for(int i=0; i<triCntOld; ++i){
            auto &tr = trigs.at(i);
            tr.pop_stack(); 
        }
        vertCntOld = vertCnt;
        triCntOld = triCnt;
    }
}

// i is the index for which triangle neighbouring trigs[itr] we assign new neighbours to
void sphereObj::newNeighbours(GLuint itr, GLuint i)
{
    const GLuint key1[] = {0, 1, 0}, key2[] = {2, 2, 1}; // triangles neighbouring pivot
    GLuint j, k;
    GLuint pivot, subj;
    GLuint map[][3] = { {0, 2, 1},
                        {1, 2, 0},
                        {0, 1, 2} };
    
    auto &tr = trigs.at(itr);
    subj = tr.getNeigh_tri(i); // subject triangle index
    auto &tr_subj = trigs.at(subj); // subject triangle
    pivot = tr_subj.getIndex(i); // subject's signiture vertex, used to id it
    j = tr.top_stack(key1[i]); // old central neighbouring triangle
    k = getTrig(j, pivot);
    tr_subj.putNeigh_tri(map[i][0], k);
    j = tr.top_stack(key2[i]); // old central neighbour triangle
    k = getTrig(j, pivot);                
    tr_subj.putNeigh_tri(map[i][1], k);
    tr_subj.putNeigh_tri(map[i][2], itr);
}

// returns the new triangle which is a neighbour to pivot
// itr is old neighbouring triangle
GLuint sphereObj::getTrig(GLuint itr, GLuint pivot)
{
    GLuint i, j, k;

    for(i=0; i<3; ++i){
        j = trigs.at(itr).getNeigh_tri(i); // neighbouring triangle
        k = trigs.at(j).getIndex(i); // pivot of neighbouring triangle
        if(k == pivot) return j;
    }
    std::ostringstream oss;
    oss << "Error: triangle::getTrig(GLuint, GLuint), failed to find pivot " << pivot << '.';
    std::string str =  oss.str();
    throw std::runtime_error(str);
}

// sets the indices to the vertices for the new triangles
void sphereObj::setIndex(triangle &tr)
{    
    auto &t0 = trigs.at(tr.getNeigh_tri(0));
    auto &t1 = trigs.at(tr.getNeigh_tri(1));
    auto &t2 = trigs.at(tr.getNeigh_tri(2));
    t0.putIndex(0, tr.top_stack(0));
    t0.putIndex(1, tr.getIndex(2));
    t0.putIndex(2, tr.getIndex(0));
    
    t1.putIndex(0, tr.getIndex(2));
    t1.putIndex(1, tr.top_stack(1));
    t1.putIndex(2, tr.getIndex(1));

    t2.putIndex(0, tr.getIndex(0));
    t2.putIndex(1, tr.getIndex(1));
    t2.putIndex(2, tr.top_stack(2));
    tr.pop_stack();
}


// adds a new unit vertex which is half way
// between vertex j & k
void sphereObj::newVertex(GLuint j, GLuint k)
{
    GLfloat x, y, z, r;
    GLuint i = 3 * vertCnt++;
    
    j *= 3;
    k *= 3;
    auto vj = verts.at(j++);
    auto vk = verts.at(k++);
    x = vj + vk;
    vj = verts.at(j++);
    vk = verts.at(k++);
    y = vj + vk;
    vj = verts.at(j);
    vk = verts.at(k);
    z = vj + vk;    
    r = sqrt(x * x + y * y + z * z);
    verts.at(i++) = x / r;
    verts.at(i++) = y / r;
    verts.at(i) = z / r;
}

// return full buffer of indices
const std::vector<GLuint>& sphereObj::GetInds()
{  
    GLuint nInds = 3 * trigs.size();
    inds.resize(nInds);
    int i = 0, j = 0;
    for(auto tr: trigs){
        auto &index = tr.getIndex();
        inds.at(j++) = index.at(0);
        inds.at(j++) = index.at(1);
        inds.at(j++) = index.at(2);
    } 
    return inds;
}

void sphereObj::octahedron()
{
    verts.at(0) = 0.0;
    verts.at(1) = 0.0;
    verts.at(2) = 1.0;
    
    verts.at(3) = 0.0;
    verts.at(4) = 0.0;
    verts.at(5) = -1.0;

    verts.at(6) = 1.0;
    verts.at(7) = 0.0;
    verts.at(8) = 0.0;
    
    verts.at(9) = 0.0;
    verts.at(10) = 1.0;
    verts.at(11) = 0.0;
    
    verts.at(12) = -1.0;
    verts.at(13) = 0.0;
    verts.at(14) = 0.0;
    
    verts.at(15) = 0.0;
    verts.at(16) = -1.0;
    verts.at(17) = 0.0;
    for(int i=0,jt,kt,jv,kv; i<4; ++i){
        jt = (i + 3) % 4; 
        kt = (i + 1) % 4;
        jv = i + 2;
        kv = (i + 1) % 4 + 2;
        trigs.at(i).set(jt, kt, i+4, jv, kv, 0);
        trigs.at(i+4).set(jt+4, kt+4, i, jv, kv, 1);    
    }    
}




