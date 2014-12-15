#ifndef QFRDNNUMBERANDBRIGHTNESSPROCESSINGTHREAD_H
#define QFRDNNUMBERANDBRIGHTNESSPROCESSINGTHREAD_H

#include <QThread>
#include <QString>

class QFRDRNnumberAndBrightnessProcessingThread : public QThread
{
        Q_OBJECT
    public:
        struct Job {
            QString inputfile;
            QString readerID;
            QString backgroundfile;
            QString basename;
            QString outputname;
            int start;
            int end;
            double pixel_size;
            int dvmode;
            QString detectorMode;
            double gain;
            double excessNoise;
        };

        explicit QFRDRNnumberAndBrightnessProcessingThread(int threadID=0, QObject *parent = 0);

        virtual void run();

        void addJob(const Job& j);

    signals:
        void finishedJob(const QString& file);
        void setJobProgress(int value);
        void setJobProgress(int id, int value);
        void errorMessage(const QString& message);
    public slots:
        void cancel();
    protected:
        bool m_canceled;
        int threadID;
        QList<Job> jobs;

};

#endif // QFRDNNUMBERANDBRIGHTNESSPROCESSINGTHREAD_H
