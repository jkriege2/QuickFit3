#ifndef QFRDRIMAGINGFCSWIZARDCORRELATIONPROGRESS_H
#define QFRDRIMAGINGFCSWIZARDCORRELATIONPROGRESS_H

#include <QWidget>
#include "qfrdrimagingfcsthreadprogress.h"
#include "qfrdrimagingfcscorrelationjobthread.h"

namespace Ui {
    class QFRDRImagingFCSWizardCorrelationProgress;
}

class QFRDRImagingFCSWizardCorrelationProgress : public QWidget
{
        Q_OBJECT

    public:
        explicit QFRDRImagingFCSWizardCorrelationProgress(QWidget *parent = 0);
        ~QFRDRImagingFCSWizardCorrelationProgress();

        int getIDForProgress(const QFRDRImagingFCSThreadProgress* w) const;
        int getLayoutIDForProgress(const QWidget* w) const;
        void addJob(const IMFCSJob &jobin);

        bool allThreadsDone() const;

        QList<QFRDRImagingFCSCorrelationJobThread::Fileinfo> getFilesToAdd() const;
    signals:
        void correlationCompleted(bool complete);


    public slots:
        void updateProgress();
        void cancelThreads();
        void collectThreads();
    private:
        Ui::QFRDRImagingFCSWizardCorrelationProgress *ui;
    protected:
        QList<IMFCSJob> jobs;
        bool closing;
        QList<QFRDRImagingFCSCorrelationJobThread::Fileinfo> filesToAdd;
        int runningThreads() const;
        int waitingThreads() const;
        void startNextWaitingThread();

        bool lastAllDone;

};

#endif // QFRDRIMAGINGFCSWIZARDCORRELATIONPROGRESS_H
