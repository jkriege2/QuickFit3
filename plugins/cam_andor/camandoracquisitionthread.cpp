/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

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

#include "camandoracquisitionthread.h"

#include <iostream>
#include <unistd.h>
#include <time.h>
#include <cmath>
#include <QtCore>

#include "andortools.h"





#define CHECK(s, msg) \
{ \
    unsigned int error=s; \
    if (error!=DRV_SUCCESS) { \
        qDebug()<<QString(msg)+tr("\n%3    error code was: %1 [%2]\n%3    instruction was: %4").arg(error).arg(andorErrorToString(error)).arg(m_log_prefix).arg(#s); \
        emit log_error(QString(msg)+tr("\n%3    error code was: %1 [%2]\n%3    instruction was: %4").arg(error).arg(andorErrorToString(error)).arg(m_log_prefix).arg(#s)); \
        return false; \
    } \
}

#define CHECK_NO_RETURN(s, msg) \
{ \
    unsigned int error=s; \
    if (error!=DRV_SUCCESS) { \
        qDebug()<<QString(msg)+tr("\n%3    error code was: %1 [%2]\n%3    instruction was: %4").arg(error).arg(andorErrorToString(error)).arg(m_log_prefix).arg(#s); \
        emit log_warning(QString(msg)+tr("\n%3    error code was: %1 [%2]\n%3    instruction was: %4").arg(error).arg(andorErrorToString(error)).arg(m_log_prefix).arg(#s)); \
    } \
}


#define CHECK_NO_RETURN_OK(ok_var, s, msg) \
{ \
    unsigned int error=s; \
    if (error!=DRV_SUCCESS) { \
        ok_var=false; \
        qDebug()<<QString(msg)+tr("\n%3    error code was: %1 [%2]\n%3    instruction was: %4").arg(error).arg(andorErrorToString(error)).arg(m_log_prefix).arg(#s); \
        emit log_warning(QString(msg)+tr("\n%3    error code was: %1 [%2]\n%3    instruction was: %4").arg(error).arg(andorErrorToString(error)).arg(m_log_prefix).arg(#s)); \
    } \
}



CamAndorAcquisitionThread::CamAndorAcquisitionThread(QObject* parent):
    QThread(parent)
{
    setPriority(QThread::HighestPriority);
    duration_msecs=0;
}

CamAndorAcquisitionThread::~CamAndorAcquisitionThread()
{
    //dtor
}


bool CamAndorAcquisitionThread::init(int camera, QString filenamePrefix, int fileformat, int64_t numKinetics, int width, int height, double exposureTime, QString log_prefix) {
    m_camera=camera;
    m_filenamePrefix=filenamePrefix;
    m_fileformat=fileformat;
    m_log_prefix=log_prefix;
    m_width=width;
    m_height=height;
    m_exposureTime=exposureTime;
    m_numKinetics=numKinetics;
    spooling=false;
    progress=0;
    canceled=false;
    outputFilenames.clear();
    outputFilenameTypes.clear();
    if (selectCamera(camera)) {

        // set spooling for those formats with spooling ... depending on the value of the member spooling
        // the methid run() will execute different code!
        if (fileformat==1) { // TIFF (spooling)
            CHECK(SetSpool(1, 7, filenamePrefix.toAscii().data(), 100), tr("error while enabling spooling mode"));
            outputFilenameTypes.append("TIFF16");
            outputFilenames.append(filenamePrefix+".tif");
            spooling = true;
        } else if (fileformat==2) { // Andor SIF (spooling)
            CHECK(SetSpool(1, 6, filenamePrefix.toAscii().data(), 100), tr("error while enabling spooling mode"));
            outputFilenameTypes.append("AndorSIF");
            outputFilenames.append(filenamePrefix+".sif");
            spooling = true;
        } else if (fileformat==3) { // FITS (spooling)
            CHECK(SetSpool(1, 5, filenamePrefix.toAscii().data(), 100), tr("error while enabling spooling mode"));
            outputFilenameTypes.append("FITS");
            outputFilenames.append(filenamePrefix+".fits");
            spooling = true;
        } else { // make sure spooling is switched off
            CHECK(SetSpool(0, 7, filenamePrefix.toAscii().data(), 100), tr("error while disabling spooling mode"));
            if (fileformat==0) { // TIFF
                outputFilenameTypes.append("TIFF16");
                QString filename=filenamePrefix+".tif";
                outputFilenames.append(filename);
                tiff=TinyTIFFWriter_open(filename.toAscii().data(), 16, m_width, m_height);
                if (!tiff) {
                    emit log_error(tr("\n%1    could not open TIFF file '%2'").arg(m_log_prefix).arg(filename));
                    return false;
                }
            } else if (fileformat==4) { // RAW
                QString filename=filenamePrefix+".raw";
                outputFilenameTypes.append("RAW");
                outputFilenames.append(filename);
                raw=new QFile(filename);
                if (!raw->open(QIODevice::WriteOnly)) {
                    emit log_error(tr("\n%1    could not open raw file '%2', \n%1       reason: %3").arg(m_log_prefix).arg(filename).arg(raw->errorString()));
                    delete raw;
                    return false;
                }
            }
        }

        return true;
    } else {
        return false;
    }



}

void CamAndorAcquisitionThread::run() {

    GetTemperatureF(&temperature);

    bool ok=true;
    selectCamera(m_camera);
    CHECK_NO_RETURN_OK(ok, StartAcquisition(), tr("error while starting acquisition"));

    if (ok) {
        if (spooling) {
            /* SPOOLING MODE:

               Here we just have to wait and ask for the progress here and there
               ... also we have to check whether the acquisition has been canceled

               The main (waiting) loop is stalled using the wait() command for max. 1ms in every cycle!

            */
            setPriority(QThread::NormalPriority);
            int status=DRV_ACQUIRING;
            while ((!canceled) && ok && (status==DRV_ACQUIRING)) {
                selectCamera(m_camera);
                CHECK_NO_RETURN_OK(ok, GetStatus(&status), tr("error while reading status"));
            #ifdef __WINDOWS__
                long p=0;
            #else
                int p=0;
            #endif
                selectCamera(m_camera);
                CHECK_NO_RETURN_OK(ok, GetSpoolProgress(&p), tr("error acquiring spool progress"));
                progress=100.0*(double)p/(double)m_numKinetics;
                GetTemperatureF(&temperature);
                wait(qMin(1, (int)round(m_numKinetics/1000*m_exposureTime)));
            }
        } else {
            /* NON-SPOOLING MODE:

               Here we have to loop until the acquisition is complete. In the loop we have to check whether new
               images are available and if so, write them to the filesystem

            */
            setPriority(QThread::HighestPriority);
            int status=DRV_ACQUIRING;
            int64_t imageCount=0;
            uint16_t* imageBuffer=(uint16_t*)calloc(m_width*m_height, sizeof(uint16_t));
            QTime duration;
            duration.start();
            QTime ttimer;
            ttimer.start();
            QTime lastWritten;
            lastWritten.start();
            bool timedout=false;
            int lastWrittenTimeout=20000;
            int maxMissingFrames=50;
            while ((!canceled) && ok && (status==DRV_ACQUIRING) && !timedout) {
                selectCamera(m_camera);
                CHECK_NO_RETURN_OK(ok, GetStatus(&status), tr("error while reading status"));
                if (status==DRV_ACQUIRING) duration_msecs=duration.elapsed();
                while (GetOldestImage16(imageBuffer, m_width*m_height)==DRV_SUCCESS) {
                    if (m_fileformat==0) {
                        TinyTIFFWriter_writeImage(tiff, imageBuffer);
                    } else if (m_fileformat==4) {
                        raw->write((char*)imageBuffer, m_width*m_height*sizeof(uint16_t));
                    }
                    imageCount++;
                    lastWritten.start();
                }
                if (m_numKinetics>1000) {
                    if (imageCount%(m_numKinetics/1000)==0) progress=100.0*(double)imageCount/(double)m_numKinetics;
                } else {
                    progress=100.0*(double)imageCount/(double)m_numKinetics;
                }
                if (ttimer.elapsed()>500)  {
                    GetTemperatureF(&temperature);
                    ttimer.start();
                }
                timedout= (lastWritten.elapsed()>lastWrittenTimeout) && ((m_numKinetics-imageCount)<maxMissingFrames); //false
            }

            if (timedout) {

                CHECK_NO_RETURN_OK(ok, AbortAcquisition(), tr("error while aborting acquisition"));
                QString msg=tr("\n%1: tinyTIFF acquisition timed out ... acquired %2 of %3 frames with %4s timeout").arg(m_log_prefix).arg(imageCount).arg(m_numKinetics).arg(double(lastWrittenTimeout)/1000.0);
                qDebug()<<msg;
                //emit log_warning(msg);
                emit log_error(msg);
            }

            if (m_fileformat==0) {
                TinyTIFFWriter_close(tiff);
            } else if (m_fileformat==4) {
                raw->close();
                delete raw;
                raw=NULL;
            }

            free(imageBuffer);

        }

        if (canceled) CHECK_NO_RETURN_OK(ok, AbortAcquisition(), tr("error while aborting acquisition"));
    }

}

double CamAndorAcquisitionThread::getTemperature() const {
    return temperature;
}

