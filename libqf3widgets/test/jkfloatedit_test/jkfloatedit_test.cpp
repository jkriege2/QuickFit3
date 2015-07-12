#include <QApplication>
#include "testmain.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    TestMain w;

    w.show();
    return app.exec();
}

