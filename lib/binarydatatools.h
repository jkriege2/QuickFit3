#ifndef BINARYDATATOOLS_H
#define BINARYDATATOOLS_H


#include <QString>
#include <QVector>
#include "lib_imexport.h"

/*! \brief decodes a string which contains an array of doubles (64-bit), which is encoded in base64
    \ingroup qf3lib_mathtools

    \note The endianess of the data should be LITTLE ENDIAN. The method doubleArrayToString_base64() ensures this.
 */
QFLIB_EXPORT QVector<double> stringToDoubleArray_base64(const QString& data);
/*! \brief decodes a string which contains an array of doubles (64-bit), which is encoded in hex
    \ingroup qf3lib_mathtools

    \note The endianess of the data should be LITTLE ENDIAN. The method doubleArrayToString_hex() ensures this.
 */
QFLIB_EXPORT QVector<double> stringToDoubleArray_hex(const QString& data);


/*! \brief encodes an array of doubles (64-bit), into a base64-encoded string
    \ingroup qf3lib_mathtools

    \note This method ensures that the doubles are converted to LITTLE ENDIAN before encoding them
 */
QFLIB_EXPORT QByteArray doubleArrayToString_base64(const QVector<double>& data);
/*! \brief encodes an array of doubles (64-bit), into a hex-encoded string
    \ingroup qf3lib_mathtools

    \note This method ensures that the doubles are converted to LITTLE ENDIAN before encoding them
 */
QFLIB_EXPORT QByteArray doubleArrayToString_hex(const QVector<double>& data);





/*! \brief decodes a string which contains an array of qlonglongs (64bit), which is encoded in base64
    \ingroup qf3lib_mathtools

    \note The endianess of the data should be LITTLE ENDIAN. The method qlonglongArrayToString_base64() ensures this.
 */
QFLIB_EXPORT QVector<qlonglong> stringToQlonglongArray_base64(const QString& data);
/*! \brief decodes a string which contains an array of qlonglongs (64-bit), which is encoded in hex
    \ingroup qf3lib_mathtools

    \note The endianess of the data should be LITTLE ENDIAN. The method qlonglongArrayToString_hex() ensures this.
 */
QFLIB_EXPORT QVector<qlonglong> stringToQlonglongArray_hex(const QString& data);


/*! \brief encodes an array of qlonglongs (64-bit), into a base64-encoded string
    \ingroup qf3lib_mathtools

    \note This method ensures that the doubles are converted to LITTLE ENDIAN before encoding them
 */
QFLIB_EXPORT QByteArray qlonglongArrayToString_base64(const QVector<qlonglong>& data);
/*! \brief encodes an array of qlonglongs (64-bit), into a hex-encoded string
    \ingroup qf3lib_mathtools

    \note This method ensures that the doubles are converted to LITTLE ENDIAN before encoding them
 */
QFLIB_EXPORT QByteArray qlonglongArrayToString_hex(const QVector<qlonglong>& data);







/*! \brief decodes a string which contains an array of bool (8bit), which is encoded as \c 1=true and \c 0=false
    \ingroup qf3lib_mathtools

 */
QFLIB_EXPORT QVector<bool> stringToBoolArray(const QString& data);



/*! \brief encodes an array of bool (8-bit), into string
    \ingroup qf3lib_mathtools
 */
QFLIB_EXPORT QByteArray boolArrayToString(const QVector<bool>& data);
#endif // BINARYDATATOOLS_H
