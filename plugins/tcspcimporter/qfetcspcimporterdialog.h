#ifndef QFETCSPCImporterDIALOG_H
#define QFETCSPCImporterDIALOG_H

#include <QDialog>
#include "qfproject.h"
#include "programoptions.h"
#include "qfetcspcimporterthreadprogress.h"
#include "qfpluginservices.h"
#include <stdint.h>

class QFETCSPCImporterJobThread; // forward
struct TCSPCImporterJob; // forward
namespace Ui {
    class QFETCSPCImporterDialog; // forward
}

/*! \brief dialog used to correlate image sequences
    \ingroup qf3ext_tcspcimporter

    This dialog is called by QFRDRImagingFCSPlugin::correlateAndInsert(). It allows the user
    to create several correlation jobs that run in parallel and to wait until they are done.
    Finally getFilesToAdd() returns a list of file constaining ACFs and CCFs to be added to the
    project, by calling QFRDRImagingFCSPlugin::insertVideoCorrelatorFile().
*/
class QFETCSPCImporterDialog : public QDialog
{
    Q_OBJECT

public:

    QFETCSPCImporterDialog(QFPluginServices* pluginservices, ProgramOptions* opt, QWidget *parent = 0);
    ~QFETCSPCImporterDialog();

    void setProject(QFProject* project);

    void writeSettings();
    void readSettings();

    QList<QPair<QString, QString> > getFilesToAdd() const;

protected slots:
    void on_btnSelectFile_clicked();
    void on_btnAddJob_clicked();
    void on_btnLoad_clicked();
    void on_btnHelp_clicked();
    void on_spinP_valueChanged(int val);
    void on_spinS_valueChanged(int val);
    void on_spinM_valueChanged(int val);
    void on_cmbCorrelator_currentIndexChanged(int idx);

    void updateProgress();
    void updateFromFile(bool readFrameCount=true);
    void updateCorrelator();
    void updateDuration();
    void startNextWaitingThread();
    void on_chkFirstFrame_clicked(bool checked);
    void on_chkLastFrame_clicked(bool checked);
    void setEditControlsEnabled(bool enabled);
protected:
    void done(int status);
    bool allThreadsDone() const;
    int runningThreads() const;
    int waitingThreads() const;
private:
    QFPluginServices* pluginServices;
    Ui::QFETCSPCImporterDialog *ui;
    QPointer<QFProject> project;
    ProgramOptions* options;
    QString lastTCSPCFileDir;
    QString lastTCSPCFileFilter;
    QStringList tcspcFilters;
    QStringList tcspcFormatNames;
    QList<TCSPCImporterJob> jobs;
    //QList<Job> jobsToAdd;
    QList<QPair<QString, QString> > filesToAdd;
    bool closing;
    double duration;

    int getIDForProgress(const QFETCSPCImporterThreadProgress* w) const;
    int getLayoutIDForProgress(const QWidget* w) const;
};

#endif // QFETCSPCImporterDIALOG_H


