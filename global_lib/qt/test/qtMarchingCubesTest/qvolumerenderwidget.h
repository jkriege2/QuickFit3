#ifndef QVOLUMERENDERWIDGET_H
#define QVOLUMERENDERWIDGET_H

#include <QGLWidget>
#include <QGLFunctions>

#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>

class QBasicTimer;
class GeometryEngine;

class QVolumeRenderWidget : public QGLWidget, protected QGLFunctions {
        Q_OBJECT
        
    public:
        QVolumeRenderWidget(QWidget *parent = 0);
        virtual ~QVolumeRenderWidget();
    signals:

    public slots:

    protected:
        void mousePressEvent(QMouseEvent *e);
        void mouseReleaseEvent(QMouseEvent *e);
        void timerEvent(QTimerEvent *e);

        void initializeGL();
        void resizeGL(int w, int h);
        void paintGL();


    private:
        QBasicTimer *timer;
        GeometryEngine *geometries;

        QMatrix4x4 projection;

        QVector2D mousePressPosition;
        QVector3D rotationAxis;
        qreal angularSpeed;
        QQuaternion rotation;
};

#endif // QVOLUMERENDERWIDGET_H
