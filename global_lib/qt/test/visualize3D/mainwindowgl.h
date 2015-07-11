#ifndef MainWindowGL_H
#define MainWindowGL_H

#include <QMainWindow>
#include "jkqtpimagetools.h"

namespace Ui {
    class MainWindowGL;
}

class MainWindowGL : public QWidget
{
        Q_OBJECT
        
    public:
        explicit MainWindowGL(QWidget *parent = 0);
        ~MainWindowGL();

    protected slots:
        void update3DDataset();
        void setCurrent(int item);
        void writeSettings();

    private:
        Ui::MainWindowGL *ui;

        double* data3D;
        double* data3D2;
        double* data3D3;
        double* data3D4;
        int sX, sY, sZ;
        int ssX, ssY, ssZ;

        QMap<int, QString> mapLUT, mapLUTAlpha;
        QMap<int, int> mapDataSet;
        QMap<int, JKQTPMathImageColorPalette> mapColorPal;

};

#endif // MainWindowGL_H
