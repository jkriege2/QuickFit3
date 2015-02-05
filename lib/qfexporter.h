/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2014-09-26 12:42:55 +0200 (Fr, 26 Sep 2014) $  (revision $Rev: 3509 $)

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

#ifndef QFEXPORTER_H
#define QFEXPORTER_H


#include <QString>
#include <QDateTime>
#include <QMap>
#include <QVariant>
#include <QFileInfo>
#include "lib_imexport.h"

/*! \brief interface (file) write classes (exporter)
    \ingroup qf3exporterplugins

*/
class QFLIB_EXPORT QFExporter {
    public:

        virtual ~QFExporter() {}

        /** \brief return a description of the last error that occured */
        QString lastError() const {
            return err;
        }
        /** \brief return a filter string for the file format */
        virtual QString filter() const =0;
        /** \brief return a name string for the file format */
        virtual QString formatName() const =0;

        inline void setFileComment(const QString& d) {
            comment=d;
        }

        inline QString getFileComment() const {
            return comment;
        }

    protected:

        void setLastError(QString err) {
            this->err=err;
        }

    protected:
        QString err;
        QString comment;
};

#endif // QFEXPORTER_H
