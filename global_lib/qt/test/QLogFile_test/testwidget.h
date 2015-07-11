#ifndef TESTWIDGET_H
#define TESTWIDGET_H

#include <QLabel>
#include <QtGui/QWidget>
#include "../../qtlogfile.h"
#include <QPushButton>
#include <QElapsedTimer>
#include <QApplication>
#include "../../qtlogfile2.h"

class TestWidget : public QWidget
{
        Q_OBJECT
        
    public:
        TestWidget(QWidget *parent = 0);
        ~TestWidget();

    protected slots:
        void test();
    protected:
        QtLogFile* logOld;
        QtLogFile2* log2;
        QLabel* labResultsOld;
        QLabel* labResults2;
        QPushButton* btnTest;
};

#endif // TESTWIDGET_H
