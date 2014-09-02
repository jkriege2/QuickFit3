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

#ifndef CAMANDORACQUISITIONTHREAD_H
#define CAMANDORACQUISITIONTHREAD_H


#include <QThread>
#include <QString>
#include <QStringList>
#include <inttypes.h>
#include <QFile>
#include "tinytiffwriter.h"

/*! \brief Thread used for image series acquisitions by QFExtensionCameraAndor
    \ingroup qf3ext_andor

 */
class CamAndorAcquisitionThread : public QThread {
        Q_OBJECT
    public:
        /** Default constructor */
        CamAndorAcquisitionThread(QObject* parent=NULL);
        /** Default destructor */
        virtual ~CamAndorAcquisitionThread();
        /** \brief initialize this thread */
        bool init(int camera, QString filenamePrefix, int fileformat, int64_t numKinetics, int width, int height, double exposureTime, QString log_prefix);
        /** \brief progress of acquisition 0..100 */
        double getProgress() { return progress; };
        /** \brief duration of acquisition in milliseconds */
        double getDurationMilliseconds() { return duration_msecs; };

        double getTemperature() const;

        QStringList getOutputFilenames() { return outputFilenames; }
        QStringList getOutputFilenameTypes() { return outputFilenameTypes; }
    public slots:
        /** \brief cancel the acquisition */
        void cancelAcquisition() { canceled=true; }

    signals:
        void log_error(QString message);
        void log_text(QString message);
        void log_warning(QString message);
    protected:
        int m_camera;
        QString m_filenamePrefix;
        int m_fileformat;
        double m_exposureTime;
        int64_t m_numKinetics;
        int m_width;
        int m_height;
        bool spooling;
        bool canceled;
        double progress;
        double duration_msecs;
        QString m_log_prefix;
        QStringList outputFilenames;
        QStringList outputFilenameTypes;
        float temperature;

        TinyTIFFFile* tiff;
        QFile* raw;

        virtual void run();
    private:
};

#endif // CAMANDORACQUISITIONTHREAD_H
