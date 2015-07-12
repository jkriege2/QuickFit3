#include "testmain.h"
#include <QApplication>
#include <QCleanlooksStyle>

int main(int argc, char * argv[]) {
    QApplication app(argc, argv);

    TestMain win;
    win.show();
    return app.exec();
}

