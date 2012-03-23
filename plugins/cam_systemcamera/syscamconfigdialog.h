#ifndef SYSCAMCONFIGDIALOG_H
#define SYSCAMCONFIGDIALOG_H

#include <QDialog>
#include <QSettings>
#include "qfecamsystemcamera.h"

class VideoCapture; //forward
namespace Ui {
    class SyscamConfigDialog;
}

class SyscamConfigDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit SyscamConfigDialog(VideoCapture* vid, int cam, QWidget *parent = 0);
        ~SyscamConfigDialog();

        void loadFromQSettings(QSettings& data);
        void saveToQSettings(QSettings& data);
        
    private:
        Ui::SyscamConfigDialog *ui;
};

#endif // SYSCAMCONFIGDIALOG_H
