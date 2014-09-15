#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include "qfcollapsibleframe.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
        Q_OBJECT
        
    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

    public slots:
        void updateWidgets();
        void updateRLabels();
        void on_cmbCollapsibleOrientation_currentIndexChanged(int index);
    private:
        Ui::MainWindow *ui;
        QList<QFCollapsibleFrame*> cframes;
};

#endif // MAINWINDOW_H
