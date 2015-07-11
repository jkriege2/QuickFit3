#include "testwidget.h"
#include <QVBoxLayout>
#include <QLabel>

TestWidget::TestWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout* lay=new QVBoxLayout(this);
    setLayout(lay);
    btnTest=new QPushButton(tr("run test ..."), this);
    lay->addWidget(btnTest);
    connect(btnTest, SIGNAL(clicked()), this, SLOT(test()));
    lay->addWidget(new QLabel(tr("<b>QtLogFile:</b>"), this));
    logOld=new QtLogFile(this);
    lay->addWidget(logOld, 1);
    lay->addWidget(new QLabel(tr("<b>Test results:</b>"), this));
    labResultsOld=new QLabel(this);
    lay->addWidget(labResultsOld);



    lay->addWidget(new QLabel(tr("<b>QtLogFile2:</b>"), this));
    log2=new QtLogFile2(this);
    lay->addWidget(log2, 1);
    lay->addWidget(new QLabel(tr("<b>Test results:</b>"), this));
    labResults2=new QLabel(this);
    lay->addWidget(labResults2);

}

TestWidget::~TestWidget()
{
    
}

#define testLogs(log, label, startV, end) {\
        QString results=label->text(); \
        QElapsedTimer timer; \
        timer.start(); \
        for (int i=startV; i<end; i++) { \
            if (i%3==0) log->log_text(tr("message %1\n").arg(i)); \
            if (i%3==1) log->log_warning(tr("message %1\n").arg(i)); \
            if (i%3==2) log->log_error(tr("message %1\n").arg(i)); \
            QApplication::processEvents(); \
        } \
        results.append(tr("avg(%2..%3) = %1 ms\n").arg(double(timer.nsecsElapsed())/1.0e6/double(end-startV)).arg(startV).arg(end)); \
        label->setText(results); \
        QApplication::processEvents(); \
    }


void TestWidget::test()
{
    resize(800,600);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    logOld->clearLog();
    logOld->close_logfile();
    if (QFile::exists("oldlogwidget.log")) QFile::remove("oldlogwidget.log");
    logOld->open_logfile("oldlogwidget.log", false);

    log2->clearLog();
    log2->close_logfile();
    if (QFile::exists("logwidget2.log")) QFile::remove("logwidget2.log");
    log2->open_logfile("logwidget2.log");

    testLogs(logOld, labResultsOld, 0,     10);
    testLogs(log2,   labResults2  , 0,     10);
    testLogs(logOld, labResultsOld, 10,    100);
    testLogs(log2,   labResults2  , 10,    100);
    testLogs(logOld, labResultsOld, 100,   1000);
    testLogs(log2,   labResults2  , 100,   1000);
    testLogs(logOld, labResultsOld, 1000,  10000);
    testLogs(log2,   labResults2  , 1000,  10000);

    QApplication::restoreOverrideCursor();

}
