/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QFIMPORTER_H
#define QFIMPORTER_H

#include <QString>
#include <QDateTime>
#include <QMap>
#include <QVariant>
#include <QFileInfo>
#include "lib_imexport.h"

/*! \brief interface (file) reader reader classes (importer)
    \ingroup qf3importerplugins

*/
class QFLIB_EXPORT QFImporter {
    public:
        struct QFLIB_EXPORT FileInfo {
            QString comment;
            QDateTime date;
            QString filename;
            QMap<QString, QVariant> properties;
            FileInfo() {
                comment="";
                date=QDateTime::currentDateTime();
                properties.clear();
            }
            inline void init(const QString& filename) {
                date=QFileInfo(filename).created();
                this->filename=filename;
            }
        };

        virtual ~QFImporter() {}

        /** \brief return a description of the last error that occured */
        QString lastError() const {
            return err;
        }
        /** \brief return a filter string for the file format */
        virtual QString filter() const =0;
        /** \brief return a name string for the file format */
        virtual QString formatName() const =0;

        const FileInfo& getFileInfo() const {
            return fileinfo;
        }

    protected:

        void setLastError(QString err) {
            this->err=err;
        }

    private:
        QString err;
    protected:
        FileInfo fileinfo;
};


#endif // QFIMPORTER_H

