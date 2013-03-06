#ifndef QFRDRIMAGINGFCSCORRELATIONDIALOG_H
#define QFRDRIMAGINGFCSCORRELATIONDIALOG_H

#include <QDialog>
#include "qfproject.h"
#include "programoptions.h"
#include "qfrdrimagingfcsthreadprogress.h"
#include "qfpluginservices.h"
#include "qfrdrimagingfcsseriesdialog.h"
#include <stdint.h>

class QFRDRImagingFCSCorrelationJobThread; // forward
struct IMFCSJob; // forward
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
class QFRDRImagingFCSCorrelationDialog : public QDialog {
        Q_OBJECT

    public:

        QFRDRImagingFCSCorrelationDialog(QFPluginServices* pluginservices, ProgramOptions* opt, QWidget *parent = 0);
        ~QFRDRImagingFCSCorrelationDialog();

        void setProject(QFProject* project);

        void writeSettings();
        void readSettings();

        QStringList getFilesToAdd() const;
    public slots:
            void openFile(const QString& file);

    protected slots:
        void on_btnSelectImageFile_clicked();
        void on_btnSelectImageFileNoCount_clicked();
        void on_btnSelectBackgroundFile_clicked();
        void on_btnAddJob_clicked();
        void on_btnAddSeriesJob_clicked();
        void on_btnLoad_clicked();
        void on_btnLoadNoCount_clicked();
        void on_btnHelp_clicked();
        void on_spinP_valueChanged(int val);
        void on_spinS_valueChanged(int val);
        void on_spinM_valueChanged(int val);
        void on_spinDecay_valueChanged(double val);
        void on_spinDecay2_valueChanged(double val);
        void on_cmbCorrelator_currentIndexChanged(int idx);
        void on_cmbBackground_currentIndexChanged(int idx);
        void on_cmbBleachType_currentIndexChanged(int idx);
        void on_cmbDualView_currentIndexChanged(int index);

        void frameTimeChanged(double value);
        void frameRateChanged(double value);
        void updateProgress();
        void updateFromFile(bool readFiles=true, bool countFrames=true);
        void updateCorrelator();
        void updateFrameCount();
        void updateImageSize();
        void updateBleach();
        void startNextWaitingThread();
        void on_chkFirstFrame_clicked(bool checked);
        void on_chkLastFrame_clicked(bool checked);
        void setEditControlsEnabled(bool enabled);
        void on_btnDataExplorer_clicked();
        void readBackgroundFramesize();

    protected:
        void done(int status);
        bool allThreadsDone() const;
        int runningThreads() const;
        int waitingThreads() const;
        void ensureTiffReader();
        IMFCSJob initJob();
        void addJob(IMFCSJob jobin, bool ignoreDualView=false);
    private:
        QFPluginServices* pluginServices;
        Ui::QFRDRImagingFCSCorrelationDialog *ui;
        QPointer<QFProject> project;
        ProgramOptions* options;
        QString lastImagefileDir;
        QString lastImagefileFilter;
        QStringList imageFilters;
        QStringList imageFormatNames;
        QList<IMFCSJob> jobs;
        //QList<Job> jobsToAdd;
        QStringList filesToAdd;
        bool closing;
        QString inputconfigfile;



        int image_width;
        int image_height;
        int32_t frame_count;
        QString filenameDisplayed;

        int background_width;
        int background_height;
        int32_t background_frame_count;

        int getIDForProgress(const QFRDRImagingFCSThreadProgress* w) const;
        int getLayoutIDForProgress(const QWidget* w) const;
};

#endif // QFRDRIMAGINGFCSCORRELATIONDIALOG_H


