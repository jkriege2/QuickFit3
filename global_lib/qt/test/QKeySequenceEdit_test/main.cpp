#include <QtGui/QApplication>
#include <QFormLayout>
#include "../../qkeysequenceedit.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QWidget w;
    QFormLayout fl(&w);
    w.setLayout(&fl);

    QKeySequenceEdit edt(&w);
    fl.addRow("key sequence:", &edt);

    w.show();
    
    return a.exec();
}
