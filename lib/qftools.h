#ifndef QFTOOLS_H
#define QFTOOLS_H


#include <cmath>
#include <cfloat>
#include <QSettings>
#include <QWidget>
#include <QSplitter>
#include <QFile>
#include "lib_imexport.h"

/*! \brief check whether the dlotaing point number is OK (i.e. non-inf, non-NAN)
    \ingroup qf3lib_mathtools
 */
#define QFFloatIsOK(v) (std::isfinite(v))

/*! \brief round a value according to the error
    \ingroup qf3lib_mathtools

    this returns a rounded value that equals the input value with \a addSignifcant mor
    significant digits than the error

    So with \a addSignifcant = 2 The value 1.23456789&pm;0.001
    will be rounded to 1.23456&pm;0.001
*/
QFLIB_EXPORT double roundWithError(double value, double error, int addSignifcant=1) ;

/*! \brief round a value to its first 1+\a addSignifcant significant digits
    \ingroup qf3lib_mathtools

*/
QFLIB_EXPORT double roundError(double error, int addSignifcant=1) ;

/*! \brief store the geometry of a given widget to a QSettings
    \ingroup qf3lib_tools

    This method stores the widget size and position.

    \param settings the QSettings object to write to
    \param widget the widget to store
    \param prefix this prefix is prepended to the QSettings keys
*/
QFLIB_EXPORT void saveWidgetGeometry(QSettings& settings, QWidget* widget, QString prefix=QString(""));

/*! \brief load the geometry of a given widget to a QSettings
    \ingroup qf3lib_tools

    This method loads the widget size and position.

    \param settings the QSettings object to write to
    \param[out] widget the widget to change
    \param prefix this prefix is prepended to the QSettings keys
*/
QFLIB_EXPORT void loadWidgetGeometry(QSettings& settings, QWidget* widget, QString prefix=QString(""));


/*! \brief load the geometry of a given widget to a QSettings
    \ingroup qf3lib_tools

    This method loads the widget size and position.

    \param settings the QSettings object to write to
    \param[out] widget the widget to change
    \param defaultPosition default position of the widget
    \param defaultSize default size of the widget
    \param prefix this prefix is prepended to the QSettings keys
*/
QFLIB_EXPORT void loadWidgetGeometry(QSettings& settings, QWidget* widget, QPoint defaultPosition, QSize defaultSize, QString prefix=QString(""));

/*! \brief store the geometry of a given widget to a QSettings
    \ingroup qf3lib_tools

    This method stores the widget size and position.

    \param settings the QSettings object to write to
    \param widget the widget to store
    \param prefix this prefix is prepended to the QSettings keys
*/
QFLIB_EXPORT void saveSplitter(QSettings& settings, QSplitter* splitter, QString prefix=QString(""));

/*! \brief load the geometry of a given widget to a QSettings
    \ingroup qf3lib_tools

    This method loads the widget size and position.

    \param settings the QSettings object to write to
    \param[out] plitter the plitter to change
    \param prefix this prefix is prepended to the QSettings keys
*/
QFLIB_EXPORT void loadSplitter(QSettings& settings, QSplitter* splitter, QString prefix=QString(""));

/*! \brief outputs a string designating the type of a QVariant
    \ingroup qf3lib_tools

    \param variant input QVariant
    \return a string describing the type of \a variant
*/
QFLIB_EXPORT QString getQVariantType(const QVariant& variant);

/*! \brief outputs a string encoding the contents of a QVariant
    \ingroup qf3lib_tools

    \param variant input QVariant
    \return a string encoding the contents of \a variant
*/
QFLIB_EXPORT QString getQVariantData(const QVariant& variant);

/*! \brief outputs a Variant, from a type and data, as encoded by getQVariantType() and getQVariantData()
    \ingroup qf3lib_tools

    \param type type string
    \param data contents of the new QVariant, encoded as string
    \return a QVariant, based on \a type and \a data. If the data is invalid for some reason, the function returns an invalid QVariant
*/
QFLIB_EXPORT QVariant getQVariantFromString(const QString& type, const QString& data);

/*! \brief convert a number to a QString with a given decimalSeparator
    \ingroup qf3lib_tools

*/
QFLIB_EXPORT QString doubleToQString(double value, int prec = 10, char f = 'g', QChar decimalSeparator=',' );

/*! \brief convert a number to a QString with the c-locale (and without group separator)
    \ingroup qf3lib_tools

*/
QFLIB_EXPORT QString CDoubleToQString(double value);

/*! \brief convert a number to a QString with the c-locale (and without group separator)
    \ingroup qf3lib_tools

*/
QFLIB_EXPORT double CQStringToDouble(QString value);

/*! \brief convert a string \a data to a boolean
    \ingroup qf3lib_tools

*/
QFLIB_EXPORT bool QStringToBool(const QString& data);

/*! \brief make sure the given file exists (creates an empty file)
    \ingroup qf3lib_tools

*/
QFLIB_EXPORT bool touchFile(const QString& filename);



/*! \brief escapify a string
    \ingroup qf3lib_tools

*/
QFLIB_EXPORT QString escapify(const QString& text);


/*! \brief deescapify a string
    \ingroup qf3lib_tools

*/
QFLIB_EXPORT QString deescapify(const QString& text);



/*! \brief escapify a string
    \ingroup qf3lib_tools

*/
QFLIB_EXPORT QString escapify(const QStringList& text);


/*! \brief deescapify a string
    \ingroup qf3lib_tools

*/
QFLIB_EXPORT QStringList deescapifyList(const QString& text);
#endif // QFTOOLS_H
