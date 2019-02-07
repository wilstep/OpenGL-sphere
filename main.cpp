// OpenGL Involute gear simulation
// Stephen R Williams, Feb 2019
// License: GPL-3.0

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <cmath>
#include <chrono>
#include <thread>

#include "sphere.hpp"
#include "opengl.hpp"

GLfloat const *gverts;
GLuint const *ginds;

GLfloat metric(GLuint i, GLuint j)
{
    GLfloat dx, dy, dz;
    
    i *= 3;
    j *= 3;
    dx = gverts[i++] - gverts[j++];
    dy = gverts[i++] - gverts[j++];
    dz = gverts[i] - gverts[j];
    return sqrt(dx * dx + dy * dy + dz * dz);
}

void print(int i)
{
    int i1, i2, i3;

    std::cout << i;
    i *= 3;    
    i1 = ginds[i++];
    i2 = ginds[i++];
    i3 = ginds[i];    
    std::cout << ": " << metric(i1, i2); 
    std::cout << ", " << metric(i1, i3); 
    std::cout << ", " << metric(i2, i3);
}

void print2(int i)
{
    int i1;
    GLfloat x, y, z, r;
   
    i1 = 3 * ginds[i];
    x = gverts[i1++];
    y = gverts[i1++];
    z = gverts[i1];  
    r = x * x + y * y + z * z;
    r = sqrt(r);
    std::cout << ginds[i++] << ": r = " << r;
    std::cout << ", x = " << x << ", y = " << y << ", z = " << z;
    std::cout << std::endl;

    i1 = 3 * ginds[i];
    x = gverts[i1++];
    y = gverts[i1++];
    z = gverts[i1];  
    r = x * x + y * y + z * z;
    r = sqrt(r);
    std::cout << ginds[i++] << ": r = " << r;
    std::cout << ", x = " << x << ", y = " << y << ", z = " << z;
    std::cout << std::endl;

    i1 = 3 * ginds[i];
    x = gverts[i1++];
    y = gverts[i1++];
    z = gverts[i1];  
    r = x * x + y * y + z * z;
    r = sqrt(r);
    std::cout << ginds[i] << ": r = " << r;
    std::cout << ", x = " << x << ", y = " << y << ", z = " << z;
    std::cout << std::endl;
}

void run2()
{
    GLuint N;
    
    sphere::build(3);
    auto verts = sphere::GetVerts();
    gverts = verts.data();
    std::cout << std::endl;
    auto inds = sphere::GetInds();
    ginds = inds.data();
    N = inds.size() / 3; // number of triangles
    for(int i=0; i<N; ++i) print2(i);
    std::cout << "finished\n"; 
    
    //std::ifstream fin;
    //fin.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    //fin.open("test.txt");
    //fin.close();
}


void run(unsigned int order)
{
    int maj, min, rev;
    glfwGetVersion(&maj, &min, &rev);
    std::cout << maj << '.' << min << '.' << rev << std::endl; 	


    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow* window = glfwCreateWindow(1200, 900, "OpenGL", nullptr, nullptr); // Windowed
    //GLFWwindow* window = glfwCreateWindow(1920, 1200, "OpenGL", glfwGetPrimaryMonitor(), nullptr); // Fullscreen
    glfwMakeContextCurrent(window);
    // initialise GLEW
    glewExperimental = GL_TRUE;
    glewInit();
    // basic window is now setup
    
    oglWrap::info();
    sphere::build(order);
    const GLuint NInds = sphere::GetNInds();
    auto verts = sphere::GetVertsNorms();
    auto inds = sphere::GetInds();
    oglWrap::createBuff(verts, inds);
    sphere::release();  
    
    oglWrap::setUp();
    // shader is now compiled
    
    // setup shader variables
    auto perspective = oglWrap::perspective(30.0f, 4.0f/3.0f, 0.1f, 180.0f);
    oglWrap::setMat4("perspective", perspective.data());
    auto rotateY = oglWrap::rotateY(0.0f);
    oglWrap::setMat4("rotate", rotateY.data());
   
    // enable depth testing
    glEnable(GL_DEPTH_TEST); 
    
    GLfloat omega = 0.0f;
    auto t_start = std::chrono::high_resolution_clock::now();
    while(!glfwWindowShouldClose(window)){
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE); // end loop if escape key is pressed
        // Clear the screen to black
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // rotate on z axis
        rotateY = oglWrap::rotateY(omega);
        omega += 0.5;
        oglWrap::setMat4("rotate", rotateY.data());        
        
        // set color and draw
        oglWrap::setColor(0.1f, 0.2f, 0.5f);
        oglWrap::draw(NInds, 0);
        
        // sleep until 50 milli seconds is reached
        auto t_now = std::chrono::high_resolution_clock::now();
        GLfloat time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
        // 20 frames per second, acceptable
        std::this_thread::sleep_for(std::chrono::milliseconds(50 - (int) (time * 1000)));
        t_start = std::chrono::high_resolution_clock::now();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }   
    glfwTerminate();
}


int main(int argc, char *argv[])
{
    if(argc != 2){
        std::cout << "usage: " << argv[0] << " order, where order is a positve integer\n";
        std::cout << "which represents how many times the triangles are divided into smaller ones\n";
        return 0;
    }
    try{ 
        run(atoi(argv[1]));
    }
    catch (std::ifstream::failure e) {
        std::cerr << "ifstream file error: " << e.what() << std::endl;
        return 1;
    }
    catch(const std::runtime_error &e){
        std::cerr << "Runtime error: "  << e.what() << std::endl;
        return 1;     
    }
    catch(const std::out_of_range &e){
        std::cerr << "Out of Range error: " << e.what() << '\n';
        return 1;
    }
    catch (const std::bad_alloc &e){
        std::cerr << "Allocation error: " << e.what() << std::endl;
        return 1;
    }
    catch(std::exception &e){
        std::cerr << "unspecified standard exception caught: " << e.what() << std::endl;
        return 1;
        //std::exit(EXIT_FAILURE); 
    } 
    return 0;
}

