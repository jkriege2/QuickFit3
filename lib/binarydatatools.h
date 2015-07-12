/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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

#ifndef BINARYDATATOOLS_H
#define BINARYDATATOOLS_H

#include <stdint.h>
#include <cstdlib>
#include <cstdio>
#include <QString>
#include <QVector>
#include "lib_imexport.h"
#include <QFile>

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


/** \brief read a uint32_t from a binary file
 *  \ingroup qf3lib_mathtools
 */
QFLIB_EXPORT uint32_t binfileReadUint32(QFile& file);

/** \brief read a int32_t from a binary file
 *  \ingroup qf3lib_mathtools
 */
QFLIB_EXPORT int64_t binfileReadInt64(QFile& file);

/** \brief read a int32_t from a binary file
 *  \ingroup qf3lib_mathtools
 */
QFLIB_EXPORT int32_t binfileReadInt32(QFile& file);

/** \brief read a int32_t from a binary file
 *  \ingroup qf3lib_mathtools
 */
QFLIB_EXPORT int16_t binfileReadInt16(QFile& file);

/** \brief read a int32_t from a binary file
 *  \ingroup qf3lib_mathtools
 */
QFLIB_EXPORT int8_t binfileReadInt8(QFile& file);

/** \brief write a uint32_t to a binary file
 *  \ingroup qf3lib_mathtools
 */
QFLIB_EXPORT void binfileWriteUint32(QFile& file, uint32_t data);

/** \brief read a uint64_t from a binary file
 *  \ingroup qf3lib_mathtools
 */
QFLIB_EXPORT uint64_t binfileReadUint64(QFile& file);

/** \brief read a uint64_t from a binary file
 *  \ingroup qf3lib_mathtools
 */
QFLIB_EXPORT uint64_t binfileReadUint64(FILE* file);

/** \brief write a uint64_t to a binary file
 *  \ingroup qf3lib_mathtools
 */
QFLIB_EXPORT void binfileWriteUint64(QFile& file, uint64_t data);

/** \brief read a uint16_t from a binary file
 *  \ingroup qf3lib_mathtools
 */
QFLIB_EXPORT uint16_t binfileReadUint16(QFile& file);

/** \brief write a uint16_t to a binary file
 *  \ingroup qf3lib_mathtools
 */
QFLIB_EXPORT void binfileWriteUint16(QFile& file, uint16_t data);


/** \brief read a uint8_t from a binary file
 *  \ingroup qf3lib_mathtools
 */
QFLIB_EXPORT uint8_t binfileReadUint8(QFile& file);

/** \brief write a uint8_t to a binary file
 *  \ingroup qf3lib_mathtools
 */
QFLIB_EXPORT void binfileWriteUint8(QFile& file, uint8_t data);

/** \brief read a double from a binary file
 *  \ingroup qf3lib_mathtools
 */
QFLIB_EXPORT double binfileReadDouble(QFile& file);

/** \brief write a double to a binary file
 *  \ingroup qf3lib_mathtools
 */
QFLIB_EXPORT void binfileWriteDouble(QFile& file, double data);

/** \brief write an array of dataN doubles to a binary file
 *  \ingroup qf3lib_mathtools
 */
QFLIB_EXPORT void binfileWriteDoubleArray(QFile& file, const double* data, uint32_t dataN);

/** \brief write an array of dataN floatss to a binary file
 *  \ingroup qf3lib_mathtools
 */
QFLIB_EXPORT void binfileWriteFloatArray(QFile& file, const float* data, uint32_t dataN);

/** \brief write an array of dataN doubles to a binary file and subtract 1.0 from every entry
 *  \ingroup qf3lib_mathtools
 */
QFLIB_EXPORT void binfileWriteDoubleArrayMinus1(QFile& file, const double* data, uint32_t dataN);

/** \brief read an array of dataN doubles from a binary file
 *  \ingroup qf3lib_mathtools
 */
QFLIB_EXPORT void binfileReadDoubleArray(QFile& file, double* data, uint32_t dataN);
/** \brief read an array of dataN doubles from a binary file
 *  \ingroup qf3lib_mathtools
 */
QFLIB_EXPORT double* binfileReadDoubleArray(QFile& file, uint32_t dataN);
/** \brief read an array of dataN doubles from a binary file
 *  \ingroup qf3lib_mathtools
 */
QFLIB_EXPORT QVector<double> binfileReadDoubleArrayV(QFile& file, uint32_t dataN);
/** \brief read an array of dataN doubles from a binary file
 *  \ingroup qf3lib_mathtools
 */
QFLIB_EXPORT void binfileReadDoubleArray(QFile& file, QVector<double>& dataOut, uint32_t dataN);


/** \brief read an array of dataN uint16s from a binary file
 *  \ingroup qf3lib_mathtools
 */
QFLIB_EXPORT void binfileReadUInt16Array(QFile& file, uint16_t* data, uint32_t dataN);


#endif // BINARYDATATOOLS_H
