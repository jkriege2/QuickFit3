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

QString findB040ExperimentDescriptionForData(const QString& filename);

QString findEvalsettingsFileForVideoCorrelatorFile(const QString& vidCorrFile);
QString findOverviewFileForVideoCorrelatorFile(const QString& vidCorrFile);

QString findFileWithExtensionForVideoCorrelatorFile(const QString& vidCorrFile, const QString& newExtension);

int getEvalsettingsDV2Mode(const QString& filename, const QString &datafilename=QString(""), bool *isFCCS=NULL, bool *isACF=NULL);
#endif // QFRDRIMAGINGFCSTOOLS_H
