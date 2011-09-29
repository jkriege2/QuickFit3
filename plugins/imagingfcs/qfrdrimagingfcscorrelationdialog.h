#ifndef QFRDRIMAGINGFCSCORRELATIONDIALOG_H
#define QFRDRIMAGINGFCSCORRELATIONDIALOG_H

#include <QDialog>
#include "qfproject.h"
#include "programoptions.h"
#include "qfrdrimagingfcsthreadprogress.h"
#include "qfpluginservices.h"
#include <stdint.h>

class QFRDRImagingFCSCorrelationJobThread; // forward
struct Job; // forward
namespace Ui {
    class QFRDRImagingFCSCorrelationDialog; // forward
}

/*! \brief dialog used to correlate image sequences
    \ingroup qf3rdrdp_imaging_fcs

    This dialog is called by QFRDRImagingFCSPlugin::correlateAndInsert(). It allows the user
    to create several correlation jobs that run in parallel and to wait until they are done.
    Finally getFilesToAdd() returns a list of file constaining ACFs and CCFs to be added to the
    project, by calling QFRDRImagingFCSPlugin::insertVideoCorrelatorFile().
*/
class QFRDRImagingFCSCorrelationDialog : public QDialog
{
    Q_OBJECT

public:

    QFRDRImagingFCSCorrelationDialog(QFPluginServices* pluginservices, ProgramOptions* opt, QWidget *parent = 0);
    ~QFRDRImagingFCSCorrelationDialog();

    void setProject(QFProject* project);

    void writeSettings();
    void readSettings();

    QStringList getFilesToAdd() const;

protected slots:
    void on_btnSelectImageFile_clicked();
    void on_btnAddJob_clicked();
    void on_btnLoad_clicked();
    void on_btnHelp_clicked();
    void on_spinP_valueChanged(int val);
    void on_spinS_valueChanged(int val);
    void on_spinM_valueChanged(int val);
    void on_cmbCorrelator_currentIndexChanged(int idx);
    void frameTimeChanged(double value);
    void frameRateChanged(double value);
    void updateProgress();
    void updateFromFile(bool readFrameCount=true);
    void updateCorrelator();
    void updateFrameCount();
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
    Ui::QFRDRImagingFCSCorrelationDialog *ui;
    QPointer<QFProject> project;
    ProgramOptions* options;
    QString lastImagefileDir;
    QString lastImagefileFilter;
    QStringList imageFilters;
    QStringList imageFormatNames;
    QList<Job> jobs;
    //QList<Job> jobsToAdd;
    QStringList filesToAdd;
    bool closing;
    int32_t frame_count;
    QString inputconfigfile;

    int getIDForProgress(const QFRDRImagingFCSThreadProgress* w) const;
    int getLayoutIDForProgress(const QWidget* w) const;
};

#endif // QFRDRIMAGINGFCSCORRELATIONDIALOG_H


