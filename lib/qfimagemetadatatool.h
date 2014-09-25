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

#endif // QFIMAGEMETADATATOOL_H
