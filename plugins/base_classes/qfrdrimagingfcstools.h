/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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



#ifndef QFRDRIMAGINGFCSTOOLS_H
#define QFRDRIMAGINGFCSTOOLS_H

#include <QFile>
#include <QSettings>
#include <QString>
#include <QVariant>
#include <QStringList>
#include <QList>
#include <stdint.h>
#include <QToolBar>
#include <QMenu>
#include <QAction>
#include "binarydatatools.h"
#include "qfimagemetadatatool.h"


int stringToDualViewMode(const QString& dv);


/** \brief extract some informations from a configuration file, as created by the B040 SPIM plugin
 *  \ingroup qf3rdrdp_imaging_fcs
 */
void readB040SPIMExperimentConfigFile(QSettings& set, double& frametime, double& baseline_offset, QString& backgroundfile, int& image_width, int& image_height, bool& hasPixel, double& pixel_width, double& pixel_height, int &dualViewMode);



/** \brief extract some informations from a configuration file, as created by the B040 SPIM plugin
 *  \ingroup qf3rdrdp_imaging_fcs
 */
void readB040SPIMExperimentConfigFileAssociatedFiles(QSettings& set, QStringList& files, QStringList& description,  QStringList& type);

/** \brief scans the given config file for associated files and appends them, together with an appropriate categorization, to files and file_types
 *         The category in file_types may be directly used by QFRDRImagingFCSData in its associated files list!
 *  \ingroup qf3rdrdp_imaging_fcs
 */
void appendCategorizedFilesFromB040SPIMConfig(QSettings& settings, QStringList& files, QStringList& files_types, QStringList &files_descriptions);



QString findEvalsettingsFileForVideoCorrelatorFile(const QString& vidCorrFile);
QString findOverviewFileForVideoCorrelatorFile(const QString& vidCorrFile);

QString findFileWithExtensionForVideoCorrelatorFile(const QString& vidCorrFile, const QString& newExtension);

int getEvalsettingsDV2Mode(const QString& filename, const QString &datafilename=QString(""), bool *isFCCS=NULL, bool *isACF=NULL, QString *inputFilename=NULL, QString *group=NULL);

bool readEvalSettingsFile(const QString &evalFilename, bool isDCCF, QMap<QString, QVariant> &initParams, QStringList &paramsReadonly, int &width, int &height, QStringList &files, QStringList &files_types, QStringList &files_descriptions, QString &filename_settings, QString& filename_acquisition, QString filename_overview, QString& filename_overviewstd, QString& filename_background, QString& filename_backgroundstddev, QString& role, int dccfid, bool& overviewReal);
void imFCSFilenameHeuristics(const QString &filename, QString* evalFilename, bool *isCross, bool *isDCCF, int* dccfid, bool* isJanBFile, QString* role);
#endif // QFRDRIMAGINGFCSTOOLS_H
