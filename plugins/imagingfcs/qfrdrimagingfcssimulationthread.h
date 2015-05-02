/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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

        GET_SET_MACRO(double, DG)
        GET_SET_MACRO(double, DR)
        GET_SET_MACRO(double, DRG)
        GET_SET_MACRO(double, DG2)
        GET_SET_MACRO(double, DR2)
        GET_SET_MACRO(double, DRG2)
        GET_SET_MACRO(double, psf_size_g)
        GET_SET_MACRO(double, psf_size_r)
        GET_SET_MACRO(double, crosstalk)
        GET_SET_MACRO(double, VX)
        GET_SET_MACRO(double, VY)
        GET_SET_MACRO(double, deltax)
        GET_SET_MACRO(double, deltay)
        GET_SET_MACRO(double, pixel_size)
        GET_SET_MACRO(double, frametime)
        GET_SET_MACRO(long, frames)
        GET_SET_MACRO(long, warmup)
        GET_SET_MACRO(bool, dualView)
        GET_SET_MACRO(QString, filename)
        GET_SET_MACRO(int, width)
        GET_SET_MACRO(int, height)
        GET_SET_MACRO(double, brightnessG)
        GET_SET_MACRO(double, brightnessR)
        GET_SET_MACRO(double, brightnessG2)
        GET_SET_MACRO(double, brightnessR2)
        GET_SET_MACRO(double, background)
        GET_SET_MACRO(double, backgroundNoise)
        GET_SET_MACRO(int, walkersG)
        GET_SET_MACRO(int, walkersR)
        GET_SET_MACRO(int, walkersRG)
        GET_SET_MACRO(int, walkersG2)
        GET_SET_MACRO(int, walkersR2)
        GET_SET_MACRO(int, walkersRG2)

        GET_SET_MACRO(bool, FlowEeverywhere)
        GET_SET_MACRO(bool, onlyHalf_DG)
        GET_SET_MACRO(bool, onlyHalf_DR)
        GET_SET_MACRO(bool, onlyHalf_DRG)
        GET_SET_MACRO(bool, onlyHalf_DG2)
        GET_SET_MACRO(bool, onlyHalf_DR2)
        GET_SET_MACRO(bool, onlyHalf_DRG2)

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
        QVector<WalkerData> createWalkers(int count, bool onlyHalfImage);
        void propagateWalkers(QVector<WalkerData>& walkers, double D, bool onlyHalfImage);

        bool canceled;
        double DG;
        double DR;
        double DRG;
        double DG2;
        double DR2;
        double DRG2;

        bool onlyHalf_DG;
        bool onlyHalf_DR;
        bool onlyHalf_DRG;
        bool onlyHalf_DG2;
        bool onlyHalf_DR2;
        bool onlyHalf_DRG2;

        double psf_size_g;
        double psf_size_r;
        double deltax;
        double deltay;
        double crosstalk;
        double VX;
        double VY;
        bool FlowEeverywhere;
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
        double brightnessG2;
        double brightnessR2;
        int walkersG;
        int walkersR;
        int walkersRG;
        int walkersG2;
        int walkersR2;
        int walkersRG2;
        double background;
        double backgroundNoise;


        MTRand rng;
        
};

#endif // QFRDRIMAGINGFCSSIMULATIONTHREAD_H
