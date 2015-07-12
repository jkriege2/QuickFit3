#ifndef TESTMAIN_H
#define TESTMAIN_H
#include <QtGui>
#include <QtCore>
#include "jkdoubleedit.h"


class TestMain : public QWidget {
        Q_OBJECT
    public:
        /** Default constructor */
        TestMain(QWidget* parent=NULL);
        /** Default destructor */
        virtual ~TestMain();
    protected slots:
        void valueChanged(double value);
    private:
};

#endif // TESTMAIN_H
