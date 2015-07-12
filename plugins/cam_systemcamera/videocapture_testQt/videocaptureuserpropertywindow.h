#ifndef VIDEOCAPTUREUSERPROPERTYWINDOW_H
#define VIDEOCAPTUREUSERPROPERTYWINDOW_H

#include <QDialog>
#include <QFormLayout>
#include "../../videocapture.h"
#include <QList>
#include <QMap>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>

class VideoCaptureUserPropertyWindow : public QDialog
{
    Q_OBJECT

public:
    explicit VideoCaptureUserPropertyWindow(QWidget *parent = 0);
    ~VideoCaptureUserPropertyWindow();
    void init(VideoCapture* vid);
protected slots:
    void spinChanged(int value);
    void buttonClicked();

private:
    QFormLayout* layout;
    VideoCapture* vid;



    QMap<QObject*, VideoCapture::UserProperties> widgets;
    QMap<QObject*, int> buttons;
};

#endif // VIDEOCAPTUREUSERPROPERTYWINDOW_H
