#include "qvolumerenderwidget.h"

#include <QBasicTimer>
#include <QMouseEvent>

#include <cmath>
#include <QDebug>

#include "geometryengine.h"

QVolumeRenderWidget::QVolumeRenderWidget(QWidget *parent):
    QGLWidget(parent), timer(new QBasicTimer), geometries(new GeometryEngine)
{
}

QVolumeRenderWidget::~QVolumeRenderWidget() {
    delete timer; timer = 0;
    delete geometries; geometries = 0;
}

void QVolumeRenderWidget::mousePressEvent(QMouseEvent *e) {
    mousePressPosition = QVector2D(e->posF());
}

void QVolumeRenderWidget::mouseReleaseEvent(QMouseEvent *e) {
    // Mouse release position - mouse press position
    QVector2D diff = QVector2D(e->posF()) - mousePressPosition;

    // Rotation axis is perpendicular to the mouse position difference
    // vector
    QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();

    // Accelerate angular speed relative to the length of the mouse sweep
    qreal acc = diff.length() / 100.0;

    // Calculate new rotation axis as weighted sum
    rotationAxis = (rotationAxis * angularSpeed + n * acc).normalized();

    // Increase angular speed
    angularSpeed += acc;
}

void QVolumeRenderWidget::timerEvent(QTimerEvent *e) {
    Q_UNUSED(e);

    // Decrease angular speed (friction)
    angularSpeed *= 0.99;

    // Stop rotation when speed goes below threshold
    if (angularSpeed < 0.01)
        angularSpeed = 0.0;
    else {
        // Update rotation
        rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;

        // Update scene
        updateGL();
    }
}

void QVolumeRenderWidget::initializeGL() {
    qglClearColor(Qt::black);

//! [2]
    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);
//! [2]

    qDebug() << "Initializing geometries...";
    geometries->init();

    // using QBasicTimer because its faster that QTimer
    timer->start(12, this);
}

void QVolumeRenderWidget::resizeGL(int w, int h) {
    // Set OpenGL viewport to cover whole widget
    glViewport(0, 0, w, h);

    // Calculate aspect ratio
    qreal aspect = (qreal)w / ((qreal)h?h:1);

    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const qreal zNear = 3.0, zFar = 7.0, fov = 45.0;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);
}

void QVolumeRenderWidget::paintGL() {
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//! [6]
    // Calculate model view transformation
    QMatrix4x4 matrix;
    matrix.translate(0.0, 0.0, -5.0);
    matrix.rotate(rotation);

    // Draw cube geometry
    geometries->drawCubeGeometry();
}
