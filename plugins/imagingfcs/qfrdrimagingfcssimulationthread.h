/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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

#include <QApplication>
#include <QElapsedTimer>
#include <QFile>
#include <QSettings>
#include <QString>
#include <QVector>
#include <QFileInfo>
#include <QDebug>
#include <QThread>
#include <QStringList>
#include "cpptools.h"
#include "tinytiffwriter.h"
#include "MersenneTwister.h"
#include "qftools.h"
#include "datatools.h"


#define SIMENV_NORMAL 0
#define SIMENV_GRIDBOUNDARIES 1
#define SIMENV_TRAPS 2

#define BOUNDARY_PERIODIC 0
#define BOUNDARY_REINTRODUCE 1

#define PSF_GAUSS 0
#define PSF_PIXELGAUSS 1

class QFRDRImagingFCSSimulationThread : public QThread
{
        Q_OBJECT
    public:
        explicit QFRDRImagingFCSSimulationThread(QObject *parent = 0);
        int getCurrentFrame() const;

        GET_SET_MACRO(float, DG3)
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
        GET_SET_MACRO(float, sim_timestep)
        GET_SET_MACRO(long, frames)
        GET_SET_MACRO(long, warmup)
        GET_SET_MACRO(bool, dualView)
        GET_SET_MACRO(QString, filename)
        GET_SET_MACRO(int, width)
        GET_SET_MACRO(int, height)
        GET_SET_MACRO(float, brightnessG)
        GET_SET_MACRO(float, brightnessG3)
        GET_SET_MACRO(float, brightnessR)
        GET_SET_MACRO(float, brightnessG2)
        GET_SET_MACRO(float, brightnessR2)
        GET_SET_MACRO(float, background)
        GET_SET_MACRO(float, backgroundNoise)
        GET_SET_MACRO(int, walkersG)
        GET_SET_MACRO(int, walkersR)
        GET_SET_MACRO(int, walkersRG)
        GET_SET_MACRO(int, walkersG2)
        GET_SET_MACRO(int, walkersG3)
        GET_SET_MACRO(int, walkersR2)
        GET_SET_MACRO(int, walkersRG2)

        GET_SET_MACRO(bool, FlowEeverywhere)
        GET_SET_MACRO(bool, onlyHalf_DG)
        GET_SET_MACRO(bool, onlyHalf_DG3)
        GET_SET_MACRO(bool, onlyHalf_DR)
        GET_SET_MACRO(bool, onlyHalf_DRG)
        GET_SET_MACRO(bool, onlyHalf_DG2)
        GET_SET_MACRO(bool, onlyHalf_DR2)
        GET_SET_MACRO(bool, onlyHalf_DRG2)
        GET_SET_MACRO(int, boundaryConditions)

        GET_SET_MACRO(float, boundaryGridSpacing)
        GET_SET_MACRO(float, boundaryGridJumpProbability)
        GET_SET_MACRO(bool, boundaryGridOnlyRight)
        GET_SET_MACRO(int, environmentMode)
        GET_SET_MACRO(float, simspace_sizeinc)

        GET_SET_MACRO(bool, saveTrajectores)
        GET_SET_MACRO(int, maxTrajectores)
        GET_SET_MACRO(int, trajectoresMaxSteps)
        GET_SET_MACRO(bool, saveMSD)
        GET_SET_MACRO(int, msdMaxSteps)
        GET_MACRO(QStringList, msdNames)
        GET_MACRO(QStringList, trajNames)

        GET_SET_MACRO(float, trapGridSpacing)
        GET_SET_MACRO(float, trapDiameter)
        GET_SET_MACRO(float, trapSlowdown)
        GET_SET_MACRO(bool, trapOnlyRight)
        GET_SET_MACRO(int, psf_type)

    public slots:
        void cancel();
        void waitForFinish();
    signals:
        void progress(int currentFrame);
        void statusMessage(const QString message);
    protected:
        virtual void run();

        inline float psf_gauss(float x, float y, float wxy) {
            if (qfSqr(x)+qfSqr(y)>5.0*qfSqr(wxy)) return 0;
            return exp(-2.0*(qfSqr(x)+qfSqr(y))/qfSqr(wxy));
        }
        inline float psf_pixelgauss(float x, float y, float wxy) {
            if (qfSqr(x)+qfSqr(y)>5.0*qfSqr(wxy)) return 0;
            return (erf((pixel_size-2.0*x)/wxy/M_SQRT2)+erf((pixel_size+2.0*x)/wxy/M_SQRT2))*(erf((pixel_size-2.0*y)/wxy/M_SQRT2)+erf((pixel_size+2.0*y)/wxy/M_SQRT2))/qfSqr(2.0*erf(pixel_size/wxy/M_SQRT2));
        }

        struct WalkerData {
            float x;
            float y;
        };
        QVector<WalkerData> createWalkers(int count, bool onlyHalfImage);
        void propagateWalkers(QVector<WalkerData>& walkers, float D, bool onlyHalfImage, QList<QVector<QPair<float,float> > >* msds=NULL, QList<QVector<QPair<float, float> > > *traj=NULL);
        void calcMSD(QList<QVector<double> > &msdout, const QList<QVector<QPair<float, float> > > &wg_msd, const QVector<uint64_t> &tau);
        void saveTraj(QList<QVector<double> > &msdout, const QList<QVector<QPair<float, float> > > &wg_msd, int& tmax, QStringList& columnNames, const QString &wname, QVector<double>* jumpDistX=NULL, QVector<double>* jumpDistN=NULL, int jumpDistBins=0, double jumpDistMin=0, double jumpDistMax=0);

        bool canceled;
        float DG;
        float DR;
        float DRG;
        float DG2;
        float DG3;
        float DR2;
        float DRG2;

        bool onlyHalf_DG;
        bool onlyHalf_DG3;
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
        float simspace_sizeinc;
        float frametime;
        float sim_timestep;
        uint64_t frames;
        uint64_t warmup;
        int boundaryConditions;

        bool dualView;
        QString filename;

        bool saveMSD;
        int msdMaxSteps;

        bool saveTrajectores;
        int maxTrajectores;
        int trajectoresMaxSteps;

        int width;
        int height;
        uint64_t currentFrame;
        float brightnessG;
        float brightnessR;
        float brightnessG2;
        float brightnessG3;
        float brightnessR2;
        int walkersG;
        int walkersR;
        int walkersRG;
        int walkersG2;
        int walkersG3;
        int walkersR2;
        int walkersRG2;
        float background;
        float backgroundNoise;

        float boundaryGridSpacing;
        float boundaryGridJumpProbability;
        bool boundaryGridOnlyRight;
        int environmentMode;

        float trapGridSpacing;
        float trapDiameter;
        float trapSlowdown;
        bool trapOnlyRight;

        int psf_type;

        QStringList msdNames;
        QStringList trajNames;
        MTRand rng;

        //QVector<bool> trapGrid;
        
};

#endif // QFRDRIMAGINGFCSSIMULATIONTHREAD_H
