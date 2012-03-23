#ifndef SYSCAMCONFIGDIALOG_H
#define SYSCAMCONFIGDIALOG_H

#include <QDialog>
#include <QSettings>
#include "qfecamsystemcamera.h"
#include <QMap>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QCheckBox>

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
        VideoCapture* vid;
        int cam;
        QMap<int, QSpinBox*> spins;
        QMap<int, QCheckBox*> checks;
        QMap<int, QComboBox*> combos;
};

#endif // SYSCAMCONFIGDIALOG_H
