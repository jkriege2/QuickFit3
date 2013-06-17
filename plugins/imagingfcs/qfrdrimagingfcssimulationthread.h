#ifndef QFRDRIMAGINGFCSSIMULATIONTHREAD_H
#define QFRDRIMAGINGFCSSIMULATIONTHREAD_H

#include <QThread>
#include "tools.h"
#include "tinytiffwriter.h"
#include "../../extlibs/MersenneTwister.h"

class QFRDRImagingFCSSimulationThread : public QThread
{
        Q_OBJECT
    public:
        explicit QFRDRImagingFCSSimulationThread(QObject *parent = 0);
        int getCurrentFrame() const;

        GetSetMacro(double, DG);
        GetSetMacro(double, DR);
        GetSetMacro(double, DRG);
        GetSetMacro(double, psf_size_g);
        GetSetMacro(double, psf_size_r);
        GetSetMacro(double, crosstalk);
        GetSetMacro(double, VX);
        GetSetMacro(double, VY);
        GetSetMacro(double, pixel_size);
        GetSetMacro(double, frametime);
        GetSetMacro(long, frames);
        GetSetMacro(long, warmup);
        GetSetMacro(bool, dualView);
        GetSetMacro(QString, filename);
        GetSetMacro(int, width);
        GetSetMacro(int, height);
        GetSetMacro(double, brightnessG);
        GetSetMacro(double, brightnessR);
        GetSetMacro(double, background);
        GetSetMacro(double, backgroundNoise);
        GetSetMacro(int, walkersG);
        GetSetMacro(int, walkersR);
        GetSetMacro(int, walkersRG);
    public slots:
        void cancel();
        void waitForFinish();
    signals:
        void progress(int currentFrame);
        void statusMessage(const QString message);
    protected:
        virtual void run();

        struct WalkerData {
            double x;
            double y;
        };
        QVector<WalkerData> createWalkers(int count);
        void propagateWalkers(QVector<WalkerData>& walkers, double D);

        bool canceled;
        double DG;
        double DR;
        double DRG;

        double psf_size_g;
        double psf_size_r;
        double crosstalk;
        double VX;
        double VY;
        double pixel_size;
        double frametime;
        long frames;
        long warmup;

        bool dualView;
        QString filename;

        int width;
        int height;
        int currentFrame;
        double brightnessG;
        double brightnessR;
        int walkersG;
        int walkersR;
        int walkersRG;
        double background;
        double backgroundNoise;


        MTRand rng;
        
};

#endif // QFRDRIMAGINGFCSSIMULATIONTHREAD_H
