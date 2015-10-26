#include <QApplication>

#include "controlWidget.h"
#include <QFileDialog>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);


    controlWidget* wdgt=new controlWidget(NULL);
    wdgt->show();


    return app.exec();
}
