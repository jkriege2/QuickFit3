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


#ifndef QCAIROPAINTDEVICE_H
#define QCAIROPAINTDEVICE_H

#include <QPaintDevice>
#include <QString>
#include <QSize>

class QCairoPaintEngine;

class QCairoPaintDevice : public QPaintDevice
{
    public:
        enum CairoFileType {
            cftPDF14=0,
            cftPDF15=1,
            cftPDF=cftPDF15,
            cftPS2=10,
            cftPS3=11,
            cftPS=cftPS3,
            cftEPS2=20,
            cftEPS3=21,
            cftEPS=cftEPS3,
            cftSVG11=30,
            cftSVG12=31,
            cftSVG=cftSVG12,
        };

        explicit QCairoPaintDevice(const QSizeF& size_millimeters, const QString& fileName, CairoFileType filetype, bool exportText=false);
        virtual ~QCairoPaintDevice();

        virtual QPaintEngine * paintEngine () const;

        CairoFileType getFileType() const;
        QString getFileName() const;
        QSizeF getFileSize() const;
        QSizeF getFileSizeMM() const;
        bool getExportText() const;

    protected:
        virtual int metric (PaintDeviceMetric metric) const;

    protected:
        QString m_fileName;
        CairoFileType m_filetype;
        QSizeF m_size;
        QCairoPaintEngine* engine;
        bool m_exportText;
};



#endif // QCAIROPAINTDEVICE_H
