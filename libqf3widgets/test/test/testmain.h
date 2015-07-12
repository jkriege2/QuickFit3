#ifndef TESTMAIN_H
#define TESTMAIN_H

#include <QMainWindow>
#include <QtGui>
#include "qmodernprogresswidget.h"
#include "qmoretextobject.h"
#include "qenhancedtableview.h"

// forward
class QTabWidget;

class TestMain : public QMainWindow {
        Q_OBJECT
    public:
        TestMain(QWidget* parent=NULL);
        virtual ~TestMain();
    protected:
        virtual void showEvent(QShowEvent* event);
    protected slots:
        void incProgress();
    private:
        QTabWidget* tabMain;
        QModernProgressDialog* dlg;

        QModernProgressWidget* prg2;
        QModernProgressWidget* prg3;
        QModernProgressWidget* prg4;

        int progress;
        bool prg_up;

        QTextEdit* textEdit;
};

#endif // TESTMAIN_H
