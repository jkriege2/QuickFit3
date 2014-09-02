/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

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

#ifndef JKQTPLatexEngineAdapter_H
#define JKQTPLatexEngineAdapter_H

#include "QTeXEngine/src/QTeXEngine.h"
#include <QString>
#include <QStringList>
#include "jkqtpbaseplotter.h"

class JKQTPLatexEngineAdapter : public JKQtBasePlotter::JKQTPPaintDeviceAdapter
{
    public:
        JKQTPLatexEngineAdapter(bool docMode=false, bool colorMode=true, QTeXPaintDevice::OutputMode outputMode=QTeXPaintDevice::Tikz, bool useLatexParser=true);

        virtual QString getFilter() const;
        virtual QString getFormatName() const;
        virtual QString getFormatID() const;
        virtual QStringList getFileExtension() const;
        virtual bool getSetAbsolutePaperSize() const;
        virtual double getPrintSizeXInMM() const ;
        virtual double getPrintSizeYInMM() const ;
        virtual bool isPrinter() const;
        virtual bool useLatexParser() const;
        virtual QPaintDevice* createPaintdevice(const QString& filename, int widthPix, int heightPix) const;

    protected:
        bool docMode;
        bool colorMode;
        bool m_useLatexParser;
        QTeXPaintDevice::OutputMode outputMode;
        QString id;
};

#endif // JKQTPLatexEngineAdapter_H
