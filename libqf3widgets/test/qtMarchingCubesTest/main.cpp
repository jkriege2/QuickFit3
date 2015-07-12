#include <QtGui/QApplication>
#include "qvolumerenderwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QVolumeRenderWidget w;
    w.show();
    
    return a.exec();
}
