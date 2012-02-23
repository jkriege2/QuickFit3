#ifndef QFRDRIMAGINGFCSTOOLS_H
#define QFRDRIMAGINGFCSTOOLS_H

#include <QFile>
#include <QSettings>
#include <QString>
#include <QVariant>
#include <QStringList>
#include <QList>
#include <stdint.h>

/** \brief extract some informations from a configuration file, as created by the B040 SPIM plugin
 *  \ingroup qf3rdrdp_imaging_fcs
 */
void readB040SPIMExperimentConfigFile(QSettings& set, double& frametime, double& baseline_offset, QString& backgroundfile, int& image_width, int& image_height);

/** \brief extract some informations from a configuration file, as created by the B040 SPIM plugin
 *  \ingroup qf3rdrdp_imaging_fcs
 */
void readB040SPIMExperimentConfigFileAssociatedFiles(QSettings& set, QStringList& files, QStringList& description,  QStringList& type);

/** \brief scans the given config file for associated files and appends them, together with an appropriate categorization, to files and file_types
 *         The category in file_types may be directly used by QFRDRImagingFCSData in its associated files list!
 *  \ingroup qf3rdrdp_imaging_fcs
 */
void appendCategorizedFilesFromB040SPIMConfig(QSettings& settings, QStringList& files, QStringList& files_types, QStringList &files_descriptions);

/** \brief read a uint32_t from a binary file
 *  \ingroup qf3rdrdp_imaging_fcs
 */
uint32_t binfileReadUint32(QFile& file);

/** \brief write a uint32_t to a binary file
 *  \ingroup qf3rdrdp_imaging_fcs
 */
void binfileWriteUint32(QFile& file, uint32_t data);

/** \brief read a double from a binary file
 *  \ingroup qf3rdrdp_imaging_fcs
 */
double binfileReadDouble(QFile& file);

/** \brief write a double to a binary file
 *  \ingroup qf3rdrdp_imaging_fcs
 */
void binfileWriteDouble(QFile& file, double data);

/** \brief write an array of dataN doubles to a binary file
 *  \ingroup qf3rdrdp_imaging_fcs
 */
void binfileWriteDoubleArray(QFile& file, const double* data, uint32_t dataN);

/** \brief write an array of dataN doubles to a binary file and subtract 1.0 from every entry
 *  \ingroup qf3rdrdp_imaging_fcs
 */
void binfileWriteDoubleArrayMinus1(QFile& file, const double* data, uint32_t dataN);

/** \brief read an array of dataN doubles from a binary file
 *  \ingroup qf3rdrdp_imaging_fcs
 */
void binfileReadDoubleArray(QFile& file, double* data, uint32_t dataN);

#endif // QFRDRIMAGINGFCSTOOLS_H
