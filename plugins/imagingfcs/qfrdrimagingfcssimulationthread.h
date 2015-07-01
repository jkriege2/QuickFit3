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

        GET_SET_MACRO(float, DG)
        GET_SET_MACRO(float, DR)
        GET_SET_MACRO(float, DRG)
        GET_SET_MACRO(float, DG2)
        GET_SET_MACRO(float, DR2)
        GET_SET_MACRO(float, DRG2)
        GET_SET_MACRO(float, psf_size_g)
        GET_SET_MACRO(float, psf_size_r)
        GET_SET_MACRO(float, crosstalk)
        GET_SET_MACRO(float, VX)
        GET_SET_MACRO(float, VY)
        GET_SET_MACRO(float, deltax)
        GET_SET_MACRO(float, deltay)
        GET_SET_MACRO(float, pixel_size)
        GET_SET_MACRO(float, frametime)
        GET_SET_MACRO(long, frames)
        GET_SET_MACRO(long, warmup)
        GET_SET_MACRO(bool, dualView)
        GET_SET_MACRO(QString, filename)
        GET_SET_MACRO(int, width)
        GET_SET_MACRO(int, height)
        GET_SET_MACRO(float, brightnessG)
        GET_SET_MACRO(float, brightnessR)
        GET_SET_MACRO(float, brightnessG2)
        GET_SET_MACRO(float, brightnessR2)
        GET_SET_MACRO(float, background)
        GET_SET_MACRO(float, backgroundNoise)
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
            float x;
            float y;
        };
        QVector<WalkerData> createWalkers(int count, bool onlyHalfImage);
        void propagateWalkers(QVector<WalkerData>& walkers, float D, bool onlyHalfImage);

        bool canceled;
        float DG;
        float DR;
        float DRG;
        float DG2;
        float DR2;
        float DRG2;

        bool onlyHalf_DG;
        bool onlyHalf_DR;
        bool onlyHalf_DRG;
        bool onlyHalf_DG2;
        bool onlyHalf_DR2;
        bool onlyHalf_DRG2;

        float psf_size_g;
        float psf_size_r;
        float deltax;
        float deltay;
        float crosstalk;
        float VX;
        float VY;
        bool FlowEeverywhere;
        float pixel_size;
        float frametime;
        long frames;
        long warmup;

        bool dualView;
        QString filename;

        int width;
        int height;
        int currentFrame;
        float brightnessG;
        float brightnessR;
        float brightnessG2;
        float brightnessR2;
        int walkersG;
        int walkersR;
        int walkersRG;
        int walkersG2;
        int walkersR2;
        int walkersRG2;
        float background;
        float backgroundNoise;


        MTRand rng;
        
};

#endif // QFRDRIMAGINGFCSSIMULATIONTHREAD_H
