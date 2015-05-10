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


#ifndef JKQTPCairoEngineAdapter_H
#define JKQTPCairoEngineAdapter_H

#include <QString>
#include <QStringList>
#include "jkqtpbaseplotter.h"

class JKQTPCairoEngineAdapter : public JKQtBasePlotter::JKQTPPaintDeviceAdapter
{
    public:
        enum Format {
            formatPDF14,
            formatPDF15,
            formatPS2,
            formatPS3,
            formatEPS2,
            formatEPS3,
            formatSVG11,
			formatSVG12
        };

        JKQTPCairoEngineAdapter(Format format, bool exportText);

        virtual QString getFilter() const;
        virtual QString getFormatName() const;
        virtual QString getFormatID() const;
        virtual QStringList getFileExtension() const;
        virtual bool getSetAbsolutePaperSize() const;
        virtual double getPrintSizeXInMM() const ;
        virtual double getPrintSizeYInMM() const ;
        virtual bool isPrinter() const;
        virtual QPaintDevice* createPaintdevice(const QString& filename, int widthPix, int heightPix) const;
        virtual QPaintDevice* createPaintdeviceMM(const QString& filename, double widthMM, double heightMM) const;

    protected:
         Format format;
         bool exportText;

};

#endif // JKQTPCairoEngineAdapter_H
