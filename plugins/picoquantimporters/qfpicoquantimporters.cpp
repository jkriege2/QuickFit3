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

#include "qfpicoquantimporters.h"
#include "qftcspcreaderpicoquant.h"
#include "qftcspcreaderpicoquantpt3.h"
#include "qftcspcreaderpicoquantptu.h"

QStringList QFPicoQuantImporters::getIDs() const {
    QStringList res;
    res<<"tcspcimporter_picoquant";
    res<<"tcspcimporter_picoquantpt3";
    res<<"tcspcimporter_picoquantptu";
    return res;
}

QFImporter* QFPicoQuantImporters::createImporter(QString id) const  {
    if (id=="tcspcimporter_picoquant") {
        return new QFTCSPCReaderPicoquant();
    } 
    if (id=="tcspcimporter_picoquantpt3") {
        return new QFTCSPCReaderPicoquantPT3();
    }
    if (id=="tcspcimporter_picoquantptu") {
        return new QFTCSPCReaderPicoquantPTU();
    }
    return NULL;
}

Q_EXPORT_PLUGIN2(importers_picoquant, QFPicoQuantImporters)
