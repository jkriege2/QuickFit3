#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMessageBox>
#include "../../videocapture.h"
#include "videocaptureuserpropertywindow.h"

namespace Ui {
    class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

protected slots:
    void on_btnConnect_toggled(bool checked);
    void on_btnCapture_clicked();
    void on_btnCaptureContinuous_toggled(bool checked);
    void on_btnConfig_clicked();

    void captureTimed();

private:
    Ui::Widget *ui;
    VideoCapture* vid;
    VideoCaptureUserPropertyWindow* dlg;
};

#endif // WIDGET_H
