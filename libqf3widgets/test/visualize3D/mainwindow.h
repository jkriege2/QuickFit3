#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QWidget
{
        Q_OBJECT
        
    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

    protected slots:
        void update3DDataset();

    private:
        Ui::MainWindow *ui;

        double* data3D;
        double* data3D2;
        double* data3D3;
        int sX, sY, sZ;

};

#endif // MAINWINDOW_H
