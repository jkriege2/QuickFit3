#ifndef QFRDRIMAGINGFCSCORRELATIONDIALOG_H
#define QFRDRIMAGINGFCSCORRELATIONDIALOG_H

#include <QDialog>
#include "qfproject.h"
#include "programoptions.h"
#include "qfrdrimagingfcsthreadprogress.h"

class QFRDRImagingFCSCorrelationJobThread; // forward
namespace Ui {
    class QFRDRImagingFCSCorrelationDialog; // forward
}

/*! \brief job description for correlation
    \ingroup qf3rdrdp_imaging_fcs
*/
struct Job {
    /** \brief progress widget for this job */
    QPointer<QFRDRImagingFCSThreadProgress> progress;
    /** \brief thread object for this job */
    QPointer<QFRDRImagingFCSCorrelationJobThread> thread;
    /** \brief name of the input file */
    QString filename;
    /** \brief fileformat of the input file */
    int fileFormat;
    /** \brief correlator to use */
    int correlator;
    /** \brief background correction mode */
    int backgroundCorrection;
    /** \brief offset to substract from frames */
    float backgroundOffset;
    /** \brief correlator: number of linear correlators */
    int S;
    /** \brief correlator: channels per lin. correlator */
    int P;
    /** \brief correlator: m factor */
    int m;
    /** \brief first frame to calculate */
    int32_t range_min;
    /** \brief last frame to calculate */
    int32_t range_max;
    /** \brief add job results to project */
    bool addToProject;
    /** \brief duration of a single frame */
    double frameTime;
    /** \brief output prefix */
    QString prefix;
    /** \brief calculate ACF of each pixel */
    bool acf;
    /** \brief calculate the CCF to the nearest neighbour pixels */
    bool ccf;
    /** \brief statistics over frames */
    uint32_t statistics_frames;
    /** \brief calculate statistics */
    bool statistics;
    /** \brief create video */
    bool video;
    /** \brief average over this number of frames, if creating a video */
    uint32_t video_frames;

};



/*! \brief dialog used to correlate image sequences
    \ingroup qf3rdrdp_imaging_fcs
*/
class QFRDRImagingFCSCorrelationDialog : public QWidget
{
    Q_OBJECT

public:

    QFRDRImagingFCSCorrelationDialog(ProgramOptions* opt, QWidget *parent = 0);
    ~QFRDRImagingFCSCorrelationDialog();

    void setProject(QFProject* project);

    void writeSettings();
    void readSettings();


protected slots:
    void on_btnSelectImageFile_clicked();
    void on_btnAddJob_clicked();
    void on_btnLoad_clicked();
    void on_spinP_valueChanged(int val);
    void on_spinS_valueChanged(int val);
    void on_spinM_valueChanged(int val);
    void on_cmbCorrelator_currentIndexChanged(int idx);
    void frameTimeChanged(double value);
    void frameRateChanged(double value);
    void updateProgress();
    void updateFromFile();
    void addJobToProject(Job job);
    void updateCorrelator();
    void startNextWaitingThread();
protected:
    void closeEvent(QCloseEvent * event);
    bool allThreadsDone() const;
    int runningThreads() const;
    int waitingThreads() const;
private:
    Ui::QFRDRImagingFCSCorrelationDialog *ui;
    QPointer<QFProject> project;
    ProgramOptions* options;
    QString lastImagefileDir;
    QString lastImagefileFilter;
    QStringList imageFilters;
    QStringList imageFormatNames;
    QList<Job> jobs;
    QList<Job> jobsToAdd;
    bool closing;

    int getIDForProgress(const QFRDRImagingFCSThreadProgress* w) const;
    int getLayoutIDForProgress(const QWidget* w) const;
};

#endif // QFRDRIMAGINGFCSCORRELATIONDIALOG_H


