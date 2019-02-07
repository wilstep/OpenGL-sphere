// OpenGL Involute gear simulation
// Stephen R Williams, Feb 2019
// License: GPL-3.0

namespace oglWrap
{
    void info();
    void createBuff(std::vector<GLfloat> &vertices, std::vector<GLuint> &indices);
    void setUp();
    void close();
    void draw(GLuint n, GLuint offset);
    
    std::vector<GLfloat>& perspective(GLfloat theta, GLfloat ar, GLfloat zn, GLfloat zf);
    std::vector<GLfloat>& rotateZ(GLfloat theta);
    std::vector<GLfloat>& rotateY(GLfloat theta);
    
    void setFloat(const std::string &name, GLfloat value);
    void setVec3(const std::string &name, GLfloat data[]);
    void setMat4(const std::string &name, GLfloat data[]);
    void setColor(GLfloat x, GLfloat y, GLfloat z); 
}
