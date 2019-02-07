sphere: triangle.o main.o sphere.o sphereObj.o opengl.o
	g++ -g -o sphere sphere.o sphereObj.o main.o opengl.o triangle.o -lglfw -lGLEW -lGL 

sphereObj.o: sphereObj.cpp sphereObj.hpp triangle.hpp
	g++ -g -std=c++17 -c sphereObj.cpp

triangle.o: triangle.cpp triangle.hpp sphereObj.hpp
	g++ -g -std=c++17 -c triangle.cpp

sphere.o: sphere.cpp sphere.hpp sphereObj.hpp triangle.hpp
	g++ -g -std=c++17 -c sphere.cpp

opengl.o: opengl.cpp opengl.hpp
	g++ -g -std=c++17 -c opengl.cpp 

main.o: main.cpp sphere.hpp opengl.hpp
	g++ -g -std=c++17 -c main.cpp
