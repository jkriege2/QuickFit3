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


#ifndef QFVisualize3DGLWidget_H
#define QFVisualize3DGLWidget_H
#include <QtGlobal>
#ifdef QFVisualize3DGLWidget_USES_QOPENGLWIDGET
#   if (QT_VERSION < QT_VERSION_CHECK(5, 4, 0))
#    undef QFVisualize3DGLWidget_USES_QOPENGLWIDGET
#  endif
#endif

#include <QtGlobal>
#ifdef QFVisualize3DGLWidget_USES_QOPENGLWIDGET
#  include <QOpenGLWidget>
#  include <QOpenGLFunctions>
#  include <QOpenGLFunctions_2_0>
#  define WIDGET_BASECLASS QOpenGLWidget
#  define OPENGLFUNCTIONSCLASS QOpenGLFunctions_2_0
#  define OPENGLFUNCTIONSVERSIONSTR "2.0"
#else
#  include <QGLWidget>
#  define WIDGET_BASECLASS QGLWidget
#endif

#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QList>

#ifdef Q_OS_MAC
#  include <glu.h>
#  include <glext.h>
#  include <gl.h>
#else
#  include <GL/glu.h>
#  include <GL/glext.h>
#  include <GL/gl.h>
#endif

//#include <QGLFunctions>
#include <QGLBuffer>
#include "../ticktock.h"
#include "../lib_imexport.h"
#include <QGLShader>
#include <QGLShaderProgram>


#ifndef GLchar
    typedef char GLchar;
#endif

/** \defgroup QFVisualize3D 3D Volume rendering widgets for Qt and OpenGL
 *  \ingroup tools
 * */

/** \brief this widgets actually performs the volume rendering
 *  \ingroup QFVisualize3D
 *
 *  \see <a href="http://www.cg.informatik.uni-siegen.de/data/Tutorials/EG2006/RTVG03_VolumeRendering.pdf">http://www.cg.informatik.uni-siegen.de/data/Tutorials/EG2006/RTVG03_VolumeRendering.pdf</a>
 */
#ifdef QFVisualize3DGLWidget_USES_QOPENGLWIDGET
class LIB_EXPORT QFVisualize3DGLWidget : public QOpenGLWidget, public TickTock
#else
class LIB_EXPORT QFVisualize3DGLWidget : public QGLWidget, public TickTock
#endif
{
        Q_OBJECT
    public:
#ifdef QFVisualize3DGLWidget_USES_QOPENGLWIDGET
    public:
        /** \brief update the OPenGL drawing in the widget */
        inline void updateGL() {
            update();
        }
    public:
        /** \brief returns a pointer to the OpenGL functions object (in Qt >=5.4) */
        OPENGLFUNCTIONSCLASS* getOpenGLFunctions();
    protected:
        bool initedOpenGLFunctions32;

#endif

    public:
        class GLScene; // forward
        /** \brief an element in the scene graph, displayed in a QFVisualize3DGLWidget */
        class LIB_EXPORT GLSceneElement {
            public:
                GLSceneElement(GLScene* scene);
                virtual ~GLSceneElement();
                virtual void draw()=0;
                virtual void finish()=0;
                void setColor(QColor c);
                void setTexture(int t);
                void setShader(int s);
                int addSetTexture(const QImage &image, GLenum mode=GL_TEXTURE_2D, GLenum activeTex=GL_TEXTURE0);
                int addSetTexture(uint8_t* image, int width, int height, GLenum mode=GL_TEXTURE_2D, GLenum activeTex=GL_TEXTURE0, GLenum format=GL_RGBA, GLenum type=GL_UNSIGNED_BYTE);
                void addMoreTexture(uint8_t* image, int width, int height, GLenum mode=GL_TEXTURE_2D, GLenum activeTex=GL_TEXTURE0, GLenum format=GL_RGBA, GLenum type=GL_UNSIGNED_BYTE);
                void setScene(GLScene* s);
                void setOrderNo(double i);
                double getOrderNo() const;
            protected:
                inline void glColor();
                inline void glColor(QColor color);
                GLScene* scene;
                QColor color;
                int texture;
                QVector<int> moreTextures;
                QMap<int, QColor> morecolors;
                double orderNo;
                int shader;
        };

        /** \brief a group of lines in the scene graph, displayed in a QFVisualize3DGLWidget */
        class LIB_EXPORT GLLines: public GLSceneElement {
            public:
                GLLines(GLScene* scene, GLenum mode=GL_LINES);
                void appendLine(const QVector3D &c1, const QVector3D& c2);
                void appendLine(double x1, double y1, double z1, double x2, double y2, double z2);
                void appendLine(const QVector3D &c1, const QVector3D& c2, QColor color);
                void appendLine(double x1, double y1, double z1, double x2, double y2, double z2, QColor color);
                void draw();
                virtual void finish();
                void setLineWidth(float lw);
            protected:
                QVector<QVector3D> vertexCoord;
                float lineWidth;
                GLenum  mode;
        };


        /** \brief a quadrilateral in the scene graph, displayed in a QFVisualize3DGLWidget */
        class LIB_EXPORT GLQuad: public GLSceneElement {
            public:
                GLQuad(GLScene* scene, GLenum mode=GL_QUADS);
                void appendTexCoord(const QVector2D& c);
                void appendVertex(const QVector3D& c);
                void appendVertex(float x, float y, float z);
                void appendVertex(const QVector3D& c, const QVector3D& n);
                void appendVertex(float x, float y, float z, float nx, float ny, float nz);

                void appendVertex(const QVector3D& c, QColor col);
                void appendVertex(const QVector3D& c, const QVector3D& n, QColor col);
                void appendVertex(float x, float y, float z, QColor col);
                void appendVertex(float x, float y, float z, float nx, float ny, float nz, QColor col);

                void appendCoord(const QVector3D& v, const QVector2D& tex);
                void appendCoord(const QVector3D& v, const QVector3D& n, const QVector2D& tex);

                void draw();
                void setCube(QVector3D center, float w, float h, float d);
                virtual void finish();
            protected:
                QVector<QVector2D> texCoord;
                QVector<QVector3D> vertexCoord;
                QVector<QVector3D> normals;
                GLenum  mode;

                QGLBuffer bufTex;
                QGLBuffer bufVer;
                QGLBuffer bufNorm;
        };



        /** \brief manages the scene graph, displayed in a QFVisualize3DGLWidget */
        class LIB_EXPORT GLScene {
            public:
                GLScene(QFVisualize3DGLWidget* sceneWidget);
                virtual ~GLScene();

                void addSceneElement(GLSceneElement* element);
                void clearSceneElements();

                void clearShaders();
                int addShaderFromFile(const QString& filenameVertex, const QString& filenameFragment);
                inline GLuint getShader(int i) const { return shaders.value(i, 0); }

                /** \brief sorts elements according to orderNo */
                void finalizeScene();

                void draw();
                void clearTextures();
                int addTexture(const QImage &image, GLenum mode=GL_TEXTURE_2D, GLenum activeTex=GL_TEXTURE0);
                /** \brief add texture in 32-bit RGBA format (or any format specified in format and type) */
                int addTexture(uint8_t* image, int width, int height, GLenum mode=GL_TEXTURE_2D, GLenum activeTex=GL_TEXTURE0, GLenum format=GL_RGBA, GLenum type=GL_UNSIGNED_BYTE);
                inline GLuint getTexture(int i) { if (i>=0 && i<textures.size()) return textures[i].id; else return 0; }
                inline GLuint getActiveTexture(int i) { if (i>=0 && i<textures.size()) return textures[i].activeTex; else return GL_TEXTURE0; }
                inline GLenum getTextureMode(int i) { if (i>=0 && i<textures.size()) return textures[i].mode; else return GL_TEXTURE_2D; }
                inline GLint getTextureWidth(int i) { if (i>=0 && i<textures.size()) return textures[i].width; else return 0; }
                inline GLint getTextureHeight(int i) { if (i>=0 && i<textures.size()) return textures[i].height; else return 0; }
                inline int getTextureCount() const { return textures.size(); }

                inline QFVisualize3DGLWidget* getSceneWidget() const { return sceneWidget; }

            protected:

                QVector<GLSceneElement*> scene;
                QFVisualize3DGLWidget* sceneWidget;
                struct texData {
                    GLuint id;
                    GLenum activeTex;
                    GLint width;
                    GLint height;
                    GLenum mode;
                };

                QVector<texData> textures;
                QList<GLuint> shaders;


        };
        /** \brief a light source for the scene graph, displayed in a QFVisualize3DGLWidget */
        class LIB_EXPORT GLLight {
            public:
                GLLight(GLenum  light, QFVisualize3DGLWidget* sceneWidget);
                void draw();
                void setAmbient(QColor col);
                void setDiffuse(QColor col);
                void setSpecular(QColor col);
                void setPos(QVector3D v);
                void setDir(QVector3D v);
            protected:
                QColor ambient;
                QColor diffuse;
                QColor specular;
                QVector3D pos;
                QVector3D dir;
                GLenum  light;
        };

        /** \brief represents in a QFVisualize3DGLWidget */
        class LIB_EXPORT GLCamera {
            public:
                GLCamera(QFVisualize3DGLWidget* sceneWidget);
                void draw();
            protected:
                QFVisualize3DGLWidget* sceneWidget;
        };

        /** \brief the type of volume rendering */
        enum VisualizationMode {
            /** \brief render by mapping textures on several planes in the volume */
            TextureRendering=0
        };

        /** \brief blend modes for the renderer */
        enum BlendMode {
            AlphaBlending=0,
            MaximumIntensity=1
        };

        /** \brief interpolation used to smooth the rendering */
        enum InterpolationMode {
            NoInterpolation=0,
            LinearInterpolation=1,
            HighQualityInterpolation=2
        };

        /** \brief represents a volume data object, together with its properties */
        struct LIB_EXPORT VolumeData {
            VolumeData(){
                transparentLowerThreshold=0;
                transparentUpperThreshold=1;
                vizMode=TextureRendering;
                volumeData=NULL;
                sizeX=sizeY=sizeZ=0;
                internalData=false;
                showTeapot=true;
                visible=true;
                m_cp1_enabled=false;
                m_cp1_type=0;
                m_cp1_position=50;
                m_cp2_enabled=false;
                m_cp2_type=0;
                m_cp2_position=50;
                color=QColor("grey");
                singleColor=false;
            };
            /** \brief percentile of lower invisible pixel values */
            double transparentLowerThreshold;
            /** \brief percentile of upper invisible pixel values */
            double transparentUpperThreshold;
            /** \brief how this should be rendered */
            VisualizationMode vizMode;
            /** \brief pointer to the actual data */
            float* volumeData;
            /** \brief size of the volume in pixels */
            int sizeX, sizeY, sizeZ;
            /** \brief size of one voxel in the volume in real coordinates */
            double pixSizeX, pixSizeY, pixSizeZ;
            /** \brief if true, data is managed internalls (i.e. is free, when no longer needed!) */
            bool internalData;
            /** \brief LUT for the colors */
            QVector<int> lut;
            /** \brief don't use color LUT, but a single color */
            bool singleColor;
            QColor color;
            /** \brief LUT for the alpha values */
            QVector<uint8_t> lut_alpha;
            bool showTeapot;
            /** \brief visbility of the object */
            bool visible;

            bool m_cp1_enabled;
            int m_cp1_type;
            int m_cp1_position;
            bool m_cp2_enabled;
            int m_cp2_type;
            int m_cp2_position;

            void clear();
        };

        explicit QFVisualize3DGLWidget(QWidget *parent = 0);
        virtual ~QFVisualize3DGLWidget();

        QImage getImage();

        /** \brief add a volume data record and sets it to display not volume data, but a test geometry (cube + coordinate system */
        int addTestGeometry();
        /** \brief add a new volume data object with the given data array and size ... makes a copy (downcast to float!!!) of the given data and manages it internally */
        int addCopiedVolumeData(double* data, int sX, int sY, int sZ, VisualizationMode vm=QFVisualize3DGLWidget::TextureRendering, double pixSizeX=-1, double pixSizeY=-1, double pixSizeZ=-1);
        /** \brief set the current volume data object with the given data array and size ... makes a copy (downcast to float!!!) of the given data and manages it internally */
        void setCopiedVolumeData(double *data, int sX, int sY, int sZ, VisualizationMode vm=QFVisualize3DGLWidget::TextureRendering, double pixSizeX=-1, double pixSizeY=-1, double pixSizeZ=-1);
        /** \brief add a new volume data object with the given data array and size ... sets the new volume data object as current object
         *
         * \note if \a internal == \c true, the data is afterwards owned by this object
         */
        int addVolumeData(float* data, int sX, int sY, int sZ, VisualizationMode vm=QFVisualize3DGLWidget::TextureRendering, double pixSizeX=-1, double pixSizeY=-1, double pixSizeZ=-1, bool internal=false);
        /** \brief set the current volume data object to the given data. If the old data was internally managed, it is freed at this point!
         *
         * \note if \a internal == \c true, the data is afterwards owned by this object
         */
        void setVolumeData(float *data, int sX, int sY, int sZ, VisualizationMode vm=QFVisualize3DGLWidget::TextureRendering, double pixSizeX=-1, double pixSizeY=-1, double pixSizeZ=-1, bool internal=false);
        /** \brief clear the current volume data object and draws nothing afterwards, if memory was managed internally, it is free'd at this point */
        void clearCurrentVolumeData();
        /** \brief delete the given volume data object */
        void deleteVolumeData(int i);
        /** \brief returns the number of volume data objects in this widget */
        int getVolumeDataCount() const { return vdata.size(); }
        /** \brief return the data record of the given volume data item */
        VolumeData getVolumeData(int i) const { return vdata[i]; }
        /** \brief returns the index of the current volume data object */
        int getCurrentVolumeDataID() const { return currentVData; }

        /** \brief displays the famous teapot */
        void setTestGeometry();
        /** \brief clear the current volume dataset */
        void clearAllVolumeData();
        /** \brief returns the VolumeData::transparentLowerThreshold of the current volume data object */
        double getTransparentLowerThreshold() const ;
        /** \brief returns the VolumeData::transparentUpperThreshold of the current volume data object */
        double getTransparentUpperThreshold() const;
        /** \brief returns the whether the current volume data object is visible */
        bool getGeoVisible() const;
        /** \brief returns the camera object */
        GLCamera& getCamera() { return camera; }
        /** \brief returns the current background color */
        QColor backgroundColor() const { return background; }
        /** \brief return the ambient illumination color */
        QColor ambientColor() const { return ambient; }
        /** \brief returns the current blend mode */
        BlendMode blendMode() const { return m_blend; }
        /** \brief returns the current texture interpolation mode */
        InterpolationMode interpolationMode() const { return m_interp; }

        bool cutPlane1Enabled() const;
        int cutPlane1Type() const;
        int cutPlane1Position() const;
        bool cutPlane2Enabled() const;
        int cutPlane2Type() const;
        int cutPlane2Position() const;

        /** \brief returns the current x-Axis rotation angle */
        double xRotation() const;
        /** \brief returns the current y-Axis rotation angle */
        double yRotation() const;
        /** \brief returns the current z-Axis rotation angle */
        double zRotation() const;

        QColor singleColorColor() const;
        bool singleColor() const;

    public slots:

        /** \brief update the displayed scene from the volume data set with setVolumeData(). This method rebuilds the whole geometry and recopies all necessary textures, so this might take a bit longer. */
        void updateVolumeVisualization(bool updateWidget=true);

        /** \brief set the visualization mode of the current volume data object */
        void setVisualizationMode(VisualizationMode mode);
        /** \brief set the color lookup table of the current volume data object (the lut should have N 4-byte entries in the QRgb layout, you may use QFLUTEditor to edit/send this )  */
        void setLUT(const QVector<int>& lut);
        /** \brief set the alpha-value lokup table of the current volume data object. The length should be the same, as the color LUT (see setLUT(), you may use QFLUTEditor to edit/send this ) */
        void setAlphaLUT(const QVector<uint8_t>& lut);
        /** \brief set the color lookup table of the current volume data object (the lut should have N 4-byte entries in the QRgb layout, you may use QFLUTEditor to edit/send this )  */
        void setLUT(const QVector<QRgb>& lut);
        /** \brief set the alpha-value lokup table of the current volume data object. The length should be the same, as the color LUT (see setLUT(), you may use QFLUTEditor to edit/send this ) */
        void setAlphaLUT(const QVector<int>& lut);

        void setSingleColor(bool singlecolor);
        void setColor(QColor color);

        /** \brief sets the VolumeData::transparentLowerThreshold of the current volume data object */
        void setTransparentLowerThreshold(double val);
        /** \brief sets the VolumeData::transparentUpperThreshold of the current volume data object */
        void setTransparentUpperThreshold(double val);
        /** \brief sets the visibility of the current volume data object */
        void setGeoVisible(bool v);

        void setCutPlane1Enabled(bool en);
        void setCutPlane1Type(int t);
        void setCutPlane1Position(int p);
        void setCutPlane2Enabled(bool en);
        void setCutPlane2Type(int t);
        void setCutPlane2Position(int p);


        /** \brief set the current x-Axis rotation angle */
        void setXRotation(double angle);
        /** \brief set the current y-Axis rotation angle */
        void setYRotation(double angle);
        /** \brief set the current z-Axis rotation angle */
        void setZRotation(double angle);

        /** \brief set the current background color */
        void setBackgroundColor(QColor col);

        /** \brief set the current ambient color */
        void setAmbientColor(QColor col);

        /** \brief set whether to display a line cube around the volume data  */
        void setShowBounds(bool en);

        bool getShowBounds() const;

        /** \brief set the current blending mode */
        void setBlendMode(BlendMode m);
        /** \brief set the current texture interpolation mode */
        void setInterpolationMode(InterpolationMode m);
        /** \brief set the current blending mode */
        void setBlendMode(int m) { setBlendMode((BlendMode)m); }
        /** \brief set the current blending mode */
        void setInterpolationMode(int m) { setInterpolationMode((InterpolationMode)m); }



        void setCurrentVData(int i) { currentVData=i; }

        bool get_doUpdate() const { return doUpdate; }
        void set_doUpdate(bool enabled);
    signals:
        /** \brief emitted when the current x-Axis rotation angle changes */
        void xRotationChanged(double angle);
        /** \brief emitted when the current y-Axis rotation angle changes */
        void yRotationChanged(double angle);
        /** \brief emitted when the current z-Axis rotation angle changes */
        void zRotationChanged(double angle);
    public:
        void qGLCheckError(const QString& message=QString());

    protected:
        inline QRgb applyCutPlanes(QRgb rgb, int x, int y, int z, int vd);
        /** \brief load and compile the used shaders */
        void updateShadersInScene();

         /** \brief workaround for a bug in QtOpenGL, which leads to an infinite loop when calling QGLFunctions::glActiveTexture() in release mode on windows (mingw)
          *
          *  \todo test whether this works/is needed on linux
          */
         void glActiveTexture(GLenum texture);
         void glDeleteProgram(GLuint program);
         GLuint glCreateShader(GLenum  shaderType);
         void glShaderSource(GLuint shader,  GLsizei count,  const GLchar **string,  const GLint *length);
         void glBlendEquation(GLenum mode);
         void glCompileShader(GLuint shader);
         void glDeleteShader(GLuint shader);
         void glGetShaderiv(GLuint shader,  GLenum pname,  GLint *params);
         void glGetShaderInfoLog(GLuint shader,  GLsizei maxLength,  GLsizei *length,  GLchar *infoLog);
         void glAttachShader(GLuint program,  GLuint shader);
         void glLinkProgram(GLuint program);
         void glGetProgramiv(GLuint program,  GLenum pname,  GLint *params);
         void glGetProgramInfoLog(GLuint program,  GLsizei maxLength,  GLsizei *length,  GLchar *infoLog);
         void glUseProgram(GLuint program);
         GLint glGetUniformLocation(GLuint program,  const GLchar *name);
         void glUniform1i(GLint location,  GLint v0);
         void glUniform1f(GLint location,  GLfloat  v0);

         void setUniformVarI(int shader, QString varname, int value);
         void setUniformVarF(int shader, QString varname, float value);

         GLuint glCreateProgram();

         void initializeGL();
         void paintGL();
         void resizeGL(int width, int height);
         void mousePressEvent(QMouseEvent *event);
         void mouseMoveEvent(QMouseEvent *event);
         void wheelEvent(QWheelEvent *);



         double xRot;
         double yRot;
         double zRot;
         float scale;
         QPoint lastPos;

         QList<VolumeData> vdata;

         int currentVData;

         BlendMode m_blend;
         InterpolationMode m_interp;
        GLScene scene;
        QList<GLLight> light;
        GLCamera camera;

        QColor background;
        QColor ambient;


        bool showBounds;

        int lutSize;

        int lastMaxDir;
        bool lastDirPos;

        bool doUpdate;

        void findMaxViewDir(int& maxDir, bool& dirPos);


        void addStackNodeX(int vd, QRgb* lut, int lutSize, float lmin, float lmax, bool posDir);
        void addStackNodeY(int vd, QRgb* lut, int lutSize, float lmin, float lmax, bool posDir);
        void addStackNodeZ(int vd, QRgb* lut, int lutSize, float lmin, float lmax, bool posDir);

        inline float arrayX2WorldX(int x, int sizeX, double dX) {
            return (float(x)-float(sizeX-1)/2.0)*dX;
        }
        inline float arrayY2WorldY(int y, int sizeY, double dY) {
            return (float(y)-float(sizeY-1)/2.0)*dY;
        }
        inline float arrayZ2WorldZ(int z, int sizeZ,  double dZ) {
            return (float(z)-float(sizeZ-1)/2.0)*dZ;
        }
    protected slots:
};

#endif // QFVisualize3DGLWidget_H

