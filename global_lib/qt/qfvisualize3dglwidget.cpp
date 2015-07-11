/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/



#include "qfvisualize3dglwidget.h"
#include <float.h>
#include "statistics_tools.h"
#include "ticktock.h"
#include <QDebug>
#include <QMouseEvent>
#include <QMatrix4x4>
#include <QVector4D>
#include <QElapsedTimer>
#include <QGLFunctions>

#ifdef QFVisualize3DGLWidget_USES_QOPENGLWIDGET
#define GLViewport GLFuncs->glViewport
#define GLMatrixMode GLFuncs->glMatrixMode
#define GLLoadIdentity GLFuncs->glLoadIdentity

#define GLEnable GLFuncs->glEnable
#define GLShadeModel GLFuncs->glShadeModel
#define GLLightModeli GLFuncs->glLightModeli
#define GLDepthFunc GLFuncs->glDepthFunc
#define GLClearColor GLFuncs->glClearColor
#define GLLightModelfv GLFuncs->glLightModelfv
#define GLBlendEquation GLFuncs->glBlendEquation
#define GLBlendFunc GLFuncs->glBlendFunc
#define GLHint GLFuncs->glHint
#define GLMatrixMode GLFuncs->glMatrixMode
#define GLOrthof GLFuncs->glOrthof
#define GLOrtho GLFuncs->glOrtho
#define GLGetError GLFuncs->glGetError
#else
#define GLViewport glViewport
#define GLMatrixMode glMatrixMode
#define GLLoadIdentity glLoadIdentity
#define GLEnable glEnable
#define GLShadeModel glShadeModel
#define GLLightModeli glLightModeli
#define GLDepthFunc glDepthFunc
#define GLClearColor glClearColor
#define GLLightModelfv glLightModelfv
#define GLBlendEquation glBlendEquation
#define GLBlendFunc glBlendFunc
#define GLHint glHint
#define GLMatrixMode glMatrixMode
#define GLOrthof glOrthof
#define GLOrtho glOrtho
#define GLGetError glGetError
#endif

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

static void qNormalizeAngle(double &angle)
{
    while (angle < 0)
        angle += 360;
    while (angle > 360)
        angle -= 360;
}
static double qNormalizeAngleO(double anglei)
{
    double angle=anglei;
    while (angle < 0)
        angle += 360;
    while (angle > 360)
        angle -= 360;

    return angle;
}

static void qSetColor(GLfloat* colf, QColor col) {
    colf[0]=col.redF();
    colf[1]=col.greenF();
    colf[2]=col.blueF();
    colf[3]=col.alphaF();
}

/*static void qSetColor(GLint* colf, QColor col) {
    colf[0]=col.red();
    colf[1]=col.green();
    colf[2]=col.blue();
    colf[3]=col.alpha();
}*/

inline QRgb qGLRGBA(int r, int g, int b, int a)// set RGBA value
{
    return (((r & 0xff) ) | ((g & 0xff) << 8) | ((b & 0xff) << 16 ) | ((a & 0xff) << 24));
}

static inline QRgb qRgbaPreM(int r, int g, int b, int alpha=255) {
    return qRgba(r*alpha/255, g*alpha/255, b*alpha/255, alpha);
}
static inline QRgb qGLRGBAPreM(int r, int g, int b, int alpha=255) {
    return qGLRGBA(r*alpha/255, g*alpha/255, b*alpha/255, alpha);
}





void QFVisualize3DGLWidget::qGLCheckError(const QString& message) {
#ifdef QFVisualize3DGLWidget_USES_QOPENGLWIDGET
    OPENGLFUNCTIONSCLASS *GLFuncs = getOpenGLFunctions();
    if (!GLFuncs) return;
#endif
    GLenum e=GLGetError();
    switch(e) {
        case GL_INVALID_VALUE: qDebug()<<message<<"ERROR: GL_INVALID_VALUE"; break;
        case GL_INVALID_ENUM: qDebug()<<message<<"ERROR: GL_INVALID_ENUM"; break;
        case GL_INVALID_OPERATION: qDebug()<<message<<"ERROR: GL_INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW: qDebug()<<message<<"ERROR: GL_STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW: qDebug()<<message<<"ERROR: GL_STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY: qDebug()<<message<<"ERROR: GL_OUT_OF_MEMORY"; break;
        //case GL_NO_ERROR: qDebug()<<message<<"NO ERROR"; break;
        //case : qDebug()<<message<<"ERROR: "; break;
        default: break;
    }

}

QFVisualize3DGLWidget::GLCamera::GLCamera(QFVisualize3DGLWidget *sceneWidget)
{

    this->sceneWidget=sceneWidget;
}

void QFVisualize3DGLWidget::GLCamera::draw()
{
#ifdef QFVisualize3DGLWidget_USES_QOPENGLWIDGET
    OPENGLFUNCTIONSCLASS *GLFuncs = sceneWidget->getOpenGLFunctions();
    if (!GLFuncs) return;
#endif
    GLdouble basesize=4.0;
    GLdouble    aspect, left, right, bottom, top;
    GLdouble nearp=1;
    GLdouble farp=100;


    /* compute aspect ratio */
    aspect = (GLdouble) sceneWidget->width() / (GLdouble) sceneWidget->height();

    /* make sure the window goes from [-basesize, basesize] in the
       smallest dimension */
    if ( aspect < 1.0 ) {
         left = -basesize;
         right = basesize;
         bottom = -basesize * ( 1.0 / aspect );
         top = basesize * ( 1.0 / aspect );
    } else {
         left = -basesize * aspect;
         right = basesize * aspect;
         bottom = -basesize;
         top = basesize;
    }

    GLMatrixMode( GL_PROJECTION );
    /* Reset world coordinates first ... */
    GLLoadIdentity();

#ifdef QT_OPENGL_ES_1
    GLOrthof( left, right, bottom, top, nearp, farp );
#else
    GLOrtho( left, right, bottom, top, nearp, farp );
#endif
}


QFVisualize3DGLWidget::GLLight::GLLight(GLenum light, QFVisualize3DGLWidget *sceneWidget)
{
    Q_UNUSED(sceneWidget);
    diffuse=QColor(150,150,150,255);
    specular=diffuse;
    this->light=light;

}

void QFVisualize3DGLWidget::GLLight::draw()
{
    GLfloat temp[] = { 0, 0, 0, 1 };
    qSetColor(temp, diffuse);
    glLightfv( light, GL_DIFFUSE, temp );
    qSetColor(temp, specular);
    glLightfv( light, GL_SPECULAR, temp );
    glEnable( light );
    glEnable(GL_LIGHT0);
}

void QFVisualize3DGLWidget::GLLight::setAmbient(QColor col)
{
    ambient=col;
}

void QFVisualize3DGLWidget::GLLight::setDiffuse(QColor col)
{
    diffuse=col;
}

void QFVisualize3DGLWidget::GLLight::setSpecular(QColor col)
{
    specular=col;
}

void QFVisualize3DGLWidget::GLLight::setPos(QVector3D v)
{
    pos=v;
}

void QFVisualize3DGLWidget::GLLight::setDir(QVector3D v)
{
    dir=v;
}

QFVisualize3DGLWidget::GLScene::GLScene(QFVisualize3DGLWidget *sceneWidget)
{
    this->sceneWidget=sceneWidget;
}

QFVisualize3DGLWidget::GLScene::~GLScene()
{
    clearSceneElements();
    clearTextures();
    clearShaders();
}

void QFVisualize3DGLWidget::GLScene::addSceneElement(QFVisualize3DGLWidget::GLSceneElement *element)
{
    element->setScene(this);
    scene.append(element);
}

void QFVisualize3DGLWidget::GLScene::clearSceneElements()
{
    qDeleteAll(scene);
    scene.clear();
}

void QFVisualize3DGLWidget::GLScene::clearShaders()
{
    //qDebug()<<"clearShaders "<<shaders.size();
    for (int i=0; i<shaders.size(); i++) {
        //qDebug()<<"clearShaders"<<i<<shaders[i];
        sceneWidget->glDeleteProgram(shaders[i]);
    }
    shaders.clear();
}


int QFVisualize3DGLWidget::GLScene::addShaderFromFile(const QString &filenameVertex, const QString &filenameFragment)
{
    //qDebug()<<"addShaderFromFile("<<filenameVertex<<filenameFragment<<")";

    //return -1;

    // Create the shaders
    GLuint VertexShaderID = sceneWidget->glCreateShader(GL_VERTEX_SHADER);
    if (glGetError()!=GL_NO_ERROR) {
        VertexShaderID = sceneWidget->glCreateShader(GL_VERTEX_SHADER);
    }
    //qGLCheckError("glCreateShader");

    GLuint FragmentShaderID = sceneWidget->glCreateShader(GL_FRAGMENT_SHADER);
    if (glGetError()!=GL_NO_ERROR) {
        FragmentShaderID = sceneWidget->glCreateShader(GL_FRAGMENT_SHADER);
    }
    //qGLCheckError("glCreateShader");

    //QGLFunctions funcs(sceneWidget->context());
    // Read the Vertex Shader code from the file
    QFile fVertex(filenameVertex);
    QString VertexShaderCode;
    if (fVertex.open(QFile::ReadOnly|QFile::Text)) {
        VertexShaderCode=fVertex.readAll();
        fVertex.close();
    }


    // Read the Fragment Shader code from the file
    QFile fFragment(filenameFragment);
    QString FragmentShaderCode;
    if (fFragment.open(QFile::ReadOnly|QFile::Text)) {
        FragmentShaderCode=fFragment.readAll();
        fFragment.close();
    }


    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    // qDebug()<<"Compiling shader: "<<filenameVertex;
    QByteArray a=VertexShaderCode.toLatin1();
    //qDebug()<<"shader code: \n"<<a;
    const char * aP=a.constData();
    sceneWidget->glShaderSource(VertexShaderID, 1, &aP , NULL);
    //qGLCheckError("glShaderSource");
    sceneWidget->glCompileShader(VertexShaderID);
    //qGLCheckError("glCompileShader");

    // Check Vertex Shader
    sceneWidget->glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    //qGLCheckError("glGetShaderiv");
    //qDebug()<<"  status: "<<Result;
    sceneWidget->glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    //qGLCheckError("glGetShaderiv");
    //qDebug()<<"  messageLength: "<<InfoLogLength;
    QByteArray VertexShaderErrorMessage(InfoLogLength, '\0');
    sceneWidget->glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, VertexShaderErrorMessage.data());
    //qGLCheckError("glGetShaderInfoLog");
    //qDebug()<<"  message:\n"<<QString(VertexShaderErrorMessage);

    // Compile Fragment Shader
    //qDebug()<<"Compiling shader: "<<filenameFragment;
    QByteArray b=FragmentShaderCode.toLatin1();
    //qDebug()<<"shader code: \n"<<b;
    const char * bP=b.constData();
    sceneWidget->glShaderSource(FragmentShaderID, 1, &bP , NULL);
    //qGLCheckError("glShaderSource");
    sceneWidget->glCompileShader(FragmentShaderID);
    //qGLCheckError("glCompileShader");


    // Check Fragment Shader
    sceneWidget->glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    sceneWidget->glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    QByteArray FragmentShaderErrorMessage(InfoLogLength, '\0');
    sceneWidget->glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, FragmentShaderErrorMessage.data());
    //qDebug()<<"  message:\n  "<<QString(FragmentShaderErrorMessage);

    // Link the program
    //qDebug()<<"Linking program";
    GLuint ProgramID = sceneWidget->glCreateProgram();
    sceneWidget->glAttachShader(ProgramID, VertexShaderID);
    sceneWidget->glAttachShader(ProgramID, FragmentShaderID);
    sceneWidget->glLinkProgram(ProgramID);

    // Check the program
    sceneWidget->glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    sceneWidget->glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    QByteArray ProgramErrorMessage( qMax(InfoLogLength, int(1)), '\0' );
    sceneWidget->glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, ProgramErrorMessage.data());
    //qDebug()<<"  message:\n  "<<QString(ProgramErrorMessage);

    sceneWidget->glDeleteShader(VertexShaderID);
    sceneWidget->glDeleteShader(FragmentShaderID);

    shaders.append( ProgramID );

    return shaders.size()-1;




/*

    QGLShader* shaderV=new QGLShader(QGLShader::Vertex, sceneWidget->context());
    shaderV->compileSourceFile(filenameVertex);
    QGLShader* shaderF=new QGLShader(QGLShader::Fragment);
    shaderF->compileSourceFile(filenameFragment);
    QGLShaderProgram* program = new QGLShaderProgram(sceneWidget->context(), NULL);
    program->addShader(shaderV);
    program->addShader(shaderF);
    program->link();
    shaders.append(shader);
    shader->setParent(NULL);
    return shaders.size()-1;*/

}


bool GLsceneFinalizeLessThan(const QFVisualize3DGLWidget::GLSceneElement* l1, const QFVisualize3DGLWidget::GLSceneElement* l2) {
    return l1->getOrderNo()<l2->getOrderNo();
}

void QFVisualize3DGLWidget::GLScene::finalizeScene()
{
    qStableSort(scene.begin(), scene.end(), GLsceneFinalizeLessThan);
    for (int i=0; i<scene.size(); i++) {
        scene[i]->finish();
    }
}

void QFVisualize3DGLWidget::GLScene::draw()
{
    //qDebug()<<"GLScene::draw";
    for (int i=0; i<scene.size(); i++) {
        scene[i]->draw();
    }
}

void QFVisualize3DGLWidget::GLScene::clearTextures()
{
    //qDebug()<<"clear textures";
    QVector<GLuint> tex;
    for (int i=0; i<textures.size(); i++) {
        tex<<textures[i].id;

    }
    textures.clear();
    if (tex.size()>0) glDeleteTextures(tex.size(), tex.data());
}

int QFVisualize3DGLWidget::GLScene::addTexture(const QImage &image, GLenum mode, GLenum activeTex)
{
    uint8_t* data=(uint8_t*)malloc(image.width()*image.height()*4*sizeof(uint8_t));
    int idx=0;
    for (int y=0; y<image.height(); y++) {
        QRgb* line=(QRgb*)image.scanLine(y);
        for (int x=0; x<image.width(); x++) {
            data[idx]=qRed(line[x]);
            idx++;
            data[idx]=qGreen(line[x]);
            idx++;
            data[idx]=qBlue(line[x]);
            idx++;
            data[idx]=qAlpha(line[x]);
            idx++;
        }

    }


    int texName=addTexture(data, image.width(), image.height(), mode, activeTex, GL_RGBA, GL_UNSIGNED_BYTE);

    free(data);

    return texName;
}

int QFVisualize3DGLWidget::GLScene::addTexture(uint8_t *image, int width, int height, GLenum mode, GLenum activeTex, GLenum format, GLenum type)
{
    sceneWidget->makeCurrent();
    //qDebug()<<"GLScene::addTexture("<<image<<width<<height<<(void*)mode<<(void*)activeTex<<(void*)format<<(void*)type;
    if (!QGLContext::currentContext() || !QGLContext::currentContext()->isValid()) return -1;
    GLuint texName;

    //QGLFunctions glFuncs(QGLContext::currentContext());

    glEnable(mode);
    if (glGetError()!=GL_NO_ERROR) glEnable(mode);
    if (glGetError()!=GL_NO_ERROR) return -1;
    //qGLCheckError("glEnable");

    sceneWidget->glActiveTexture( activeTex );
    if (glGetError()!=GL_NO_ERROR) sceneWidget->glActiveTexture( activeTex );



    // texture is byte-aligned (1)
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    //qGLCheckError("glPixelStorei");

    // reserve texture name
    glGenTextures(1, &texName);
    //qGLCheckError("glGenTextures");
    // make it a 2D texture oor anything else specified by mode
    glBindTexture(mode, texName);
    //qGLCheckError("glBindTexture");

    static GLfloat temp[]={0,0,0,0};
    glTexParameterfv(mode, GL_TEXTURE_BORDER_COLOR, temp);
    //qGLCheckError("glTexParameterfv");

    if (sceneWidget->interpolationMode()==LinearInterpolation) {
        glTexParameteri(mode, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(mode, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    } else /*if (sceneWidget->interpolationMode()==NoInterpolation)*/ {
        glTexParameteri(mode, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(mode, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }

    glTexParameteri(mode, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    //qGLCheckError("glTexParameteri");
    glTexParameteri(mode, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    //qGLCheckError("glTexParameteri");

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, type, image);
    //qGLCheckError("glTexImage2D");

    glDisable(mode);
    //qGLCheckError("glDisable");

    texData d;
    d.id=texName;
    d.activeTex=activeTex;
    d.width=width;
    d.height=height;
    d.mode=mode;
    textures.append(d);
    return textures.size()-1;
}


QFVisualize3DGLWidget::GLSceneElement::GLSceneElement(GLScene *sceneWidget)
{
    this->scene=sceneWidget;
    this->color=QColor(Qt::transparent);
    texture=-1;
    orderNo=0;
    shader=-1;
}

QFVisualize3DGLWidget::GLSceneElement::~GLSceneElement()
{

}

void QFVisualize3DGLWidget::GLSceneElement::setColor(QColor c)
{
    color=c;
}

void QFVisualize3DGLWidget::GLSceneElement::setTexture(int t)
{
    texture=t;
}

void QFVisualize3DGLWidget::GLSceneElement::setShader(int s)
{
    shader=s;
}

int QFVisualize3DGLWidget::GLSceneElement::addSetTexture(const QImage &image, GLenum mode, GLenum activeTex)
{
    texture=scene->addTexture(image, mode, activeTex);
    return texture;
}

int QFVisualize3DGLWidget::GLSceneElement::addSetTexture(uint8_t *image, int width, int height, GLenum mode, GLenum activeTex, GLenum format, GLenum type)
{
    texture=scene->addTexture(image, width, height, mode, activeTex, format, type);
    return texture;
}

void QFVisualize3DGLWidget::GLSceneElement::addMoreTexture(uint8_t *image, int width, int height, GLenum mode, GLenum activeTex, GLenum format, GLenum type)
{
    //return;
    moreTextures.append(scene->addTexture(image, width, height, mode, activeTex, format, type));
}

void QFVisualize3DGLWidget::GLSceneElement::setScene(QFVisualize3DGLWidget::GLScene *s)
{
    scene=s;
}

void QFVisualize3DGLWidget::GLSceneElement::setOrderNo(double i)
{
    orderNo=i;
}

double QFVisualize3DGLWidget::GLSceneElement::getOrderNo() const
{
    return orderNo;
}

void QFVisualize3DGLWidget::GLSceneElement::glColor()
{
    glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF());
}

void QFVisualize3DGLWidget::GLSceneElement::glColor(QColor color)
{
    glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF());
}


QFVisualize3DGLWidget::GLLines::GLLines(QFVisualize3DGLWidget::GLScene *sceneWidget, GLenum mode):
    GLSceneElement(sceneWidget)
{
    this->lineWidth=1;
    this->mode=mode;
}



void QFVisualize3DGLWidget::GLLines::appendLine(const QVector3D &c1, const QVector3D &c2)
{
    vertexCoord.append(c1);
    vertexCoord.append(c2);
}

void QFVisualize3DGLWidget::GLLines::appendLine(double x1, double y1, double z1, double x2, double y2, double z2)
{
    appendLine(QVector3D(x1,y1,z1), QVector3D(x2,y2,z2));
}

void QFVisualize3DGLWidget::GLLines::appendLine(const QVector3D &c1, const QVector3D &c2, QColor color)
{
    morecolors[vertexCoord.size()]=color;
    vertexCoord.append(c1);
    vertexCoord.append(c2);
}

void QFVisualize3DGLWidget::GLLines::appendLine(double x1, double y1, double z1, double x2, double y2, double z2, QColor color)
{
    appendLine(QVector3D(x1,y1,z1), QVector3D(x2,y2,z2), color);
}


void QFVisualize3DGLWidget::GLLines::draw()
{
    glBegin(mode);
    glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    for (int i=0; i<vertexCoord.size(); i=i+2) {
        glLineWidth(lineWidth);
        glColor(morecolors.value(i, color));
        glVertex3d(vertexCoord[i].x(), vertexCoord[i].y(), vertexCoord[i].z());
        //glColor(lineColors.value(i, color));
        glVertex3d(vertexCoord[i+1].x(), vertexCoord[i+1].y(), vertexCoord[i+1].z());
    }
    glEnd();
}

void QFVisualize3DGLWidget::GLLines::finish()
{

}

void QFVisualize3DGLWidget::GLLines::setLineWidth(float lw)
{
    lineWidth=lw;
}

QFVisualize3DGLWidget::GLQuad::GLQuad(GLScene *sceneWidget, GLenum mode):
    GLSceneElement(sceneWidget)
{
    this->mode=mode;
}

void QFVisualize3DGLWidget::GLQuad::appendTexCoord(const QVector2D &c)
{
    texCoord.append(c);
}

void QFVisualize3DGLWidget::GLQuad::appendVertex(const QVector3D &c)
{
    appendVertex(c, QVector3D(0,0,0));
}

void QFVisualize3DGLWidget::GLQuad::appendVertex(float x, float y, float z)
{
    appendVertex(QVector3D(x,y,z));
}

void QFVisualize3DGLWidget::GLQuad::appendVertex(const QVector3D &c, const QVector3D &n)
{
    vertexCoord.append(c);
    while (normals.size()<vertexCoord.size()) {
        normals.append(QVector3D(0,0,0));
    }
    while (normals.size()>vertexCoord.size()) {
        normals.pop_back();
    }
    normals[vertexCoord.size()-1]=n;
}

void QFVisualize3DGLWidget::GLQuad::appendVertex(float x, float y, float z, float nx, float ny, float nz)
{
    appendVertex(QVector3D(x,y,z), QVector3D(nx,ny,nz).normalized());
}

void QFVisualize3DGLWidget::GLQuad::appendCoord(const QVector3D &v, const QVector2D &tex)
{
    appendVertex(v);
    appendTexCoord(tex);
}

void QFVisualize3DGLWidget::GLQuad::appendCoord(const QVector3D &v, const QVector3D &n, const QVector2D &tex)
{
    appendVertex(v, n);
    appendTexCoord(tex);
}

void QFVisualize3DGLWidget::GLQuad::appendVertex(const QVector3D &c, QColor col)
{
    morecolors[vertexCoord.size()]=col;
    appendVertex(c);
}

void QFVisualize3DGLWidget::GLQuad::appendVertex(const QVector3D &c, const QVector3D &n, QColor col)
{
    morecolors[vertexCoord.size()]=col;
    appendVertex(c, n);
}

void QFVisualize3DGLWidget::GLQuad::appendVertex(float x, float y, float z, QColor col)
{
    appendVertex(QVector3D(x,y,z), col);
}

void QFVisualize3DGLWidget::GLQuad::appendVertex(float x, float y, float z, float nx, float ny, float nz, QColor col)
{
    appendVertex(QVector3D(x,y,z), QVector3D(nx,ny,nz).normalized(), col);
}

void QFVisualize3DGLWidget::GLQuad::draw()
{
    //QGLShaderProgram* shad=scene->getShader(shader);

    //QGLFunctions glFuncs(QGLContext::currentContext());
    if (moreTextures.size()>0) {
        for (int i=0; i<moreTextures.size(); i++) {

            if (moreTextures[i]>=0) {
                scene->getSceneWidget()->glActiveTexture( scene->getActiveTexture(moreTextures[i]) );
                if (glGetError()!=GL_NO_ERROR) scene->getSceneWidget()->glActiveTexture( scene->getActiveTexture(moreTextures[i]) );
                scene->getSceneWidget()->qGLCheckError("glActiveTexture "+QString::number(scene->getActiveTexture(moreTextures[i])));

                glEnable(scene->getTextureMode(moreTextures[i]));
                scene->getSceneWidget()->qGLCheckError("glEnable "+QString::number(scene->getTextureMode(moreTextures[i])));

                glBindTexture(GL_TEXTURE_2D, scene->getTexture(texture));
                scene->getSceneWidget()->qGLCheckError("glBindTexture");
            }
        }
    }
    if (texture>=0) {
        scene->getSceneWidget()->glActiveTexture( scene->getActiveTexture(texture) );
        if (glGetError()!=GL_NO_ERROR) scene->getSceneWidget()->glActiveTexture( scene->getActiveTexture(texture) );
        scene->getSceneWidget()->qGLCheckError("glActiveTexture "+QString::number(scene->getActiveTexture(texture)));

        glEnable(scene->getTextureMode(texture));
        scene->getSceneWidget()->qGLCheckError("glEnable "+QString::number(scene->getTextureMode(texture)));

        glBindTexture(GL_TEXTURE_2D, scene->getTexture(texture));
        scene->getSceneWidget()->qGLCheckError("glBindTexture");

    }
    if (shader>=0) {
        scene->getSceneWidget()->glUseProgram(scene->getShader(shader));
        scene->getSceneWidget()->qGLCheckError("glUseProgram");

        int texIdx=0;
        QString varname=QString("texture"+QString::number(texIdx));
        scene->getSceneWidget()->setUniformVarI(shader, varname, texIdx);
        scene->getSceneWidget()->setUniformVarI(shader, varname+"_width", scene->getTextureWidth(texture));
        scene->getSceneWidget()->setUniformVarI(shader, varname+"_height", scene->getTextureHeight(texture));
        texIdx=1;
        varname=QString("texture"+QString::number(texIdx));
        scene->getSceneWidget()->setUniformVarI(shader, varname, texIdx);
        scene->getSceneWidget()->setUniformVarI(shader, varname+"_width", scene->getTextureWidth(texture));
        scene->getSceneWidget()->setUniformVarI(shader, varname+"_height", scene->getTextureHeight(texture));


        //qDebug()<<scene->getActiveTexture(texture)<<scene->getActiveTexture(texture)-GL_TEXTURE0;
    }

    glBegin(mode);

        glColor(QColor(Qt::white));
        for (int i=0; i<vertexCoord.size(); i=i+1) {
            if (texture<0) {
                glColor(morecolors.value(i, color));
            }
            if (texture>=0 && i<texCoord.size()) {
                glTexCoord2d(texCoord[i].x(), texCoord[i].y());
            }
            glVertex3d(vertexCoord[i].x(), vertexCoord[i].y(), vertexCoord[i].z());
            if (i<normals.size()) {
                normals[i].normalize();
                glNormal3d(normals[i].x(), normals[i].y(), normals[i].z());
            }
        }
    glEnd();
    if (shader>=0) {
        scene->getSceneWidget()->glUseProgram(0);
        //qGLCheckError("glUseProgram");
    }
    if (moreTextures.size()>0) {
        for (int i=0; i<moreTextures.size(); i++) {
            scene->getSceneWidget()->glActiveTexture( scene->getActiveTexture(moreTextures[i]) );
            scene->getSceneWidget()->qGLCheckError("glActiveTexture "+QString::number(scene->getActiveTexture(moreTextures[i])));
            glDisable(scene->getTextureMode(moreTextures[i]));
            scene->getSceneWidget()->qGLCheckError("glDisable "+QString::number(scene->getTextureMode(moreTextures[i])));
        }
    }
    if (texture>=0) {
        scene->getSceneWidget()->glActiveTexture( scene->getActiveTexture(texture) );
        scene->getSceneWidget()->qGLCheckError("glActiveTexture "+QString::number(scene->getActiveTexture(texture)));
        glDisable(scene->getTextureMode(texture));
        scene->getSceneWidget()->qGLCheckError("glDisable "+QString::number(scene->getTextureMode(texture)));
    }
}

void QFVisualize3DGLWidget::GLQuad::setCube(QVector3D center, float w, float h, float d)
{

    appendVertex(center.x()-w/2.0, center.y()+h/2.0, center.z()-d/2.0, 0,0,-1, QColor("blue"));
    appendVertex(center.x()+w/2.0, center.y()+h/2.0, center.z()-d/2.0, 0,0,-1, QColor("blue"));
    appendVertex(center.x()+w/2.0, center.y()-h/2.0, center.z()-d/2.0, 0,0,-1, QColor("blue"));
    appendVertex(center.x()-w/2.0, center.y()-h/2.0, center.z()-d/2.0, 0,0,-1, QColor("blue"));

    appendVertex(center.x()-w/2.0, center.y()-h/2.0, center.z()+d/2.0, -1,0,0, QColor("red"));
    appendVertex(center.x()-w/2.0, center.y()+h/2.0, center.z()+d/2.0, -1,0,0, QColor("red"));
    appendVertex(center.x()-w/2.0, center.y()+h/2.0, center.z()-d/2.0, -1,0,0, QColor("red"));
    appendVertex(center.x()-w/2.0, center.y()-h/2.0, center.z()-d/2.0, -1,0,0, QColor("red"));

    appendVertex(center.x()+w/2.0, center.y()-h/2.0, center.z()-d/2.0, 1,0,0, QColor("darkred"));
    appendVertex(center.x()+w/2.0, center.y()+h/2.0, center.z()-d/2.0, 1,0,0, QColor("darkred"));
    appendVertex(center.x()+w/2.0, center.y()+h/2.0, center.z()+d/2.0, 1,0,0, QColor("darkred"));
    appendVertex(center.x()+w/2.0, center.y()-h/2.0, center.z()+d/2.0, 1,0,0, QColor("darkred"));

    appendVertex(center.x()-w/2.0, center.y()-h/2.0, center.z()-d/2.0, 0,-1,0, QColor("green"));
    appendVertex(center.x()+w/2.0, center.y()-h/2.0, center.z()-d/2.0, 0,-1,0, QColor("green"));
    appendVertex(center.x()+w/2.0, center.y()-h/2.0, center.z()+d/2.0, 0,-1,0, QColor("green"));
    appendVertex(center.x()-w/2.0, center.y()-h/2.0, center.z()+d/2.0, 0,-1,0, QColor("green"));

    appendVertex(center.x()-w/2.0, center.y()+h/2.0, center.z()-d/2.0, 0,1,0, QColor("darkgreen"));
    appendVertex(center.x()-w/2.0, center.y()+h/2.0, center.z()+d/2.0, 0,1,0, QColor("darkgreen"));
    appendVertex(center.x()+w/2.0, center.y()+h/2.0, center.z()+d/2.0, 0,1,0, QColor("darkgreen"));
    appendVertex(center.x()+w/2.0, center.y()+h/2.0, center.z()-d/2.0, 0,1,0, QColor("darkgreen"));

    appendVertex(center.x()-w/2.0, center.y()-h/2.0, center.z()+d/2.0, 0,0,1, QColor("darkblue"));
    appendVertex(center.x()+w/2.0, center.y()-h/2.0, center.z()+d/2.0, 0,0,1, QColor("darkblue"));
    appendVertex(center.x()+w/2.0, center.y()+h/2.0, center.z()+d/2.0, 0,0,1, QColor("darkblue"));
    appendVertex(center.x()-w/2.0, center.y()+h/2.0, center.z()+d/2.0, 0,0,1, QColor("darkblue"));

}

void QFVisualize3DGLWidget::GLQuad::finish()
{
    if (mode==GL_QUADS)  {
        for (int i=0; i<vertexCoord.size(); i+=4) {
            if (i+3<vertexCoord.size()) {
                QVector3D v1, v2;
                if (normals[i]==QVector3D(0,0,0)) {
                    v1=vertexCoord[i+1]-vertexCoord[i];
                    v2=vertexCoord[i+3]-vertexCoord[i];
                    normals[i]=QVector3D::crossProduct(v1, v2).normalized();
                }
                if (normals[i+1]==QVector3D(0,0,0)) {
                    v1=vertexCoord[i+2]-vertexCoord[i+1];
                    v2=vertexCoord[i]-vertexCoord[i+1];
                    normals[i+1]=QVector3D::crossProduct(v1, v2).normalized();
                }
                if (normals[i+2]==QVector3D(0,0,0)) {
                    v1=vertexCoord[i+3]-vertexCoord[i+2];
                    v2=vertexCoord[i+1]-vertexCoord[i+2];
                    normals[i+2]=QVector3D::crossProduct(v1, v2).normalized();
                }
                if (normals[i+3]==QVector3D(0,0,0)) {
                    v1=vertexCoord[i]-vertexCoord[i+3];
                    v2=vertexCoord[i+2]-vertexCoord[i+3];
                    normals[i+3]=QVector3D::crossProduct(v1, v2).normalized();
                }
            }
        }
    }



}


#ifdef QFVisualize3DGLWidget_USES_QOPENGLWIDGET

OPENGLFUNCTIONSCLASS *QFVisualize3DGLWidget::getOpenGLFunctions()
{
    QOpenGLContext* context=QOpenGLContext::currentContext();
    if (!context) return NULL;

    OPENGLFUNCTIONSCLASS *GLFuncs = context->versionFunctions<OPENGLFUNCTIONSCLASS>();
    if (!GLFuncs) {
        qDebug()<<"could not get OpenGL "OPENGLFUNCTIONSVERSIONSTR" compatibility functions!";
        return NULL;
    } else {
        //qDebug()<<"got OpenGL "OPENGLFUNCTIONSVERSIONSTR" compatibility functions!";
    }
    if (!initedOpenGLFunctions32) {
        makeCurrent();
        if (!GLFuncs->initializeOpenGLFunctions()) {
            qDebug()<<"could not initialize OpenGL "OPENGLFUNCTIONSVERSIONSTR" compatibility functions!";
            return NULL;
        } else {
            qDebug()<<"initialized OpenGL "OPENGLFUNCTIONSVERSIONSTR" compatibility functions!";
            qDebug()<<"OpenGL context isOpenGLES: "<<context->isOpenGLES();
            qDebug()<<"OpenGL context isValid: "<<context->isValid();
            qDebug()<<"OpenGL context extensions: "<<context->extensions().toList();
            initedOpenGLFunctions32=true;
        }
    }
    return GLFuncs;
}

#endif



#ifdef QFVisualize3DGLWidget_USES_QOPENGLWIDGET

QFVisualize3DGLWidget::QFVisualize3DGLWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    initedOpenGLFunctions32(false),
#else
QFVisualize3DGLWidget::QFVisualize3DGLWidget(QWidget *parent) :
    QGLWidget(parent),
#endif
    scene(this),
    camera(this)
{
    background=QColor("black");//QColor(75,75,75,255);
    ambient=QColor("white");//QColor(150,150,150,255);
    lastMaxDir=0;
    lastDirPos=true;
    xRot = 45;
    yRot = 60;
    zRot = 0;
    scale = 0.8f;
    showBounds=true;
    m_blend=AlphaBlending;
    m_interp=NoInterpolation;
    lastPos=QPoint(0,0);
    currentVData=-1;
    lutSize=0;


    doUpdate=true;


    //addTestGeometry();
}

QFVisualize3DGLWidget::~QFVisualize3DGLWidget()
{
    clearAllVolumeData();
}

QImage QFVisualize3DGLWidget::getImage()
{
#ifdef QFVisualize3DGLWidget_USES_QOPENGLWIDGET
    return grabFramebuffer();
#else
    return grabFrameBuffer(true);
#endif
}

void QFVisualize3DGLWidget::set_doUpdate(bool enabled) {
    doUpdate=enabled;
    if (enabled) {
        initializeGL();
        updateVolumeVisualization();
    }
}

QRgb QFVisualize3DGLWidget::applyCutPlanes(QRgb rgb, int xx, int yy, int zz, int vd)
{
    if (vd<0 || vd>=vdata.size()) return rgb;
    const VolumeData& vdat=vdata[vd];
    float x=xx;
    float y=yy;
    float z=zz;
    if (vdat.m_cp1_enabled) {
        double dcut=double(vdat.m_cp1_position)/100.0;
        switch (vdat.m_cp1_type) {
            case 0: // yz
                if (x>dcut*(float)vdat.sizeX) rgb=0;
                break;
            case 1: // -yz
                if (x<dcut*(float)vdat.sizeX) rgb=0;
                break;
            case 2: // xz
                if (y>dcut*(float)vdat.sizeY) rgb=0;
                break;
            case 3: // -xz
                if (y<dcut*(float)vdat.sizeY) rgb=0;
                break;
            case 4: // xy
                if (z>dcut*(float)vdat.sizeZ) rgb=0;
                break;
            case 5: // -xy
                if (z<dcut*(float)vdat.sizeZ) rgb=0;
                break;
        }

    }
    if (vdat.m_cp2_enabled) {
        double dcut=double(vdat.m_cp2_position)/100.0;
        switch (vdat.m_cp2_type) {
            case 0: // yz
                if (x>dcut*(float)vdat.sizeX) rgb=0;
                break;
            case 1: // -yz
                if (x<dcut*(float)vdat.sizeX) rgb=0;
                break;
            case 2: // xz
                if (y>dcut*(float)vdat.sizeY) rgb=0;
                break;
            case 3: // -xz
                if (y<dcut*(float)vdat.sizeY) rgb=0;
                break;
            case 4: // xy
                if (z>dcut*(float)vdat.sizeZ) rgb=0;
                break;
            case 5: // -xy
                if (z<dcut*(float)vdat.sizeZ) rgb=0;
                break;
        }

    }
    return rgb;
}

int QFVisualize3DGLWidget::addTestGeometry()
{
    VolumeData d;
    d.internalData=false;
    d.volumeData=NULL;
    d.sizeX=0;
    d.sizeY=0;
    d.sizeZ=0;
    d.transparentLowerThreshold=0.1;
    d.transparentUpperThreshold=DBL_MAX;
    d.vizMode=QFVisualize3DGLWidget::TextureRendering;
    d.showTeapot=true;

    vdata.append(d);
    currentVData=vdata.size()-1;
    updateVolumeVisualization();
    return vdata.size()-1;
}

int QFVisualize3DGLWidget::addCopiedVolumeData(double *data, int sX, int sY, int sZ, QFVisualize3DGLWidget::VisualizationMode vm, double pixSizeX, double pixSizeY, double pixSizeZ)
{
    float* d=(float*)malloc(sX*sY*sZ*sizeof(float));
    for (int i=0; i<sX*sY*sZ; i++) d[i]=data[i];
    return addVolumeData(d, sX, sY, sZ, vm, pixSizeX, pixSizeY, pixSizeZ, true);
}

void QFVisualize3DGLWidget::setCopiedVolumeData(double *data, int sX, int sY, int sZ, QFVisualize3DGLWidget::VisualizationMode vm, double pixSizeX, double pixSizeY, double pixSizeZ)
{
    float* d=(float*)malloc(sX*sY*sZ*sizeof(float));
    for (int i=0; i<sX*sY*sZ; i++) d[i]=data[i];
    setVolumeData(d, sX, sY, sZ, vm, pixSizeX, pixSizeY, pixSizeZ, true);
}

int QFVisualize3DGLWidget::addVolumeData(float *data, int sX, int sY, int sZ, VisualizationMode vm, double pixSizeX, double pixSizeY, double pixSizeZ, bool internal)
{
    //qDebug()<<"setVolumeData";
    VolumeData d;
    d.internalData=internal;
    d.volumeData=data;
    d.sizeX=sX;
    d.sizeY=sY;
    d.sizeZ=sZ;
    d.transparentLowerThreshold=0.2;
    d.transparentUpperThreshold=1;
    d.vizMode=vm;
    d.showTeapot=false;
    d.pixSizeX=pixSizeX;
    d.pixSizeY=pixSizeY;
    d.pixSizeZ=pixSizeZ;
    if (pixSizeX<0 || pixSizeY<0 || pixSizeZ<0 ) {
        d.pixSizeX=2.0/double(sX);
        d.pixSizeY=2.0/double(sY);
        d.pixSizeZ=2.0/double(sZ);
    }

    vdata.append(d);
    currentVData=vdata.size()-1;
    updateVolumeVisualization();
    return vdata.size()-1;
}

void QFVisualize3DGLWidget::setVolumeData(float *data, int sX, int sY, int sZ, QFVisualize3DGLWidget::VisualizationMode vm, double pixSizeX, double pixSizeY, double pixSizeZ, bool internal)
{
    //qDebug()<<"setVolumeData("<<currentVData<<") = "<<data;
    if (currentVData<0 || currentVData>=vdata.size()) return;
    vdata[currentVData].clear();
    vdata[currentVData].internalData=internal;
    vdata[currentVData].volumeData=data;
    vdata[currentVData].sizeX=sX;
    vdata[currentVData].sizeY=sY;
    vdata[currentVData].sizeZ=sZ;
    vdata[currentVData].showTeapot=false;
    vdata[currentVData].vizMode=vm;
    vdata[currentVData].pixSizeX=pixSizeX;
    vdata[currentVData].pixSizeY=pixSizeY;
    vdata[currentVData].pixSizeZ=pixSizeZ;
    if (pixSizeX<0 || pixSizeY<0 || pixSizeZ<0 ) {
        vdata[currentVData].pixSizeX=2.0/double(sX);
        vdata[currentVData].pixSizeY=2.0/double(sY);
        vdata[currentVData].pixSizeZ=2.0/double(sZ);
    }

    updateVolumeVisualization();
}

void QFVisualize3DGLWidget::clearCurrentVolumeData()
{
    if (currentVData<0 || currentVData>=vdata.size()) return;
    vdata[currentVData].clear();
    updateVolumeVisualization();
}

void QFVisualize3DGLWidget::deleteVolumeData(int i)
{
    if (currentVData==i) {
        if (currentVData>0) currentVData--;
    }
    vdata.removeAt(i);
    if (vdata.size()<=0) if (currentVData>0) currentVData=-1;
    updateVolumeVisualization();
}

void QFVisualize3DGLWidget::setTestGeometry()
{
    //qDebug()<<"setTeapot";
    if (currentVData<0 || currentVData>=vdata.size()) return;
    vdata[currentVData].clear();
    vdata[currentVData].showTeapot=true;
    updateVolumeVisualization();
}

void QFVisualize3DGLWidget::clearAllVolumeData()
{

    for (int i=0; i<vdata.size(); i++) {
        vdata[i].clear();
    }
    vdata.clear();
    updateVolumeVisualization();
}

double QFVisualize3DGLWidget::getTransparentLowerThreshold() const
{
    if (currentVData<0 || currentVData>=vdata.size()) return 0;
    return vdata[currentVData].transparentLowerThreshold;
}

double QFVisualize3DGLWidget::getTransparentUpperThreshold() const
{
    if (currentVData<0 || currentVData>=vdata.size()) return 1;
    return vdata[currentVData].transparentUpperThreshold ;
}

bool QFVisualize3DGLWidget::getGeoVisible() const
{
    if (currentVData<0 || currentVData>=vdata.size()) return true;
    return vdata[currentVData].visible ;
}



double QFVisualize3DGLWidget::xRotation() const
{
    return qNormalizeAngleO(xRot);
}

double QFVisualize3DGLWidget::yRotation() const
{
    return qNormalizeAngleO(yRot);
}

double QFVisualize3DGLWidget::zRotation() const
{
    return qNormalizeAngleO(zRot);
}

QColor QFVisualize3DGLWidget::singleColorColor() const
{
    if (currentVData<0 || currentVData>=vdata.size()) return QColor("grey");
    return vdata[currentVData].color;
}

bool QFVisualize3DGLWidget::singleColor() const
{
    if (currentVData<0 || currentVData>=vdata.size()) return true;
    return vdata[currentVData].singleColor;
}




void QFVisualize3DGLWidget::addStackNodeZ(int vd, QRgb* lut, int lutSize, float lmin, float lmax, bool posDir) {
    double dX=vdata[vd].pixSizeX;
    double dY=vdata[vd].pixSizeY;
    double dZ=vdata[vd].pixSizeZ;
    const int sX=vdata[vd].sizeX;
    const int sY=vdata[vd].sizeY;
    const int sZ=vdata[vd].sizeZ;


    int z0=sZ-1;
    int z1=-1;
    int dz=-1;
    if (!posDir) {
        z0=0;
        z1=sZ;
        dz=+1;
    }
    //qDebug()<<"nodesZ"<<posDir;;
    uint8_t* texture=(uint8_t*)malloc(sX*sY*4*sizeof(uint8_t));
    uint8_t* texture1=(uint8_t*)malloc(sX*sY*4*sizeof(uint8_t));
    for (int z=z0; z!=z1; z=z+dz) {


        float xx=arrayX2WorldX(sX, sX, dX);
        float yy=arrayY2WorldY(sY, sY, dY);
        float zz=arrayZ2WorldZ(z, sZ, dZ)+double(vd)/20.0*dZ;



        GLQuad* dataPlane=new GLQuad(&scene);
        scene.addSceneElement(dataPlane);
        if (scene.getSceneWidget()->interpolationMode()>=HighQualityInterpolation) {
            dataPlane->setShader(0);
        }

        if (posDir) {
            dataPlane->setOrderNo(-zz);
            dataPlane->appendTexCoord(QVector2D(0,0));
            dataPlane->appendVertex(QVector3D(-xx,-yy,zz));
            dataPlane->appendTexCoord(QVector2D(0,1));
            dataPlane->appendVertex(QVector3D( -xx,yy,zz));
            dataPlane->appendTexCoord(QVector2D(1,1));
            dataPlane->appendVertex(QVector3D(xx,yy,zz));
            dataPlane->appendTexCoord(QVector2D(1,0));
            dataPlane->appendVertex(QVector3D(xx,-yy,zz));
        } else {
            dataPlane->setOrderNo(zz);
            dataPlane->appendTexCoord(QVector2D(0,0));
            dataPlane->appendVertex(QVector3D(-xx,-yy,zz));
            dataPlane->appendTexCoord(QVector2D(1,0));
            dataPlane->appendVertex(QVector3D( xx,-yy,zz));
            dataPlane->appendTexCoord(QVector2D(1,1));
            dataPlane->appendVertex(QVector3D(xx,yy,zz));
            dataPlane->appendTexCoord(QVector2D(0,1));
            dataPlane->appendVertex(QVector3D(-xx,yy,zz));
        }

        const float* vdat=vdata[vd].volumeData;
        if (z==z0) {
            QRgb* img=(QRgb*)texture;
            for (int i=0; i<sY*sX; i++) {
                int x=i%sX;
                int y=i/sX;
                img[i]=applyCutPlanes(statisticsLookupLUT(vdat[z*sX*sY+i], lmin, lmax,lut, lutSize), x, y, z, vd);
            }
            img=(QRgb*)texture1;
            if (z+dz>=0 && z+dz<sZ) {
                for (int i=0; i<sY*sX; i++) {
                    int x=i%sX;
                    int y=i/sX;
                    img[i]=applyCutPlanes(statisticsLookupLUT(vdat[(z+dz)*sX*sY+i], lmin, lmax,lut, lutSize), x, y, z, vd);
                }
            } else {
                for (int i=0; i<sY*sX; i++) {
                    img[i]=0;//applyCutPlanes(0, x, y, z, vd);
                }
            }
        } else if (z!=z1-dz) {
            qSwap(texture, texture1);
            QRgb* img=(QRgb*)texture1;
            for (int i=0; i<sY*sX; i++) {
                int x=i%sX;
                int y=i/sX;
                img[i]=applyCutPlanes(statisticsLookupLUT(vdat[(z+dz)*sX*sY+i], lmin, lmax,lut, lutSize), x, y, z, vd);
            }
        } else {
            qSwap(texture, texture1);
            QRgb* img=(QRgb*)texture1;
            for (int i=0; i<sY*sX; i++) {
                img[i]=0;//applyCutPlanes(0, x, y, z, vd);
            }
        }

        dataPlane->addSetTexture(texture, sX, sY, GL_TEXTURE_2D, GL_TEXTURE0);
        if (scene.getSceneWidget()->interpolationMode()>LinearInterpolation) dataPlane->addMoreTexture(texture1, sX, sY, GL_TEXTURE_2D, GL_TEXTURE1);



    }
    free(texture);
    free(texture1);

}


void QFVisualize3DGLWidget::addStackNodeX(int vd, QRgb *lut, int lutSize, float lmin, float lmax, bool posDir)
{
    //QElapsedTimer timer;
    double dX=vdata[vd].pixSizeX;
    double dY=vdata[vd].pixSizeY;
    double dZ=vdata[vd].pixSizeZ;
    const int sX=vdata[vd].sizeX;
    const int sY=vdata[vd].sizeY;
    const int sZ=vdata[vd].sizeZ;
    int x0=sX-1;
    int x1=-1;
    int dx=-1;
    if (!posDir) {
        x0=0;
        x1=sX;
        dx=+1;
    }
    //qDebug()<<"nodesX"<<posDir;;
    //timer.start();
    uint8_t* texture=(uint8_t*)malloc(sZ*sY*4*sizeof(uint8_t));
    uint8_t* texture1=(uint8_t*)malloc(sZ*sY*4*sizeof(uint8_t));
    for (int x=x0; x!=x1; x=x+dx) {


        float xx=arrayX2WorldX(x, sX, dX)+double(vd)/20.0*dX;
        float yy=arrayY2WorldY(sY, sY, dY);
        float zz=arrayZ2WorldZ(sZ, sZ, dZ);



        GLQuad* dataPlane=new GLQuad(&scene);
        if (scene.getSceneWidget()->interpolationMode()>=HighQualityInterpolation) {
            dataPlane->setShader(0);
        }
        scene.addSceneElement(dataPlane);
        if (posDir) {
            dataPlane->setOrderNo(-xx);
            dataPlane->appendTexCoord(QVector2D(0,0));
            dataPlane->appendVertex(QVector3D(xx,-yy,-zz));
            dataPlane->appendTexCoord(QVector2D(0,1));
            dataPlane->appendVertex(QVector3D( xx,-yy,zz));
            dataPlane->appendTexCoord(QVector2D(1,1));
            dataPlane->appendVertex(QVector3D(xx,yy,zz));
            dataPlane->appendTexCoord(QVector2D(1,0));
            dataPlane->appendVertex(QVector3D(xx,yy,-zz));
        } else {
            dataPlane->appendTexCoord(QVector2D(0,0));
            dataPlane->setOrderNo(xx);
            dataPlane->appendVertex(QVector3D(xx,-yy,-zz));
            dataPlane->appendTexCoord(QVector2D(1,0));
            dataPlane->appendVertex(QVector3D( xx,yy,-zz));
            dataPlane->appendTexCoord(QVector2D(1,1));
            dataPlane->appendVertex(QVector3D(xx,yy,zz));
            dataPlane->appendTexCoord(QVector2D(0,1));
            dataPlane->appendVertex(QVector3D(xx,-yy,zz));
        }



        const float* vdat=vdata[vd].volumeData;
        if (x==x0) {
            QRgb* img=(QRgb*)texture;
            for (int z=0; z<sZ; z++) {
                for (int y=0; y<sY; y++) {
                    img[z*sY+y]=applyCutPlanes(statisticsLookupLUT(vdat[z*sX*sY+y*sX+x], lmin, lmax,lut, lutSize), x, y, z, vd);
                }
            }
            img=(QRgb*)texture1;
            for (int z=0; z<sZ; z++) {
                for (int y=0; y<sY; y++) {
                    img[z*sY+y]=applyCutPlanes(statisticsLookupLUT(vdat[z*sX*sY+y*sX+x+dx], lmin, lmax,lut, lutSize), x, y, z, vd);
                }
            }
            //if (x+dx<0 || x+dx>=sX) qDebug()<<"XERROR0 x="<<x<<x0<<x1<<dx;
        } else if (x!=x1-dx) {
            qSwap(texture, texture1); // texture0 = old texture 1
            QRgb* img=(QRgb*)texture1;
            for (int z=0; z<sZ; z++) {
                for (int y=0; y<sY; y++) {
                    img[z*sY+y]=applyCutPlanes(statisticsLookupLUT(vdat[z*sX*sY+y*sX+x+dx], lmin, lmax,lut, lutSize), x, y, z, vd);
                }
            }
            //if (x+dx<0 || x+dx>=sX) qDebug()<<"XERROR1 x="<<x<<x0<<x1<<dx;
        } else {
            qSwap(texture, texture1); // texture0 = old texture 1
            QRgb* img=(QRgb*)texture1; // texture1 = all 0
            for (int z=0; z<sZ; z++) {
                for (int y=0; y<sY; y++) {
                    img[z*sY+y]=0;//applyCutPlanes(0, x, y, z, vd);
                }
            }
        }


        dataPlane->addSetTexture(texture, sY, sZ, GL_TEXTURE_2D, GL_TEXTURE0);
        if (scene.getSceneWidget()->interpolationMode()>LinearInterpolation) dataPlane->addMoreTexture(texture1, sY, sY, GL_TEXTURE_2D, GL_TEXTURE1);

    }
    free(texture);
    free(texture1);

}

void QFVisualize3DGLWidget::addStackNodeY(int vd, QRgb *lut, int lutSize, float lmin, float lmax, bool posDir)
{

    double dX=vdata[vd].pixSizeX;
    double dY=vdata[vd].pixSizeY;
    double dZ=vdata[vd].pixSizeZ;
    const int sX=vdata[vd].sizeX;
    const int sY=vdata[vd].sizeY;
    const int sZ=vdata[vd].sizeZ;

    QVector3D n(0,1,0);
    int y0=sY-1;
    int y1=-1;
    int dy=-1;
    if (!posDir) {
        y0=0;
        y1=sY;
        dy=+1;
        n=QVector3D(0,-1,0);
    }
    //qDebug()<<"nodesY"<<posDir<<y0<<y1<<dy<<sX<<sY<<sZ;
    uint8_t* texture=(uint8_t*)malloc(sX*sZ*4*sizeof(uint8_t));
    uint8_t* texture1=(uint8_t*)malloc(sX*sZ*4*sizeof(uint8_t));
    for (int y=y0; y!=y1; y=y+dy) {


        float xx=arrayX2WorldX(sX, sX, dX);
        float yy=arrayY2WorldY(y, sY, dY)+double(vd)/20.0*dY;
        float zz=arrayZ2WorldZ(sZ, sZ, dZ);



        GLQuad* dataPlane=new GLQuad(&scene);
        if (scene.getSceneWidget()->interpolationMode()>=HighQualityInterpolation) {
            dataPlane->setShader(0);
        }
        scene.addSceneElement(dataPlane);
        if (posDir) {
            dataPlane->setOrderNo(-yy);
            dataPlane->appendTexCoord(QVector2D(0,0));
            dataPlane->appendVertex(QVector3D(-xx,yy,-zz), n);
            dataPlane->appendTexCoord(QVector2D(1,0));
            dataPlane->appendVertex(QVector3D(xx,yy,-zz), n);
            dataPlane->appendTexCoord(QVector2D(1,1));
            dataPlane->appendVertex(QVector3D(xx,yy,zz), n);
            dataPlane->appendTexCoord(QVector2D(0,1));
            dataPlane->appendVertex(QVector3D(-xx,yy,zz), n);

        } else {
            dataPlane->setOrderNo(yy);
            dataPlane->appendTexCoord(QVector2D(0,0));
            dataPlane->appendVertex(QVector3D(-xx,yy,-zz), n);
            dataPlane->appendTexCoord(QVector2D(0,1));
            dataPlane->appendVertex(QVector3D(-xx,yy,zz), n);
            dataPlane->appendTexCoord(QVector2D(1,1));
            dataPlane->appendVertex(QVector3D(xx,yy,zz), n);
            dataPlane->appendTexCoord(QVector2D(1,0));
            dataPlane->appendVertex(QVector3D(xx,yy,-zz), n);
        }



        const float* vdat=vdata[vd].volumeData;
        if (y==y0) {
            QRgb* img=(QRgb*)texture;
            for (int z=0; z<sZ; z++) {
                for (int x=0; x<sX; x++) {
                    img[z*sX+x]=applyCutPlanes(statisticsLookupLUT(vdat[z*sX*sY+y*sX+x], lmin, lmax,lut, lutSize), x, y, z, vd);
                }
            }
            img=(QRgb*)texture1;
            for (int z=0; z<sZ; z++) {
                for (int x=0; x<sX; x++) {
                    img[z*sX+x]=applyCutPlanes(statisticsLookupLUT(vdat[z*sX*sY+(y+dy)*sX+x], lmin, lmax,lut, lutSize), x, y, z, vd);
                }
            }
        } else if (y!=y1-dy) {
            qSwap(texture, texture1);
            QRgb* img=(QRgb*)texture1;
            for (int z=0; z<sZ; z++) {
                for (int x=0; x<sX; x++) {
                    img[z*sX+x]=applyCutPlanes(statisticsLookupLUT(vdat[z*sX*sY+(y+dy)*sX+x], lmin, lmax,lut, lutSize), x, y, z, vd);
                }
            }
        } else {
            qSwap(texture, texture1);
            QRgb* img=(QRgb*)texture1;
            for (int z=0; z<sZ; z++) {
                for (int x=0; x<sX; x++) {
                    img[z*sX+x]=0;//applyCutPlanes(0, x, y, z, vd);
                }
            }
        }

        //qDebug()<<"nodesY setting tex";
        dataPlane->addSetTexture(texture, sX, sZ, GL_TEXTURE_2D, GL_TEXTURE0);
        if (scene.getSceneWidget()->interpolationMode()>LinearInterpolation) dataPlane->addMoreTexture(texture1, sX, sZ, GL_TEXTURE_2D, GL_TEXTURE1);
    }
    free(texture);
    free(texture1);
    //qDebug()<<"nodesY done";
}

void QFVisualize3DGLWidget::updateVolumeVisualization(bool updateWidget)
{
    if (!doUpdate) return;
    QElapsedTimer timer;
    timer.start();

    //qDebug()<<"updateVolumeVisualization "<<m_interp<<m_blend;

    scene.clearSceneElements();
    scene.clearTextures();
    //scene.clearShaders();
    //updateShadersInScene();

    int lutSize=4096;
    findMaxViewDir(lastMaxDir, lastDirPos);


    for (int vd=0; vd<vdata.size(); vd++) {
        //qDebug()<<" adding vd="<<vd;
        if (vdata[vd].visible) {

            if (vdata[vd].showTeapot) {
                //builder << QGLTeapot();
                GLLines* l=new GLLines(&scene);
                l->appendLine(0,0,0,1,0,0, QColor("red"));
                l->appendLine(0,0,0,0,1,0, QColor("green"));
                l->appendLine(0,0,0,0,0,1, QColor("blue"));
                scene.addSceneElement(l);

                GLQuad* q=new GLQuad(&scene);
                q->setCube(QVector3D(1,1,1),0.5,0.5,0.5);
                scene.addSceneElement(q);
            } else if (vdata[vd].volumeData && vdata[vd].sizeX>0 && vdata[vd].sizeY>0 && vdata[vd].sizeZ>0) {

                ////////////////////////////////////////////////////////////////////////////////////////////////////
                // show bounding box
                ////////////////////////////////////////////////////////////////////////////////////////////////////
                if (showBounds) {

                    float xx=arrayX2WorldX(-1, vdata[vd].sizeX, vdata[vd].pixSizeX);
                    float yy=arrayY2WorldY(-1, vdata[vd].sizeY, vdata[vd].pixSizeY);
                    float zz=arrayZ2WorldZ(-1, vdata[vd].sizeZ, vdata[vd].pixSizeZ);
                    float xx1=arrayX2WorldX(vdata[vd].sizeX+1, vdata[vd].sizeX, vdata[vd].pixSizeX);
                    float yy1=arrayY2WorldY(vdata[vd].sizeY+1, vdata[vd].sizeY, vdata[vd].pixSizeY);
                    float zz1=arrayZ2WorldZ(vdata[vd].sizeZ+1, vdata[vd].sizeZ, vdata[vd].pixSizeZ);

                    QVector3D start(xx,yy,zz);

                    GLLines* l=new GLLines(&scene);
                    l->appendLine(start, QVector3D(xx1,yy,zz), QColor("red"));
                    l->appendLine(start, QVector3D(xx,yy1,zz), QColor("green"));
                    l->appendLine(start, QVector3D(xx,yy,zz1), QColor("blue"));
                    l->setLineWidth(2);
                    l->setOrderNo(-1e100);
                    scene.addSceneElement(l);


                    l=new GLLines(&scene);
                    l->appendLine(QVector3D(xx1,yy,zz), QVector3D(xx1,yy,zz1), QColor("darkblue"));
                    l->appendLine(QVector3D(xx1,yy,zz), QVector3D(xx1,yy1,zz), QColor("darkblue"));
                    l->appendLine(QVector3D(xx,yy1,zz), QVector3D(xx,yy1,zz1), QColor("darkblue"));
                    l->appendLine(QVector3D(xx,yy1,zz), QVector3D(xx1,yy1,zz), QColor("darkblue"));
                    l->appendLine(QVector3D(xx,yy,zz1), QVector3D(xx,yy1,zz1), QColor("darkblue"));
                    l->appendLine(QVector3D(xx,yy,zz1), QVector3D(xx1,yy,zz1), QColor("darkblue"));
                    l->appendLine(QVector3D(xx1,yy1,zz1), QVector3D(xx1,yy1,zz), QColor("darkblue"));
                    l->appendLine(QVector3D(xx1,yy1,zz1), QVector3D(xx1,yy,zz1), QColor("darkblue"));
                    l->appendLine(QVector3D(xx1,yy1,zz1), QVector3D(xx,yy1,zz1), QColor("darkblue"));
                    l->setLineWidth(1);
                    l->setOrderNo(-1e100);
                    scene.addSceneElement(l);

                }

                ////////////////////////////////////////////////////////////////////////////////////////////////////
                // CALCULATE LUTS
                ////////////////////////////////////////////////////////////////////////////////////////////////////
                if (vdata[vd].lut.size()>0) {
                    lutSize=vdata[vd].lut.size();
                }
                if (vdata[vd].singleColor) {
                    lutSize=4096;
                    if (vdata[vd].lut_alpha.size()>0) {
                        lutSize=vdata[vd].lut_alpha.size();
                    }
                }
                //qDebug()<<vd<<": singleCol: "<<vdata[vd].singleColor<<vdata[vd].color<<vdata[vd].lut_alpha.size()<<lutSize;
                QRgb* lut=(QRgb*)malloc(lutSize*sizeof(QRgb));
                float lmin=0, lmax=0;
                statisticsMinMax(vdata[vd].volumeData, vdata[vd].sizeX*vdata[vd].sizeY*vdata[vd].sizeZ, lmin, lmax);
                for (int i=0; i<lutSize; i++) {
                    const double val1=double(i)/double(lutSize-1);
                    if (double(i)/double(lutSize-1)<vdata[vd].transparentLowerThreshold) lut[i]=QColor(Qt::transparent).rgba();
                    else if (double(i)/double(lutSize-1)>vdata[vd].transparentUpperThreshold) lut[i]=QColor(Qt::transparent).rgba();
                    else {
                        if (!vdata[vd].singleColor && vdata[vd].lut.size()==lutSize && vdata[vd].lut_alpha.size()==lutSize) {
                            lut[i]=qGLRGBAPreM(qRed(vdata[vd].lut[i]), qGreen(vdata[vd].lut[i]), qBlue(vdata[vd].lut[i]), vdata[vd].lut_alpha[i]);
                            if (vdata[vd].lut_alpha[i]==0) lut[i]=QColor(Qt::transparent).rgba();
                        } else if (!vdata[vd].singleColor && vdata[vd].lut.size()==lutSize) {
                            lut[i]=qGLRGBAPreM(qRed(vdata[vd].lut[i]), qGreen(vdata[vd].lut[i]), qBlue(vdata[vd].lut[i]), qAlpha(vdata[vd].lut[i]));
                        } else if (vdata[vd].singleColor && vdata[vd].lut_alpha.size()==lutSize) {
                            lut[i]=qGLRGBAPreM(vdata[vd].color.red(), vdata[vd].color.green(), vdata[vd].color.blue(), vdata[vd].lut_alpha[i]);
                        } else {
                            if (vdata[vd].singleColor) lut[i]=qGLRGBAPreM(vdata[vd].color.red(), vdata[vd].color.green(), vdata[vd].color.blue(), val1*127.0+127.0);
                            else lut[i]=qGLRGBAPreM(val1*255.0, val1*255.0, val1*255.0, val1*127.0+127.0);
                        }
                    }
                    //qDebug()<<QString("lut[%2] = 0x%1").arg(lut[i],8,16,QLatin1Char('0')).arg(i, 3, 10, QLatin1Char(' '));
                }

                ////////////////////////////////////////////////////////////////////////////////////////////////////
                // add objects for visualization
                ////////////////////////////////////////////////////////////////////////////////////////////////////
                if (vdata[vd].vizMode==QFVisualize3DGLWidget::TextureRendering) {
                    switch(lastMaxDir) {
                        case 0: // X-direction
                            addStackNodeX(vd,  lut, lutSize, lmin, lmax, lastDirPos);
                            break;
                        case 1: // Y-direction
                            addStackNodeY(vd,  lut, lutSize, lmin, lmax, lastDirPos);
                            break;
                        default:
                        case 2: // Z-direction
                            addStackNodeZ(vd,  lut, lutSize, lmin, lmax, lastDirPos);
                            break;
                    }
                }


                ////////////////////////////////////////////////////////////////////////////////////////////////////
                // CLEANUP
                ////////////////////////////////////////////////////////////////////////////////////////////////////
                if (lut) free(lut);
            }
        }
    }
    //scene = builder.finalizedSceneNode();

    //qDumpScene(scene);

    scene.finalizeScene();

    //qDebug()<<"updateVolumeVisualization() took "<<timer.elapsed()<<"ms";

    timer.start();
    if (updateWidget) {
        //initializeGL();
        updateGL();
    }
    //qDebug()<<"updateVolumeVisualization():updateGL() took "<<timer.elapsed()<<"ms";
}

void QFVisualize3DGLWidget::setVisualizationMode(QFVisualize3DGLWidget::VisualizationMode mode)
{
    if (currentVData<0 || currentVData>=vdata.size()) return;

    if (vdata[currentVData].vizMode!=mode) {
        vdata[currentVData].vizMode=mode;
        updateVolumeVisualization();
    }
}

void QFVisualize3DGLWidget::setLUT(const QVector<int> &lut)
{
    if (currentVData<0 || currentVData>=vdata.size()) return;
    vdata[currentVData].lut=lut;
    updateVolumeVisualization();
}

void QFVisualize3DGLWidget::setAlphaLUT(const QVector<uint8_t> &lut)
{
    if (currentVData<0 || currentVData>=vdata.size()) return;
    vdata[currentVData].lut_alpha=lut;
    updateVolumeVisualization();
    //qDebug()<<"alpha: "<<lut.first()<<" ... "<<lut.last();
}

void QFVisualize3DGLWidget::setLUT(const QVector<QRgb> &lut)
{
    QVector<int> dd(lut.size());
    for (int i=0; i<lut.size(); i++) dd[i]=lut[i];
    setLUT(dd);
}

void QFVisualize3DGLWidget::setAlphaLUT(const QVector<int> &lut)
{
    QVector<uint8_t> dd(lut.size());
    for (int i=0; i<lut.size(); i++) dd[i]=lut[i];
    setAlphaLUT(dd);
}

void QFVisualize3DGLWidget::setSingleColor(bool singlecolor)
{
    if (currentVData<0 || currentVData>=vdata.size()) return;
    vdata[currentVData].singleColor=singlecolor;
    updateVolumeVisualization();
}

void QFVisualize3DGLWidget::setColor(QColor color)
{
    if (currentVData<0 || currentVData>=vdata.size()) return;
    vdata[currentVData].color=color;
    updateVolumeVisualization();
}

void QFVisualize3DGLWidget::setTransparentLowerThreshold(double val)
{
    if (currentVData<0 || currentVData>=vdata.size()) return;
    vdata[currentVData].transparentLowerThreshold=qBound(0.0,val,1.0);
    updateVolumeVisualization();
}

void QFVisualize3DGLWidget::setTransparentUpperThreshold(double val)
{
    if (currentVData<0 || currentVData>=vdata.size()) return;
    vdata[currentVData].transparentUpperThreshold=qBound(0.0,val,1.0);
    updateVolumeVisualization();
}

void QFVisualize3DGLWidget::setGeoVisible(bool v)
{
    if (currentVData<0 || currentVData>=vdata.size()) return;
    vdata[currentVData].visible=v;
    updateVolumeVisualization();

}



bool QFVisualize3DGLWidget::cutPlane1Enabled() const
{
    if (currentVData<0 || currentVData>=vdata.size()) return false;
    return vdata[currentVData].m_cp1_enabled;
}

int QFVisualize3DGLWidget::cutPlane1Type() const
{
    if (currentVData<0 || currentVData>=vdata.size()) return 0;
    return vdata[currentVData].m_cp1_type;
}

int QFVisualize3DGLWidget::cutPlane1Position() const
{
    if (currentVData<0 || currentVData>=vdata.size()) return 50;
    return vdata[currentVData].m_cp1_position;
}

void QFVisualize3DGLWidget::setCutPlane1Enabled(bool en)
{
    if (currentVData<0 || currentVData>=vdata.size()) return;
    vdata[currentVData].m_cp1_enabled=en;
    updateVolumeVisualization();
}

void QFVisualize3DGLWidget::setCutPlane1Type(int t)
{
    if (currentVData<0 || currentVData>=vdata.size()) return;
    vdata[currentVData].m_cp1_type=t;
    updateVolumeVisualization();
}

void QFVisualize3DGLWidget::setCutPlane1Position(int p)
{
    if (currentVData<0 || currentVData>=vdata.size()) return;
    vdata[currentVData].m_cp1_position=p;
    updateVolumeVisualization();
}



bool QFVisualize3DGLWidget::cutPlane2Enabled() const
{
    if (currentVData<0 || currentVData>=vdata.size()) return false;
    return vdata[currentVData].m_cp2_enabled;
}

int QFVisualize3DGLWidget::cutPlane2Type() const
{
    if (currentVData<0 || currentVData>=vdata.size()) return 0;
    return vdata[currentVData].m_cp2_type;
}

int QFVisualize3DGLWidget::cutPlane2Position() const
{
    if (currentVData<0 || currentVData>=vdata.size()) return 50;
    return vdata[currentVData].m_cp2_position;
}

void QFVisualize3DGLWidget::setCutPlane2Enabled(bool en)
{
    if (currentVData<0 || currentVData>=vdata.size()) return;
    vdata[currentVData].m_cp2_enabled=en;
    updateVolumeVisualization();
}

void QFVisualize3DGLWidget::setCutPlane2Type(int t)
{
    if (currentVData<0 || currentVData>=vdata.size()) return;
    vdata[currentVData].m_cp2_type=t;
    updateVolumeVisualization();
}

void QFVisualize3DGLWidget::setCutPlane2Position(int p)
{
    if (currentVData<0 || currentVData>=vdata.size()) return;
    vdata[currentVData].m_cp2_position=p;
    updateVolumeVisualization();
}



void QFVisualize3DGLWidget::setXRotation(double angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        if (doUpdate) updateGL();
    }
}

void QFVisualize3DGLWidget::setYRotation(double angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        if (doUpdate) updateGL();
    }
}

void QFVisualize3DGLWidget::setZRotation(double angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        if (doUpdate) updateGL();
    }
}

void QFVisualize3DGLWidget::setBackgroundColor(QColor col)
{
    background=col;
    if (doUpdate) {
        initializeGL();
        updateGL();
    }
}

void QFVisualize3DGLWidget::setAmbientColor(QColor col)
{
    ambient=col;
    if (doUpdate) {
        initializeGL();
        updateGL();
    }

}

void QFVisualize3DGLWidget::setShowBounds(bool en)
{
    showBounds=en;
    updateVolumeVisualization();
}

bool QFVisualize3DGLWidget::getShowBounds() const
{
    return showBounds;
}

void QFVisualize3DGLWidget::setBlendMode(QFVisualize3DGLWidget::BlendMode m)
{
    m_blend=m;
    if (doUpdate) {
        initializeGL();
        updateGL();
    }

}

void QFVisualize3DGLWidget::setInterpolationMode(QFVisualize3DGLWidget::InterpolationMode m)
{
    //qDebug()<<"setInterpolationMode("<<m<<")";
    m_interp=m;
    initializeGL();
    updateVolumeVisualization();
}


void QFVisualize3DGLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void QFVisualize3DGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
        int dy = event->y() - lastPos.y();

        if (event->buttons() & Qt::LeftButton) {
            setXRotation(xRot + 0.35 * (double)dy);
            setYRotation(yRot + 0.35 * (double) dx);
        } else if (event->buttons() & Qt::RightButton) {
            setXRotation(xRot + 0.35 * (double) dy);
            setZRotation(zRot + 0.35 * (double) dx);
        }
        lastPos = event->pos();
}

void QFVisualize3DGLWidget::wheelEvent(QWheelEvent * e)
{
    e->delta() > 0 ? scale += scale*0.1f : scale -= scale*0.1f;
    if (doUpdate) {
        updateGL();
    }
}

void QFVisualize3DGLWidget::findMaxViewDir(int &maxDir, bool &dirPos)
{

    GLfloat matrice[]={0,0,0,0,
                       0,0,0,0,
                       0,0,0,0,
                       0,0,0,0};

/*#ifdef QFVisualize3DGLWidget_USES_QOPENGLWIDGET
    OPENGLFUNCTIONSCLASS *GLFuncs = getOpenGLFunctions();
    if (!GLFuncs) return;
    GLFuncs->glGetFloatv(GL_MODELVIEW_MATRIX, matrice);
#else*/
    glGetFloatv(GL_MODELVIEW_MATRIX, matrice);
//#endif
    QMatrix4x4 mat(matrice[0],matrice[1],matrice[2],matrice[3],
                   matrice[4],matrice[5],matrice[6],matrice[7],
                   matrice[8],matrice[9],matrice[10],matrice[11],
                   matrice[12],matrice[13],matrice[14],matrice[15]);

    QVector4D view(0,0,-1,0);

    QVector3D vd=mat.map(view).toVector3D();
    QVector3D vabsd;
    vabsd.setX(fabs(vd.x()));
    vabsd.setY(fabs(vd.y()));
    vabsd.setZ(fabs(vd.z()));
    maxDir=0;
    dirPos=(vd.x()>0);
    //qDebug()<<vabsd.x()<<vabsd.y()<<vabsd.z();
    if (vabsd.x()>=vabsd.y() && vabsd.x()>=vabsd.z()) {
        maxDir=0;
        dirPos=(vd.x()>0);
    } else if (vabsd.y()>=vabsd.x() && vabsd.y()>=vabsd.z()) {
        maxDir=1;
        dirPos=(vd.y()>0);
    } else if (vabsd.z()>=vabsd.x() && vabsd.z()>=vabsd.y()) {
        maxDir=2;
        dirPos=(vd.z()>0);
    }
}










void QFVisualize3DGLWidget::resizeGL(int width, int height)
{
    //qDebug()<<"resizeGL";
    makeCurrent();
#ifdef QFVisualize3DGLWidget_USES_QOPENGLWIDGET
    OPENGLFUNCTIONSCLASS *GLFuncs = getOpenGLFunctions();
    if (!GLFuncs) return;
#endif
    GLViewport( 0, 0, width, height );


    camera.draw();


    GLMatrixMode(GL_MODELVIEW);
    GLLoadIdentity();


}

void QFVisualize3DGLWidget::initializeGL()
{

    //qDebug()<<"initializeGL "<<m_interp<<m_blend;
    makeCurrent();

#ifdef QFVisualize3DGLWidget_USES_QOPENGLWIDGET
    OPENGLFUNCTIONSCLASS *GLFuncs = getOpenGLFunctions();
    if (!GLFuncs) return;
#endif


    GLEnable(GL_CULL_FACE);
    GLEnable( GL_COLOR_MATERIAL );
    GLShadeModel(GL_SMOOTH);
    GLEnable(GL_LIGHTING);
    GLEnable(GL_MULTISAMPLE);


    GLLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );
    GLEnable( GL_DEPTH_TEST );
    GLDepthFunc( GL_LESS );

    static GLfloat temp[]={0,0,0,0};

    GLClearColor(background.redF(), background.greenF(), background.blueF(), background.alphaF());
    qSetColor(temp, ambient);
    GLLightModelfv(GL_LIGHT_MODEL_AMBIENT, temp);

    for (int i=0; i<light.size(); i++) {
        light[i].draw();
    }

    GLEnable(GL_BLEND);

    if (m_blend==MaximumIntensity) {
        GLBlendEquation( GL_MAX);
        GLBlendFunc( GL_SRC_COLOR, GL_DST_COLOR);
    } else {
        GLBlendEquation( GL_FUNC_ADD);
        GLBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    }
    //qDebug()<<"initializeGL "<<m_interp<<m_blend;

    GLHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );




    updateShadersInScene();
    //qDebug()<<"initializeGL after updateShadersInScene "<<m_interp<<m_blend;

}

void QFVisualize3DGLWidget::paintGL()
{
    //qDebug()<<"paintGL";


#ifdef QFVisualize3DGLWidget_USES_QOPENGLWIDGET
    OPENGLFUNCTIONSCLASS *GLFuncs = getOpenGLFunctions();
    if (!GLFuncs) return;
    GLFuncs->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLFuncs->glMatrixMode(GL_MODELVIEW);
    GLFuncs->glLoadIdentity();
    GLFuncs->glTranslatef(0.0, 0.0, -10.0);
    GLFuncs->glRotatef(xRot, 1.0, 0.0, 0.0);
    GLFuncs->glRotatef(yRot, 0.0, 1.0, 0.0);
    GLFuncs->glRotatef(zRot, 0.0, 0.0, 1.0);
    double w=1.0f;
    GLFuncs->glScalef(scale/w, scale/w, scale/w);
#else
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -10.0);
    glRotatef(xRot, 1.0, 0.0, 0.0);
    glRotatef(yRot, 0.0, 1.0, 0.0);
    glRotatef(zRot, 0.0, 0.0, 1.0);
    double w=1.0f;
    glScalef(scale/w, scale/w, scale/w);
#endif



    //if (!showTeapot) {
        int maxDir=0;
        bool dirPos=true;
        findMaxViewDir(maxDir, dirPos);
        if (lastMaxDir!=maxDir || lastDirPos!=dirPos) {
            updateVolumeVisualization(false);
        }
    //}
    tick();
    scene.draw();
    //tockAvgPrint("scene.draw()");

    // DRAW SCENE HERE!
}





void QFVisualize3DGLWidget::VolumeData::clear()
{
    if (internalData && volumeData && sizeX>0 && sizeY>0 && sizeZ>0) {
        free(volumeData);
    }
    showTeapot=false;
    internalData=false;
    volumeData=NULL;
    sizeX=sizeY=sizeZ=0;

}

void QFVisualize3DGLWidget::updateShadersInScene() {
    //qDebug()<<"updateShadersInScene "<<m_interp<<m_blend;
    scene.clearShaders();
    int shader=-1;
    if (m_interp==HighQualityInterpolation) {
        //qDebug()<<"updateShadersInScene loading high-quality shaders";
        scene.addShaderFromFile(":/QFVisualize3DGLWidget/volviz_stdvertexshader.vert", ":/QFVisualize3DGLWidget/volviz_stdfragmentshader.frag");
        shader=0;

    }

    //qDebug()<<"updateShadersInScene: done";
}

typedef void (APIENTRY * QFV_PFNGLACTIVETEXTUREARBPROC) (GLenum  texture);

void QFVisualize3DGLWidget::glActiveTexture(GLenum texture) {

    QFV_PFNGLACTIVETEXTUREARBPROC P=NULL;
    if (!P) P=(QFV_PFNGLACTIVETEXTUREARBPROC)context()->getProcAddress("glActiveTexture");
    //qDebug()<<"glActiveTexture["<<glActiveTextureP<<"]("<<texture<<")";
    if (P) P(texture);
}

typedef void (APIENTRY * QFV_GLDELETEPROGRAMP) (GLuint  program);

void QFVisualize3DGLWidget::glDeleteProgram(GLuint  program) {
    //qDebug()<<"glDeleteProgram: context "<<context()->isValid();
    QFV_GLDELETEPROGRAMP P=NULL;
    if (!P) P=(QFV_GLDELETEPROGRAMP)context()->getProcAddress("glDeleteProgram");
    //qDebug()<<"glDeleteProgram["<<P<<"]("<<program<<")";
    if (P) P(program);
}

typedef GLuint  (APIENTRY * QFV_GLCREATESHADERP) (GLenum   shaderType);

GLuint  QFVisualize3DGLWidget::glCreateShader(GLenum  shaderType) {
    QFV_GLCREATESHADERP P=NULL;
    if (!P) P=(QFV_GLCREATESHADERP)context()->getProcAddress("glCreateShader");
    if (P) return P(shaderType);
    return 0;
}

typedef void (APIENTRY * QFV_glShaderSourceP) (GLuint shader, GLsizei count, const GLchar **string, const GLint *length);

void QFVisualize3DGLWidget::glShaderSource(GLuint shader, GLsizei count, const GLchar **string, const GLint *length)
{
    QFV_glShaderSourceP P=NULL;
    if (!P) P=(QFV_glShaderSourceP)context()->getProcAddress("glShaderSource");
    if (P) P(shader, count, string, length);
}

typedef void (APIENTRY * QFV_glBlendEquationP) (GLenum mode);

void QFVisualize3DGLWidget::glBlendEquation(GLenum mode)
{
    static QFV_glBlendEquationP P=NULL;
    if (!P) P=(QFV_glBlendEquationP)context()->getProcAddress("glBlendEquation");
    if (P) P(mode);
}

typedef void (APIENTRY * QFV_glCompileShaderP) (GLuint shader);

void QFVisualize3DGLWidget::glCompileShader(GLuint shader)
{
    static QFV_glCompileShaderP P=NULL;
    if (!P) P=(QFV_glCompileShaderP)context()->getProcAddress("glCompileShader");
    if (P) P(shader);
}

typedef void (APIENTRY * QFV_glDeleteShaderP) (GLuint shader);

void QFVisualize3DGLWidget::glDeleteShader(GLuint shader)
{
    static QFV_glDeleteShaderP P=NULL;
    if (!P) P=(QFV_glDeleteShaderP)context()->getProcAddress("glDeleteShader");
    if (P) P(shader);
}



typedef void (APIENTRY * QFV_glGetShaderivP) (GLuint shader, GLenum pname, GLint *params);

void QFVisualize3DGLWidget::glGetShaderiv(GLuint shader, GLenum pname, GLint *params)
{
    static QFV_glGetShaderivP P=NULL;
    if (!P) P=(QFV_glGetShaderivP)context()->getProcAddress("glGetShaderiv");
    if (P) P(shader, pname, params);
}


typedef void (APIENTRY * QFV_glAttachShaderP) (GLuint program, GLuint shader);

void QFVisualize3DGLWidget::glAttachShader(GLuint program, GLuint shader)
{
    static QFV_glAttachShaderP P=NULL;
    if (!P) P=(QFV_glAttachShaderP)context()->getProcAddress("glAttachShader");
    if (P) P(program, shader);
}



typedef void (APIENTRY * QFV_glGetShaderInfoLogP) (GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog);

void QFVisualize3DGLWidget::glGetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog)
{
    QFV_glGetShaderInfoLogP P=NULL;
    if (!P) P=(QFV_glGetShaderInfoLogP)context()->getProcAddress("glGetShaderInfoLog");
    if (P) P(shader, maxLength, length, infoLog);
}



typedef void (APIENTRY * QFV_glGetProgramInfoLogP) (GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog);

void QFVisualize3DGLWidget::glGetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog)
{
    QFV_glGetProgramInfoLogP P=NULL;
    if (!P) P=(QFV_glGetProgramInfoLogP)context()->getProcAddress("glGetProgramInfoLog");
    if (P) P(program, maxLength, length, infoLog);
}



typedef void (APIENTRY * QFV_glUseProgramP) (GLuint program);

void QFVisualize3DGLWidget::glUseProgram(GLuint program)
{
    static QFV_glUseProgramP P=NULL;
    if (!P) P=(QFV_glUseProgramP)context()->getProcAddress("glUseProgram");
    if (P) P(program);
}


typedef GLint (APIENTRY * QFV_glGetUniformLocationP) (GLuint program, const GLchar *name);

GLint QFVisualize3DGLWidget::glGetUniformLocation(GLuint program, const GLchar *name)
{
    static QFV_glGetUniformLocationP P=NULL;
    if (!P) P=(QFV_glGetUniformLocationP)context()->getProcAddress("glGetUniformLocation");
    if (P) return P(program, name);
    return -1;
}



typedef void (APIENTRY * QFV_glUniform1iP) (GLint location, GLint v0);

void QFVisualize3DGLWidget::glUniform1i(GLint location, GLint v0)
{
    static QFV_glUniform1iP P=NULL;
    if (!P) P=(QFV_glUniform1iP)context()->getProcAddress("glUniform1i");
    if (P) P(location, v0);
}

typedef void (APIENTRY * QFV_glUniform1fP) (GLint location, GLfloat v0);

void QFVisualize3DGLWidget::glUniform1f(GLint location, GLfloat v0)
{
    static QFV_glUniform1fP P=NULL;
    if (!P) P=(QFV_glUniform1fP)context()->getProcAddress("glUniform1f");
    if (P) P(location, v0);
}






typedef GLuint (APIENTRY * QFV_glCreateProgramP) ();

GLuint QFVisualize3DGLWidget::glCreateProgram()
{
    static QFV_glCreateProgramP P=NULL;
    if (!P) P=(QFV_glCreateProgramP)context()->getProcAddress("glCreateProgram");
    if (P) return P();
    return 0;
}


typedef void (APIENTRY * QFV_glLinkProgramP) (GLuint program);

void QFVisualize3DGLWidget::glLinkProgram(GLuint program)
{
    static QFV_glLinkProgramP P=NULL;
    if (!P) P=(QFV_glLinkProgramP)context()->getProcAddress("glLinkProgram");
    if (P) P(program);
}




typedef void (APIENTRY * QFV_glGetProgramivP) (GLuint program, GLenum pname, GLint *params);

void QFVisualize3DGLWidget::glGetProgramiv(GLuint program, GLenum pname, GLint *params)
{
    static QFV_glGetProgramivP P=NULL;
    if (!P) P=(QFV_glGetProgramivP)context()->getProcAddress("glGetProgramiv");
    if (P) P(program, pname, params);
}



void QFVisualize3DGLWidget::setUniformVarI(int shader, QString varname, int value)
{
 #ifdef QFVisualize3DGLWidget_USES_QOPENGLWIDGET
    OPENGLFUNCTIONSCLASS *GLFuncs = getOpenGLFunctions();
    if (!GLFuncs) return;
    GLint loc=GLFuncs->glGetUniformLocation(scene.getShader(shader), varname.toLocal8Bit().data());
    qGLCheckError("glGetUniformLocation");
    GLFuncs->glUniform1i(loc, value);
    qGLCheckError("glUniform1i "+QString::number(loc));
#else
    GLint loc=glGetUniformLocation(scene.getShader(shader), varname.toLocal8Bit().data());
    qGLCheckError("glGetUniformLocation");
    glUniform1i(loc, value);
    qGLCheckError("glUniform1i "+QString::number(loc));
#endif
}

void QFVisualize3DGLWidget::setUniformVarF(int shader, QString varname, float value)
{
#ifdef QFVisualize3DGLWidget_USES_QOPENGLWIDGET
    OPENGLFUNCTIONSCLASS *GLFuncs = getOpenGLFunctions();
    if (!GLFuncs) return;
    GLint loc=GLFuncs->glGetUniformLocation(scene.getShader(shader), varname.toLocal8Bit().data());
    qGLCheckError("glGetUniformLocation");
    GLFuncs->glUniform1f(loc, value);
    qGLCheckError("glUniform1f "+QString::number(loc));
#else
    GLint loc=glGetUniformLocation(scene.getShader(shader), varname.toLocal8Bit().data());
    qGLCheckError("glGetUniformLocation");
    glUniform1f(loc, value);
    qGLCheckError("glUniform1f "+QString::number(loc));
#endif
}
