/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    

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

#ifndef QFESPIMB040FILENAMETOOL_H
#define QFESPIMB040FILENAMETOOL_H

#include <QMap>
#include <QString>
#include "qenhancedlineedit.h"
#include <QWidget>
#include <QDir>
#include <QFileInfo>

class QFESPIMB040OpticsSetupBase; //forward
class QFESPIMB040ExperimentDescription; //forward
class QFESPIMB040AcquisitionDescription; //forward

class QFESPIMB040FilenameTool {
    public:
        QFESPIMB040FilenameTool();

        QString transformFilename(const QString& filename) const;
        void setReplaceValues(const QMap<QString, QString>& replaceValues);
        void setReplaceValue(const QString& key, const QString& value);
        void bindLineEdit(QEnhancedLineEdit* edit);
        void setGlobalReplaces(QFESPIMB040OpticsSetupBase* setup, QFESPIMB040ExperimentDescription* exp, QFESPIMB040AcquisitionDescription* acq);

    protected:
        QMap<QString, QString> replaceValues;
        QList<QEnhancedLineEdit*> edits;
        void fillLineEdits();
};

#endif // QFESPIMB040FILENAMETOOL_H
