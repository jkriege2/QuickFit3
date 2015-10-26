/*
Copyright (c) 2014
	
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


#include "qfqtimageimporter.h"

QStringList QFQtImageImporter::getIDs() const {
    QStringList res;
    res<<"importer_qt_png"<<"importer_qt_jpeg"<<"importer_qt_gif"<<"importer_qt_pbm"<<"importer_qt_pgm"<<"importer_qt_ppm";
    return res;
}

QFImporter* QFQtImageImporter::createImporter(QString id) const  {
    if (id=="importer_qt_png") {
        return new QFQtImageImporterPNG();
    } else if (id=="importer_qt_jpeg") {
        return new QFQtImageImporterJPEG();
    } else if (id=="importer_qt_gif") {
        return new QFQtImageImporterGIF();
    } else if (id=="importer_qt_pbm") {
        return new QFQtImageImporterPBM();
    } else if (id=="importer_qt_pgm") {
        return new QFQtImageImporterPGM();
    } else if (id=="importer_qt_ppm") {
        return new QFQtImageImporterPPM();
    }

    return NULL;
}

Q_EXPORT_PLUGIN2(importers_qtimages, QFQtImageImporter)
