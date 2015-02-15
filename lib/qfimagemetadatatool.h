/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2014-09-25 17:54:41 +0200 (Do, 25 Sep 2014) $  (revision $Rev: 3506 $)

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

#ifndef QFIMAGEMETADATATOOL_H
#define QFIMAGEMETADATATOOL_H

#include <QString>
#include <QMap>
#include <QVariant>
#include <QtXml>
#include "lib_imexport.h"

/*! \defgroup qf3lib_tools_imagemetadata Image Series meta-data
    \ingroup qf3lib_tools

    Functions in this group allow to extract image (serires) Metadata from a text- image description (e.g. in the OME format, or what ImageJ writes into TIFF-files).

    All data are read and then stored in a \c QMap<QString,QVariant> under an expressive id (key).

    These functions write some default properties:
      - \c FRAMES = number of frames
      - \c PIXEL_WIDTH = width of a pixel in nanometers
      - \c PIXEL_HEIGHT = height of a pixel in nanometers
      - \c FRAMETIME = frame repetition time (1/framerate) in seconds
      - \c DELTAZ / \c STEPSIZE = z-stack step-size in nanometers
    .
    All other data in the metadata-string are stored in custom ids.
 */

/*! \brief stores the contents of an OME sub-tree under \a de into \a data (using the given \a prefix and removing the attributes/tags which are contained in \a exclude)
    \ingroup qf3lib_tools_imagemetadata

*/
void QFLIB_EXPORT qfimdtStoreOMEMetaDataTree(QMap<QString, QVariant> &data, QDomElement de, const QString &prefix=QString(), const QStringList &exclude=QStringList());
/*! \brief tries to extract OME metadata from the given \a input and writes it to \a data
    \ingroup qf3lib_tools_imagemetadata

    \see <a href="http://jcb.rupress.org/content/189/5/777/F2.large.jpg">http://jcb.rupress.org/content/189/5/777/F2.large.jpg</a>
    \see <a href="http://jcb.rupress.org/content/189/5/777.full">http://jcb.rupress.org/content/189/5/777.full</a>
    \see <a href="http://www.openmicroscopy.org/site/support/ome-model/">http://www.openmicroscopy.org/site/support/ome-model/</a>
*/
bool QFLIB_EXPORT qfimdtGetOMEMetaData(QMap<QString, QVariant>& data, const QByteArray &input);
/*! \brief tries to extract ImageJ metadata from the given \a input and writes it to \a data
    \ingroup qf3lib_tools_imagemetadata

\verbatim
ImageJ=0.46b
images=1000
\endverbatim
*/
bool QFLIB_EXPORT qfimdtGetImageJMetaData(QMap<QString, QVariant>& data, const QByteArray &input);
/*! \brief tries to extract TinyTIFFWriter metadata from the given \a input and writes it to \a data
    \ingroup qf3lib_tools_imagemetadata

\verbatim
TinyTIFFWriter_version=1.1
images=6
pixel_width=100.000000
pixel_height=200.000000
deltaz=0.000100
frametime=300
\endverbatim
*/
bool QFLIB_EXPORT qfimdtGetTinyTIFFMetaData(QMap<QString, QVariant>& data, const QByteArray &input);

/*! \brief tries to extract QuickFit 3.0 metadata for the given \a input and writes it to \a data. This extracts metadat from e.g. <tt>.configuration.ini</tt> files, which link to this image
    \ingroup qf3lib_tools_imagemetadata

*/
bool QFLIB_EXPORT qfimdtGetQuickFitMetaData(QMap<QString, QVariant>& data, const QString &filename, QString* configfile=NULL, QString* filedescription=NULL, bool filenameIsINI=false);

/*! \brief tries to find a metadata file (e.g. <tt>XXX.configuration.ini</tt>), which contains data for the given image file
    \ingroup qf3lib_tools_imagemetadata

    These files are created e.g. by the B040SPIM plugin. It has to contain a section [files], which has to contain an entry for the
    given filename. This function also returns the assigned file description (from the file) in \a description.

    Here is an example for such a configuration file:
\verbatim
[acquisition]
accumulation_time=0.00056
binning_horizontal=1
binning_vertical=1
bit_depth=14
camera_manufacturer=Andor
camera_sensor=DU860_BV
camera_serial_number=5322
cooler_enabled=true
duration_milliseconds=11200
emgain=5
emgain_enabled=true
exposure_time=0.0005
image_height=4
image_width=20
images=50000
pixel_height=0.4
pixel_units=arbitrary
pixel_width=0.4
preamplifier_gain=4.5
roi_xend=74
roi_xstart=55
roi_yend=58
roi_ystart=55
camera_pixel_width=24
camera_pixel_height=24
camera_model=DU860_BV
sensor_model=DU860_BV
magnification=60

[setup]
filterchangers\\detection\\filter=1
filters\\detection_filterchanger\\manufacturer=Semrock
filters\\detection_filterchanger\\name=BrightLine 525/50 (F37-516)
filters\\detection_filterchanger\\type=band pass
laser1\\line1\\enabled=true
laser1\\line1\\measured_power=5
laser1\\line1\\name=line #1: 491 nm
laser1\\line1\\set_power=5
laser1\\line1\\unit=mW
laser1\\shutter=true
main_illumination\\shutter=true
objectives\\detection\\NA=1
objectives\\detection\\magnification=60
objectives\\detection\\manufacturer=Nikon
objectives\\detection\\name=Nikon Apo 60x/1.0W
objectives\\tube_lens1\\NA=0.07
objectives\\tube_lens1\\magnification=1
objectives\\tube_lens1\\manufacturer=Nikon
objectives\\tube_lens1\\name="1x tubelens, f=200mm"

[experiment]
experimenter=JanK
labbook=54
sample="sample description"
start_time=2012-06-19T21:29:23
title=test with 100nm YG beads

[files]
count=3
name0=beads_no9_0.86nM_5mW_000_overview.tif
type0=TIFF16
description0=overview before acquisition  with preview settings
name1=beads_no9_0.86nM_5mW_000.tif
type1=TIFF16
description1=Andor camera acquisition image series
name2=beads_no9_0.86nM_5mW_000.measured.dat
type2=CSV
description2=measureable properties of setup
\endverbatim

*/
QString QFLIB_EXPORT findB040ExperimentDescriptionForData(const QString& filename, QString* description=NULL);

/*! \brief returns an ImageJ metadata-block
    \ingroup qf3lib_tools_imagemetadata

\verbatim
ImageJ=0.46b
images=1000
\endverbatim
*/
QByteArray QFLIB_EXPORT qfimdtBuildImageJMetaData(int frames=0, double deltaX=1, double deltaY=1, double deltaZ=1, const QString& axisunit=QString("pixel"), const QString &comment=QString());

#endif // QFIMAGEMETADATATOOL_H
