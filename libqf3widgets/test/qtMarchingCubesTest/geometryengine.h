#ifndef GEOMETRYENGINE_H
#define GEOMETRYENGINE_H

#include <QtOpenGL/QGLShaderProgram>

class GeometryEngine
{
public:
    GeometryEngine();
    virtual ~GeometryEngine();

    void init();

    void drawCubeGeometry();

private:
    void initCubeGeometry();

    GLuint *vboIds;

};

#endif // GEOMETRYENGINE_H
