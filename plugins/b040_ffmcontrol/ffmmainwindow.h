#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QSplitter>
#include <QCoreApplication>
#include "qtlogfile.h"
#include "ffmexpdescriptionwidget.h"
#include "ffmimagewidget.h"
#include "ffmexpcontrolwidget.h"
#include "ffmmainlogfile.h"
#include "programoptions.h"
#include "qfpluginservices.h"
//#include "qfeb040ffmcontrol.h"


class ffmMainWindow : public QWidget//, public QFPluginLogService
{
    Q_OBJECT
    //Q_INTERFACES(QFPluginLogService)
public:
    explicit ffmMainWindow(QObject *parent = 0);
    //ffmMainWindow(MulticontrolStage *tstage, QObject *parent = 0);

    ~ffmMainWindow();

    void loadSettings(ProgramOptions* settings);
    void storeSettings(ProgramOptions* settings);

    /** \brief log project text message
     *  \param message the message to log
     */
    virtual void log_text(QString message);
    /** \brief log project warning message
     *  \param message the warning message to log
     */
    virtual void log_warning(QString message);
    /** \brief log project error message
     *  \param message the error message to log
     */
    virtual void log_error(QString message);
    MulticontrolStage* get_stage();

protected:
    /* GUI-Elements
    */
    QVisibleHandleSplitter *vertsplit;
    QVisibleHandleSplitter *horsplit;
    QGridLayout *ml;
    FFMExpDescriptionWidget *expdescr;
    FFMExpControlWidget *expcontrol;
    FFMImageWidget *expimage;
    QWidget *logwidget;
    QGridLayout *loglay;
    QtLogFile *logfile;
    //QFPluginLogService *logservice;

    void setUpUi();
    void setUpAct();

    MulticontrolStage *stage;


signals:

public slots:

};

#endif // MAINWINDOW_H
