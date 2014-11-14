/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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

#ifndef QFTOOLS_H
#define QFTOOLS_H
#include <QTextStream>
#include "qfmathtools.h"

#include <cmath>
#include <cfloat>
#include <stdint.h>
#include <QSettings>
#include <QWidget>
#include <QDomDocument>
#include <QSplitter>
#include <QFile>
#include "lib_imexport.h"
#include <QFileDialog>
#include <QRegExp>
#include <QAction>
#include <QToolButton>
#include <QString>
#include <QIcon>
#include <QListWidget>
#include "qtriple.h"
#include "qfpluginservices.h"
#include <QXmlStreamWriter>
#include <QBitArray>


#define DEFINE_READWRITE_LOCKERS(CLASS, readLockF, writeLockF, readUnlockF, writeUnlockF) \
    class QFLIB_EXPORT CLASS##ReadLocker {\
        private:\
            const CLASS* item;\
            bool locked;\
        public:\
            inline CLASS##ReadLocker(const CLASS* item) {\
                this->item=item;\
                item->readLockF();\
                locked=true;\
            }\
            inline ~CLASS##ReadLocker() {\
                item->readUnlockF();\
                locked=false;\
            }\
            inline void unlock() {\
                if (!locked) return;\
                item->readUnlockF();\
                locked=false;\
            }\
            inline void relock() {\
                if (locked) return;\
                item->readLockF();\
                locked=true;\
            }\
    };\
    class QFLIB_EXPORT CLASS##WriteLocker {\
        private:\
            const CLASS* item;\
            bool locked;\
        public:\
            inline CLASS##WriteLocker(const CLASS* item) {\
                this->item=item;\
                item->writeLockF();\
                locked=true;\
            }\
            inline ~CLASS##WriteLocker() {\
                item->writeUnlockF();\
                locked=false;\
            }\
            inline void unlock() {\
                if (!locked) return;\
                item->writeUnlockF();\
                locked=false;\
            }\
            inline void relock() {\
                if (locked) return;\
                item->writeLockF();\
                locked=true;\
            }\
    };




class ProgramOptions; // forward

/*! \brief QuickFit's internal malloc replacement (on some systems, this does a boundary-aligned malloc)
    \ingroup qf3lib_tools
 */
QFLIB_EXPORT void* qfMalloc(size_t size);

/*! \brief Tool for malloc, which allows to allocate with the type specified as template parameter
    \ingroup qf3lib_tools
 */
template <typename T>
inline T* qfMallocT(size_t items) {
    return (T*)qfMalloc(items*sizeof(T));
}

/*! \brief QuickFit's internal malloc replacement (on some systems, this does a boundary-aligned malloc)
    \ingroup qf3lib_tools
 */
QFLIB_EXPORT void* qfCalloc(size_t num, size_t size);

/*! \brief QuickFit's internal malloc replacement (on some systems, this does a boundary-aligned malloc)
    \ingroup qf3lib_tools
 */
QFLIB_EXPORT void* qfCalloc(size_t size);

/*! \brief Tool for malloc, which allows to allocate with the type specified as template parameter
    \ingroup qf3lib_tools
 */
template <typename T>
inline T* qfCallocT(size_t items) {
    return (T*)qfCalloc(items, sizeof(T));
}
/*! \brief QuickFit's internal realloc replacement (on some systems, this does a boundary-aligned malloc)
    \ingroup qf3lib_tools
 */
QFLIB_EXPORT void* qfRealloc (void* ptr, size_t size);
template <typename T>
inline T* qfReallocT(T* ptr, size_t items) {
    return (T*)qfRealloc(ptr, items*sizeof(T));
}

/*! \brief QuickFit's internal free replacement
    \ingroup qf3lib_tools
 */
QFLIB_EXPORT void qfFree(void* data);



typedef QMap<QString, QVariant> QFStringVariantMap;
/*! \brief returns a new QToolButton for the specified QAction
    \ingroup qf3lib_tools
 */
QFLIB_EXPORT QToolButton* createButtonForAction(QAction* action, QWidget* parent);

/*! \brief returns a new separator action
    \ingroup qf3lib_tools
 */
QFLIB_EXPORT QAction* getSeparatorAction(QObject* parent=NULL);


/*! \brief creates an action and a toolbutton which uses it (in \a button)
    \ingroup qf3lib_tools
 */
QFLIB_EXPORT QAction* createActionAndButton(QToolButton*& button, QWidget* parent);


/*! \brief creates an action and a toolbutton which uses it (in \a button)
    \ingroup qf3lib_tools
 */
QFLIB_EXPORT QAction* createActionAndButton(QToolButton*& button, const QString& text, QWidget* parent);

/*! \brief creates an action and a toolbutton which uses it (in \a button)
    \ingroup qf3lib_tools
 */
QFLIB_EXPORT QAction* createActionAndButton(QToolButton*& button, const QIcon& icon, const QString& text, QWidget* parent=NULL);






/*! \brief creates an action and a toolbutton which uses it (in \a button)
    \ingroup qf3lib_tools
 */
QFLIB_EXPORT QToolButton* createButtonAndAction(QAction*& action, QWidget* parent);


/*! \brief creates an action and a toolbutton which uses it (in \a button)
    \ingroup qf3lib_tools
 */
QFLIB_EXPORT QToolButton* createButtonAndAction(QAction*& action, const QString& text, QWidget* parent);

/*! \brief creates an action and a toolbutton which uses it (in \a button)
    \ingroup qf3lib_tools
 */
QFLIB_EXPORT QToolButton* createButtonAndAction(QAction*& action, const QIcon& icon, const QString& text, QWidget* parent=NULL);




/*! \brief creates an action and a toolbutton which uses it (in \a button)
    \ingroup qf3lib_tools
 */
QFLIB_EXPORT QToolButton* createButtonAndActionShowText(QAction*& action, QWidget* parent);


/*! \brief creates an action and a toolbutton which uses it (in \a button)
    \ingroup qf3lib_tools
 */
QFLIB_EXPORT QToolButton* createButtonAndActionShowText(QAction*& action, const QString& text, QWidget* parent);

/*! \brief creates an action and a toolbutton which uses it (in \a button)
    \ingroup qf3lib_tools
 */
QFLIB_EXPORT QToolButton* createButtonAndActionShowText(QAction*& action, const QIcon& icon, const QString& text, QWidget* parent=NULL);

/*! \brief creates a toolbutton for a QAction
    \ingroup qf3lib_tools
 */
QFLIB_EXPORT QToolButton* createButtonForActionShowText(QAction* action, QWidget* parent);


/*! \brief store the geometry of a given widget to a QSettings
    \ingroup qf3lib_tools

    This method stores the widget size and position.

    \param settings the QSettings object to write to
    \param widget the widget to store
    \param prefix this prefix is prepended to the QSettings keys
*/
QFLIB_EXPORT void saveWidgetGeometry(QSettings& settings, QWidget* widget, QString prefix=QString(""));
QFLIB_EXPORT void saveWidgetGeometry(QSettings* settings, QWidget* widget, QString prefix=QString(""));
QFLIB_EXPORT void saveWidgetGeometry(ProgramOptions* settings, QWidget* widget, QString prefix=QString(""));
/*! \brief load the geometry of a given widget to a QSettings
    \ingroup qf3lib_tools

    This method loads the widget size and position.

    \param settings the QSettings object to write to
    \param[out] widget the widget to change
    \param prefix this prefix is prepended to the QSettings keys
*/
QFLIB_EXPORT void loadWidgetGeometry(QSettings& settings, QWidget* widget, QString prefix=QString(""));
QFLIB_EXPORT void loadWidgetGeometry(QSettings* settings, QWidget* widget, QString prefix=QString(""));
QFLIB_EXPORT void loadWidgetGeometry(ProgramOptions* settings, QWidget* widget, QString prefix=QString(""));


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


/*! \brief writes a QVariant into XML (value as text and type as type-parameter, encodes lists)
    \ingroup qf3lib_tools

    \note before calling this, you have to start an element using w.writeStartElement() and after this finishes, you have to call w.writeEndElement()
*/
QFLIB_EXPORT void writeQVariant(QXmlStreamWriter& w, const QVariant& variant);

/*! \brief read a QVariant from an XML node (value as text and type as type-parameter, encoded lists)
    \ingroup qf3lib_tools

*/
QFLIB_EXPORT QVariant readQVariant(QDomElement& e);




/*! \brief outputs a string encoding the contents of a QVariant
    \ingroup qf3lib_tools

    \param variant input QVariant
    \return a string encoding the contents of \a variant
*/
QFLIB_EXPORT QString getQVariantData(const QVariant& variant);
/*! \brief outputs a string encoding the contents of a QVariant, strings are C-escaped and if quotations is given, the result is enclosed in quotation marks, if it is a string.
    \ingroup qf3lib_tools

    \param variant input QVariant
    \return a string encoding the contents of \a variant
*/
QFLIB_EXPORT QString getQVariantDataCEscaped(const QVariant& variant, QChar quotations='\0');

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
QFLIB_EXPORT QString doubleToQString(double value, int prec = 10, char f = 'g', QChar decimalSeparator='.' );
/*! \brief convert a number to a QString with a given decimalSeparator, remove trailing 0
    \ingroup qf3lib_tools

*/
QFLIB_EXPORT QString doubleToQStringNoTrail(double value, int prec = 10, QChar decimalSeparator='.' );


/*! \brief convert a number to a QString in exponential form using LaTeX markup
    \ingroup qf3lib_tools

*/
QFLIB_EXPORT QString doubleToLatexQString(double data, int precision=10, bool remove_trail0=true, double belowIsZero=1e-16, double minNoExponent=1e-3, double maxNoExponent=1e4);

/*! \brief convert an integer to a QString in decimal representation
    \ingroup qf3lib_tools

*/
QFLIB_EXPORT QString intToQString(int64_t value);
/*! \brief convert an integer to a QString in binary representation
    \ingroup qf3lib_tools

*/
QFLIB_EXPORT QString intToBinaryQString(int64_t value);
/*! \brief convert an integer to a QString in hexadecimal representation
    \ingroup qf3lib_tools

*/
QFLIB_EXPORT QString intToHexQString(int64_t value);
/*! \brief convert a pointer to a QString in hexadecimal representation
    \ingroup qf3lib_tools

*/
QFLIB_EXPORT QString pntToHexQString(const void* value);
/*! \brief convert an integer to a QString in octal representation
    \ingroup qf3lib_tools

*/
QFLIB_EXPORT QString intToOctQString(int64_t value);

/*! \brief convert a vector of numbers to a QString with a given decimalSeparator
    \ingroup qf3lib_tools

*/
QFLIB_EXPORT QString doubleVecToQString(const QVector<double>& value, int prec = 10, char f = 'g', QChar decimalSeparator='.', const QString itemSeparator=", " );
/*! \brief convert a vector of booleans to a QString
    \ingroup qf3lib_tools

*/
QFLIB_EXPORT QString bitarrayToQString(const QBitArray& value, const QString itemSeparator=", ", const QString& trueName=QString("true"), const QString& falseName=QString("false") );
/*! \brief convert a vector of booleans to a QString
    \ingroup qf3lib_tools

*/
QFLIB_EXPORT QVector<double> bitarrayToNumVec(const QBitArray& value, double trueValue=1, double falseValue=0);
/*! \brief convert a vector of booleans to a QString
    \ingroup qf3lib_tools

*/
QFLIB_EXPORT QString boolvectorToQString(const QVector<bool>& value, const QString itemSeparator=", ", const QString& trueName=QString("true"), const QString& falseName=QString("false") );
/*! \brief convert a vector of booleans to a QString
    \ingroup qf3lib_tools

*/
QFLIB_EXPORT QVector<double> boolvectorToNumVec(const QVector<bool>& value, double trueValue=1, double falseValue=0);
/*! \brief convert a number to a QString with a given decimalSeparator
    \ingroup qf3lib_tools

*/
QFLIB_EXPORT QString doubleToUnitString(double value, int past_comma=5, bool remove_trail0=false, QChar decimalSeparator='.', bool HTMLout=false );

/*! \brief convert a number to a QString with the c-locale (and without group separator)
    \ingroup qf3lib_tools

*/
QFLIB_EXPORT QString CDoubleToQString(double value);

/*! \brief convert a list of numbers to a QString with the c-locale (and without group separator)
    \ingroup qf3lib_tools

*/
QFLIB_EXPORT QString CDoubleListToQString(const QList<double> values, const QString &separator=QString(", "));

/*! \brief convert a number to a QString with the c-locale (and without group separator)
    \ingroup qf3lib_tools

*/
QFLIB_EXPORT double CQStringToDouble(QString value);

/*! \brief convert a QString to a double and accept ',' and '.' as decimal separator
    \ingroup qf3lib_tools

*/
QFLIB_EXPORT double QStringToDouble(QString value);

/*! \brief convert a string \a data to a boolean
    \ingroup qf3lib_tools

*/
QFLIB_EXPORT bool QStringToBool(const QString& data);

/*! \brief convert a string \a data to a QColor
    \ingroup qf3lib_tools

*/
QFLIB_EXPORT QColor QStringToQColor(const QString& data);

/*! \brief convert a bool \a data to a string
    \ingroup qf3lib_tools

*/
QFLIB_EXPORT QString boolToQString(bool data);
/*! \brief make sure the given file exists (creates an empty file)
    \ingroup qf3lib_tools

*/
QFLIB_EXPORT bool touchFile(const QString& filename);
/*! \brief returns the given filename with its extension replaced by \a extension
    \ingroup qf3lib_tools

*/
QFLIB_EXPORT QString replaceFileExt(const QString& filename, const QString& extension);
/*! \brief reads the contenst of a file
    \ingroup qf3lib_tools

*/
QFLIB_EXPORT QString readFile(const QString& filename);
/*! \brief convert a QChar \a data to a human-readable string
    \ingroup qf3lib_tools


    This function recognises some unprintable characters and returns them as follows:
\verbatim
 ASCII value | returned string
-------------+-----------------
           0 |     [NUL]
           1 |     [SOH]
           2 |     [STX]
           3 |     [ETX]
           4 |     [EOT]
           5 |     [ENQ]
           6 |     [ACK]
           7 |     [BEL]
           8 |     [BS]
           9 |     [TAB]
          10 |     [LF]
          11 |     [VT]
          12 |     [FF]
          13 |     [CR]
          14 |     [SO]
          15 |     [SI]
          16 |     [DLE]
          17 |     [DC1]
          18 |     [DC2]
          19 |     [DC3]
          20 |     [DC4]
          21 |     [NAK]
          22 |     [SYN]
          23 |     [ETB]
          24 |     [CAN]
          25 |     [EM]
          26 |     [SUB]
          27 |     [ESC]
          28 |     [FS]
          29 |     [GS]
          30 |     [RS]
          31 |     [US]
          32 |     [SP]
         127 |     [DEL]
\endverbatim
*/
QFLIB_EXPORT QString qchartostr(QChar data);



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

    The string has to be composed of substrings, delimited by \a stringDelim (e.g. an apostrophy) and separated by
    \a separator (e.g. \c ; ).

*/
QFLIB_EXPORT QStringList deescapifyList(const QString& text, char stringDelim, char separator);

/*! \brief deescapify a string, with the default delimiter \c ' and the separator \c ; If this configuration does not work, double-quotes are used as separator.
    \ingroup qf3lib_tools
 */
QFLIB_EXPORT QStringList deescapifyList(const QString& text);




/*! \brief like QFileDialog::getOpenFileName(), but uses the system dialog on windows and a custom dialog on Linux, uses ProgramOptions to save the last filter and directory
    \ingroup qf3lib_tools
 */
QFLIB_EXPORT QString qfGetOpenFileNameSet (const QString& setPrefix,  QWidget * parent = 0, const QString & caption = QString(), const QString & dir = QString(), const QString & filter = QString(), QString * selectedFilter = 0, QFileDialog::Options options = 0 );
/*! \brief like QFileDialog::getOpenFileNames(), but uses the system dialog on windows and a custom dialog on Linux, uses ProgramOptions to save the last filter and directory
    \ingroup qf3lib_tools
 */
QFLIB_EXPORT QStringList qfGetOpenFileNamesSet (const QString& setPrefix,  QWidget * parent = 0, const QString & caption = QString(), const QString & dir = QString(), const QString & filter = QString(), QString * selectedFilter = 0, QFileDialog::Options options = 0 );
/*! \brief like QFileDialog::getSaveFileName(), but uses the system dialog on windows and a custom dialog on Linux, uses ProgramOptions to save the last filter and directory
    \ingroup qf3lib_tools
 */
QFLIB_EXPORT QString qfGetSaveFileNameSet (const QString& setPrefix,  QWidget * parent = 0, const QString & caption = QString(), const QString & dir = QString(), const QString & filter = QString(), QString * selectedFilter = 0, QFileDialog::Options options = 0 );




/*! \brief like QFileDialog::getOpenFileName(), but uses the system dialog on windows and a custom dialog on Linux
    \ingroup qf3lib_tools
 */
QFLIB_EXPORT QString qfGetOpenFileName ( QWidget * parent = 0, const QString & caption = QString(), const QString & dir = QString(), const QString & filter = QString(), QString * selectedFilter = 0, QFileDialog::Options options = 0 );
/*! \brief like QFileDialog::getOpenFileNames(), but uses the system dialog on windows and a custom dialog on Linux
    \ingroup qf3lib_tools
 */
QFLIB_EXPORT QStringList qfGetOpenFileNames ( QWidget * parent = 0, const QString & caption = QString(), const QString & dir = QString(), const QString & filter = QString(), QString * selectedFilter = 0, QFileDialog::Options options = 0 );
/*! \brief like QFileDialog::getSaveFileName(), but uses the system dialog on windows and a custom dialog on Linux
    \ingroup qf3lib_tools
 */
QFLIB_EXPORT QString qfGetSaveFileName ( QWidget * parent = 0, const QString & caption = QString(), const QString & dir = QString(), const QString & filter = QString(), QString * selectedFilter = 0, QFileDialog::Options options = 0 );
/*! \brief like QFileDialog::getSaveFileName(), but uses the system dialog on windows and a custom dialog on Linux
    \ingroup qf3lib_tools
 */
QFLIB_EXPORT QString qfGetExistingDirectory ( QWidget * parent = 0, const QString & caption = QString(), const QString & dir = QString(), QFileDialog::Options options = QFileDialog::ShowDirsOnly );
/*! \brief determine bit-depth of adressing
    \ingroup qf3lib_tools
 */
QFLIB_EXPORT int getApplicationBitDepth();

/*! \brief returns the operating system's name as a QString
    \ingroup qf3lib_tools
 */
QFLIB_EXPORT QString getOSName();

/*! \brief returns the operating system's name as a QString
    \ingroup qf3lib_tools
 */
QFLIB_EXPORT QString getOSShortName();

/*! \brief convert the given string to a string suitable for use in filenames
    \ingroup qf3lib_tools
*/
QFLIB_EXPORT QString cleanStringForFilename(const QString& text, int maxLen=127, bool removeDot=true, bool removeSlash=true);

/*! \brief adds an increasing number to the given filename, so the returned filename is not yet existent
    \ingroup qf3lib_tools
*/
QFLIB_EXPORT QString getNewFilename(const QString& filename, const QDir& dir=QDir());


/*! \brief reads the string until it finds an integer number and reads until the number end, so from "SVN2345AB" it will return 2345
    \ingroup qf3lib_tools
 */
QFLIB_EXPORT int qfReadFirstInt(const QString& text, bool* ok=NULL);

/*! \brief convert the given string to a string suitable for use in filenames
    \ingroup qf3lib_tools
*/
QFLIB_EXPORT QListWidgetItem* addCheckableQListWidgetItem(QListWidget* listWidget, const QString& text, Qt::CheckState checked=Qt::Unchecked, const QVariant& userData=QVariant());


/*! \brief remove all actions from a QWidget
    \ingroup qf3lib_tools
*/
QFLIB_EXPORT void removeAllActions(QWidget* widget);


/*! \brief return a color that encodes \a max items in diferent colors. \a value gives the (0-based) item ID.
    \ingroup qf3lib_tools
*/
QFLIB_EXPORT QColor getCycleColor(int value, unsigned int max, double S=1.0, double V=1.0);

/*! \brief remove all duplicates from a list
    \ingroup qf3lib_tools

    This copies the input list to the return argument omitting all duplicates. This should have complexity \f$ \mathcal{O}(N^2) \f$ but preserves the sorting order!
*/
template <class T>
QList<T> removeQListDouplicates(const QList<T>& list) {
    QList<T> out;
    for (int i=0; i<list.size(); i++) {
        if (out.indexOf(list[i])<0) out<<list[i];
    }
    return out;
}


/*! \brief constructs a list with the given default item contained N times
    \ingroup qf3lib_tools
*/
template <class T>
QList<T> constructQListWithMultipleItems(const T& item, int N=1) {
    QList<T> out;
    for (int i=0; i<N; i++) {
        out<<item;
    }
    return out;
}

/*! \brief constructs a list with the given default item contained
    \ingroup qf3lib_tools
*/
template <class T>
QList<T> constructQListFromItems(const T& item1) {
    QList<T> out;
    out<<item1;
    return out;
}

/*! \brief constructs a list with the given default item contained
    \ingroup qf3lib_tools
*/
template <class T>
QList<T> constructQListFromItems(const T& item1, const T& item2) {
    QList<T> out;
    out<<item1<<item2;
    return out;
}
/*! \brief constructs a list with the given default item contained
    \ingroup qf3lib_tools
*/
template <class T>
QList<T> constructQListFromItems(const T& item1, const T& item2, const T& item3) {
    QList<T> out;
    out<<item1<<item2<<item3;
    return out;
}

/*! \brief constructs a list with the given default item contained
    \ingroup qf3lib_tools
*/
template <class T>
QList<T> constructQListFromItems(const T& item1, const T& item2, const T& item3, const T& item4) {
    QList<T> out;
    out<<item1<<item2<<item3<<item4;
    return out;
}

/*! \brief constructs a list with the given default item contained
    \ingroup qf3lib_tools
*/
template <class T>
QList<T> constructQListFromItems(const T& item1, const T& item2, const T& item3, const T& item4, const T& item5) {
    QList<T> out;
    out<<item1<<item2<<item3<<item4<<item5;
    return out;
}












/*! \brief remove all duplicates from a list
    \ingroup qf3lib_tools

    This copies the input list to the return argument omitting all duplicates. This should have complexity \f$ \mathcal{O}(N^2) \f$ but preserves the sorting order!
*/
template <class T>
QVector<T> removeQVectorDouplicates(const QVector<T>& list) {
    QVector<T> out;
    for (int i=0; i<list.size(); i++) {
        if (out.indexOf(list[i])<0) out<<list[i];
    }
    return out;
}


/*! \brief constructs a list with the given default item contained N times
    \ingroup qf3lib_tools
*/
template <class T>
QVector<T> constructQVectorWithMultipleItems(const T& item, int N=1) {
    QVector<T> out;
    for (int i=0; i<N; i++) {
        out<<item;
    }
    return out;
}

/*! \brief constructs a list with the given default item contained
    \ingroup qf3lib_tools
*/
template <class T>
QVector<T> constructQVectorFromItems(const T& item1) {
    QVector<T> out;
    out<<item1;
    return out;
}

/*! \brief constructs a list with the given default item contained
    \ingroup qf3lib_tools
*/
template <class T>
QVector<T> constructQVectorFromItems(const T& item1, const T& item2) {
    QVector<T> out;
    out<<item1<<item2;
    return out;
}
/*! \brief constructs a list with the given default item contained
    \ingroup qf3lib_tools
*/
template <class T>
QVector<T> constructQVectorFromItems(const T& item1, const T& item2, const T& item3) {
    QVector<T> out;
    out<<item1<<item2<<item3;
    return out;
}

/*! \brief constructs a list with the given default item contained
    \ingroup qf3lib_tools
*/
template <class T>
QVector<T> constructQVectorFromItems(const T& item1, const T& item2, const T& item3, const T& item4) {
    QVector<T> out;
    out<<item1<<item2<<item3<<item4;
    return out;
}

/*! \brief constructs a list with the given default item contained
    \ingroup qf3lib_tools
*/
template <class T>
QVector<T> constructQVectorFromItems(const T& item1, const T& item2, const T& item3, const T& item4, const T& item5) {
    QVector<T> out;
    out<<item1<<item2<<item3<<item4<<item5;
    return out;
}









/*! \brief constructs a list with the given default item contained
    \ingroup qf3lib_tools
*/
inline QStringList constructQStringListFromItems(const QString& item1) {
    QStringList out;
    out<<item1;
    return out;
}

/*! \brief constructs a list with the given default item contained
    \ingroup qf3lib_tools
*/

inline QStringList constructQStringListFromItems(const QString& item1, const QString& item2) {
    QStringList out;
    out<<item1<<item2;
    return out;
}
/*! \brief constructs a list with the given default item contained
    \ingroup qf3lib_tools
*/

inline QStringList constructQStringListFromItems(const QString& item1, const QString& item2, const QString& item3) {
    QStringList out;
    out<<item1<<item2<<item3;
    return out;
}

/*! \brief constructs a list with the given default item contained
    \ingroup qf3lib_tools
*/

inline QStringList constructQStringListFromItems(const QString& item1, const QString& item2, const QString& item3, const QString& item4) {
    QStringList out;
    out<<item1<<item2<<item3<<item4;
    return out;
}

/*! \brief constructs a list with the given default item contained
    \ingroup qf3lib_tools
*/

inline QStringList constructQStringListFromItems(const QString& item1, const QString& item2, const QString& item3, const QString& item4, const QString& item5) {
    QStringList out;
    out<<item1<<item2<<item3<<item4<<item5;
    return out;
}




/*! \brief returns a list which only contains those items of the input list that could be cast to the given \tparam TFilter class
    \ingroup qf3lib_tools
*/
template <class T, class TFilter>
QList<TFilter*> filterListForClass(const QList<T*>& list_in) {
    QList<TFilter*> list;
    for (int i=0; i<list_in.size(); i++) {
        if (dynamic_cast<TFilter*>(list_in[i])) {
            list<<dynamic_cast<TFilter*>(list_in[i]);
        }
    }
    return list;
}





/*! \brief remove all duplicates from a list
    \ingroup qf3lib_tools

    This copies the input list to the return argument omitting all duplicates. This should have complexity \f$ \mathcal{O}(N^2) \f$ but preserves the sorting order!
*/
template <class T>
QVector<T> removeQListDouplicates(const QVector<T>& list) {
    QVector<T> out;
    for (int i=0; i<list.size(); i++) {
        if (out.indexOf(list[i])<0) out<<list[i];
    }
    return out;
}



/*! \brief swap two elements \a l and \a r in an array \a a
    \ingroup tools_math_stat

*/
template <class T>
inline void qfstatisticsSwap(T* a, long long l, long long r){
    const T tmp=a[l];
    a[l]=a[r];
    a[r]=tmp;
}




/*! \brief QuickSort (recursive implementation)
    \ingroup qf3lib_tools

    implementation from http://www.linux-related.de/index.html?/coding/sort/sort_quick.htm
*/
template <class T>
void qfstatisticsQuicksort(T* a, long long l, long long r){
    if(r>l){
        long long i=l-1;
        long long j=r;

        for(;;){
            while(a[++i]<a[r]);
            while(a[--j]>a[r] && j>i);
            if(i>=j) break;
            qfstatisticsSwap<T>(a, i, j);
        }
        qfstatisticsSwap<T>(a, i, r);

        qfstatisticsQuicksort(a, l, i-1);
        qfstatisticsQuicksort(a, i+1, r);
    }
}


/*! \brief QuickSort (recursive implementation), sorts \a a2 alongside \a a, using \a a as sort criterion
    \ingroup qf3lib_tools

    implementation from http://www.linux-related.de/index.html?/coding/sort/sort_quick.htm
*/
template <class T>
void qfstatisticsQuicksort(T* a, T* a2, long long l, long long r){
    if(r>l){
        long long i=l-1;
        long long j=r;

        for(;;){
            while(a[++i]<a[r]);
            while(a[--j]>a[r] && j>i);
            if(i>=j) break;
            qfstatisticsSwap(a, i, j);
            qfstatisticsSwap(a2, i, j);
        }
        qfstatisticsSwap(a, i, r);
        qfstatisticsSwap(a2, i, r);

        qfstatisticsQuicksort(a, a2, l, i-1);
        qfstatisticsQuicksort(a, a2, i+1, r);
    }
}







/*! \brief sort the given arrays, using \a input as sort criterion
    \ingroup qf3lib_tools

    \param input array to be sorted
    \param input2 array to be sorted
    \param N size of the array input
    \param output if \c !=NULL data is written here (the memory location pointed at by \a output has to have at least the length \a N !!!),
                  otherwise the array input is sorted inplace.
    \param output2 if \c !=NULL data is written here (the memory location pointed at by \a output has to have at least the length \a N !!!),
                  otherwise the array input is sorted inplace.

 */
template <class T>
inline void qfstatisticsSort(T* input, T* input2, long long N, T* output=NULL, T* output2=NULL) {
    if ((!input)) return ;
    if (N<=0) return;
    T* data=input;
    if (output!=NULL) {
        data=output;
        memcpy(output, input, N*sizeof(T));
    }
    T* data2=input2;
    if (output2!=NULL && input2!=NULL) {
        data2=output2;
        memcpy(output2, input2, N*sizeof(T));
    }
    qfstatisticsQuicksort(data, data2, 0, N-1);
}



/*! \brief sort the given array
    \ingroup qf3lib_tools

    \param input array to be sorted
    \param N size of the array input
    \param output if \c !=NULL data is written here (the memory location pointed at by \a output has to have at least the length \a N !!!),
                  otherwise the array input is sorted inplace.

 */
template <class T>
inline void qfstatisticsSort(T* input, long long N, T* output=NULL) {
    if ((!input)) return ;
    if (N<=0) return;
    T* data=input;
    if (output!=NULL) {
        data=output;
        memcpy(output, input, N*sizeof(T));
    }
    qfstatisticsQuicksort(data, 0, N-1);
}













/*! \brief duplicate a memory array, i.e. reserve the data of the array a second time using qfMalloc() and copy the contents
    \ingroup qf3lib_tools

*/
template <class T>
QVector<T> duplicateArrayV(const T* input, long long N) {
    if (!input||N<=0) return QVector<T>();
    QVector<T> out(N);
    for (long long i=0; i<N; i++) {
        out[i]=input[i];
    }
    return out;
}

/*! \brief duplicate a memory array, i.e. reserve the data of the array a second time using qfMalloc() and copy the contents
    \ingroup qf3lib_tools

*/
template <class T>
T* duplicateArray(const T* input, long long N) {
    if (!input||N<=0) return NULL;
    T* out=(T*)qfMalloc(N*sizeof(T));
    if (out && input && N>=0) memcpy(out, input, N*sizeof(T));
    return out;
}

/*! \brief duplicate a memory array, i.e. reserve the data of the array a second time using qfMalloc() and copy the contents
    \ingroup qf3lib_tools

*/
template <class T>
T* duplicateArray(const QVector<T>& input) {
    long long N=input.size();
    if (!input||N<=0) return NULL;
    T* out=(T*)qfMalloc(N*sizeof(T));
    if (out && input && N>=0) memcpy(out, input, N*sizeof(T));
    return out;
}
/*! \brief duplicate a memory array, i.e. reserve the data of the array a second time using qfMalloc() and copy the contents
    \ingroup qf3lib_tools

*/
template <class T>
T* qfDuplicateArray(const T* input, long long N) {
    if (!input||N<=0) return NULL;
    T* out=(T*)qfMalloc(N*sizeof(T));
    if (out && input && N>=0) memcpy(out, input, N*sizeof(T));
    return out;
}
/*! \brief duplicate a memory array, i.e. reserve the data of the array a second time using qfMalloc() and copy the contents
    \ingroup qf3lib_tools

*/
template <class T>
T* duplicateArraySort(const T* input, long long N) {
    T* out=duplicateArray(input, N);
    if (out && N>0) qfstatisticsSort(out, N);
    return out;
}

/*! \brief returns a vector containing all elements of the given array
    \ingroup qf3lib_tools

*/
template <class T>
QVector<T> arrayToVector(const T* input, long long N) {
    T dummy;
    QVector<T> out(N, dummy);
    if (input && N>0) memcpy(out.data(), input, N*sizeof(T));
    return out;
}

/*! \brief copies the contents inf \a input (\a N elements) into \a out and returns \a out
    \ingroup qf3lib_tools

*/
template <class T>
T* copyArray(T* out, const T* input, long long N) {
    if (out && input && N>0) memcpy(out, input, N*sizeof(T));
    return out;
}

/*! \brief copies the contents inf \a input (\a N elements) into \a out and returns \a out or fills out with \a defaultValue if \a input is \c NULL
    \ingroup qf3lib_tools

*/
template <class T>
T* copyArrayOrDefault(T* out, const T* input, long long N, T defaultValue) {
    if (input) {
        if (out!=input) memcpy(out, input, N*sizeof(T));
    } else {
        for (long long i=0; i<N; i++) {
            out[i]=defaultValue;
        }
    }
    return out;
}


/*! \brief builds a string out of a given array
    \ingroup qf3lib_tools

*/
template <class T>
QString arrayToString(const T* input, long long N, bool withBrackets=true) {
    QString res;
    {
        QTextStream txt(&res, QIODevice::WriteOnly);
        bool first=true;
        for (int i=0; i<N; i++) {
            if (!first) {
                txt<<QString(", ");
            }
            txt<<input[i];
            first=false;
        }
    }
    if (withBrackets) return QString("[ %1 ]").arg(res);
    return res;
}


/*! \brief takes an array of integer number created with arrayToString() or listToString(), e.g. "0,2,3,4" and returns it as a QList<int>
    \ingroup qf3lib_tools

*/

QFLIB_EXPORT QList<int> stringToIntList(const QString& data);

/*! \brief builds a string out of a given container (with size() and operator[]). If \a withIDs is \c true, the index will be output in front of each value.
    \ingroup qf3lib_tools

*/
template <class T>
QString listToString(const T& input, bool withIDs=false, bool withBrackets=true) {
    QString res;
    {
        QTextStream txt(&res, QIODevice::WriteOnly);
        bool first=true;
        for (int i=0; i<input.size(); i++) {
            if (!first) {
                txt<<QString(", ");
            }
            if (withIDs) {
                txt<<i<<":";
            }
            txt<<input[i];
            first=false;
        }
    }
    if (withBrackets) return QString("[ %1 ]").arg(res);
    return res;
}

/*! \brief builds a string out of a given container (with size() and operator[]). If \a withIDs is \c true, the index will be output in front of each value.
    \ingroup qf3lib_tools

*/
template <class T>
QString numlistToString(const T& input, bool withIDs=false, int precision=10) {
    QString res;
    {
        QTextStream txt(&res, QIODevice::WriteOnly);
        bool first=true;
        for (int i=0; i<input.size(); i++) {
            if (!first) {
                txt<<QString(", ");
            }
            if (withIDs) {
                txt<<i<<":";
            }
            txt<<QString::number(input[i], 'g', precision);
            first=false;
        }
    }
    return QString("[ %1 ]").arg(res);
}
/*! \brief returns a list of all items in the list that have a given value
    \ingroup qf3lib_tools

*/
template <class T>
QList<int> valueIndizesFromArray(const T* input, int N, T value) {
    QList<int> l;
    if (N>0 && input) {
        for (int i=0; i<N; i++) {
            if (input[i]==value) l<<i;
        }
    }
    return l;
}

/*! \brief can be used with qSort to sort <code>QList<QPair<T1, T2> ></code> for the first element of the pair in increasing order, i.e. compares with \c <
    \ingroup qf3lib_tools
*/
template <class T1, class T2>
bool qfQPairCompareFirst(const QPair<T1, T2> &s1, const QPair<T1, T2> &s2) {
    return s1.first < s2.first;
}

/*! \brief can be used with qSort to sort <code>QList<QPair<T1, T2> ></code> for the second element of the pair in increasing order, i.e. compares with \c <
    \ingroup qf3lib_tools
*/
template <class T1, class T2>
bool qfQPairCompareSecond(const QPair<T1, T2> &s1, const QPair<T1, T2> &s2) {
    return s1.second < s2.second;
}

/*! \brief can be used with qSort to sort <code>QList<QTriple<T1, T2, T3> ></code> for the first element of the triple in increasing order, i.e. compares with \c <
    \ingroup qf3lib_tools
*/
template <class T1, class T2, class T3>
bool qfQTripleCompareFirst(const QTriple<T1, T2, T3> &s1, const QTriple<T1, T2, T3> &s2) {
    return s1.first < s2.first;
}

/*! \brief can be used with qSort to sort <code>QList<QTriple<T1, T2, T3> ></code> for the second element of the triple in increasing order, i.e. compares with \c <
    \ingroup qf3lib_tools
*/
template <class T1, class T2, class T3>
bool qfQTripleCompareSecond(const QTriple<T1, T2, T3> &s1, const QTriple<T1, T2, T3> &s2) {
    return s1.second < s2.second;
}

/*! \brief can be used with qSort to sort <code>QList<QTriple<T1, T2, T3> ></code> for the third element of the triple in increasing order, i.e. compares with \c <
    \ingroup qf3lib_tools
*/
template <class T1, class T2, class T3>
bool qfQTripleCompareThird(const QTriple<T1, T2, T3> &s1, const QTriple<T1, T2, T3> &s2) {
    return s1.third < s2.third;
}

/*! \brief can be used with qSort to sort <code>QStringlist</code> for the length of the ebrties (decreasing order)
    \ingroup qf3lib_tools
*/
QFLIB_EXPORT bool qfQStringCompareLengthDecreasing(const QString &s1, const QString &s2);
/*! \brief can be used with qSort to sort <code>QStringlist</code> for the length of the ebrties (increasing order)
    \ingroup qf3lib_tools
*/
QFLIB_EXPORT bool qfQStringCompareLengthIncreasing(const QString &s1, const QString &s2);







/*! \brief can be used with qSort to sort <code>QList<QPair<T1, T2> ></code> for the first element of the pair in decreasing order, i.e. compares with \c >=
    \ingroup qf3lib_tools
*/
template <class T1, class T2>
bool qfQPairCompareFirstDec(const QPair<T1, T2> &s1, const QPair<T1, T2> &s2) {
    return s1.first >= s2.first;
}

/*! \brief can be used with qSort to sort <code>QList<QPair<T1, T2> ></code> for the second element of the pair in decreasing order, i.e. compares with \c >=
    \ingroup qf3lib_tools
*/
template <class T1, class T2>
bool qfQPairCompareSecondDec(const QPair<T1, T2> &s1, const QPair<T1, T2> &s2) {
    return s1.second >= s2.second;
}

/*! \brief can be used with qSort to sort <code>QList<QTriple<T1, T2, T3> ></code> for the first element of the triple in decreasing order, i.e. compares with \c >=
    \ingroup qf3lib_tools
*/
template <class T1, class T2, class T3>
bool qfQTripleCompareFirstDec(const QTriple<T1, T2, T3> &s1, const QTriple<T1, T2, T3> &s2) {
    return s1.first >= s2.first;
}

/*! \brief can be used with qSort to sort <code>QList<QTriple<T1, T2, T3> ></code> for the second element of the triple in decreasing order, i.e. compares with \c >=
    \ingroup qf3lib_tools
*/
template <class T1, class T2, class T3>
bool qfQTripleCompareSecondDec(const QTriple<T1, T2, T3> &s1, const QTriple<T1, T2, T3> &s2) {
    return s1.second >= s2.second;
}

/*! \brief can be used with qSort to sort <code>QList<QTriple<T1, T2, T3> ></code> for the third element of the triple in decreasing order, i.e. compares with \c >=
    \ingroup qf3lib_tools
*/
template <class T1, class T2, class T3>
bool qfQTripleCompareThirdDec(const QTriple<T1, T2, T3> &s1, const QTriple<T1, T2, T3> &s2) {
    return s1.third >= s2.third;
}





/*! \brief return a list of the values from an array \a data specified by a list of indexes \a selection
    \ingroup qf3lib_tools

    \note The parameter \a selection has to be QList<int> or QVector<int> (or a comparable list of ordinals/indexes).
          The minimum requirement is that the type T1 has a method \c size() and implements \c operator[](), The return
          value of the []-operator should be an index to data, i.e. a type like \c int, \c long, ... So a standard call
          would be with argument of type \c QList<int> or \c QVector<int>.
*/
template <class T1, class T2>
QList<T2> qfSelectFromVector(const T1& selection, T2* data) {
    QList<T2> res;
    for (int i=0; i<selection.size(); i++) {
        res<<data[selection[i]];
    }
    return res;
}

/*! \brief delete items in container (if \c !=NULL )
    \ingroup qf3lib_tools

*/
template <class T>
void qfClearContainer(T first, T last) {
    for (T it=first; it!=last; it++) {
        if (*it) delete *it;
    }
}

/*! \brief delete items in container (if \c !=NULL ) and clear container finally if \c clear==true
    \ingroup qf3lib_tools

*/
template <class T>
void qfClearContainer(T& container, bool clear=true) {
    qfClearContainer(container.begin(), container.end());
    if (clear) container.clear();
}



/*! \brief sort the vector \a data according to the permutation given in \a initialOrder and \newOrder

    After the call the element in \a data that was at position \c initialOder[0] will be at \c newOrder[0]
    and the element at \c initialOder[1] will be at \c newOrder[1] ...

    \note This is not really speed optimized. The parameters \a initialorder and neworder have to be QList<int> or QVector<int> (or a comparable list of ordinals/indexes).
          The minimum requirement is that the type T1 has a method \c size() and implements \c operator[](), The return
          value of the []-operator should be an index to data, i.e. a type like \c int, \c long, ... So a standard call
          would be with argument of type \c QList<int> or \c QVector<int>.


*/
template <class T1, class T2>
void qfSortVectorByPermutation(const T1 &initialOrder, const T1 &newOrder, T2* data) {
    T2* c=duplicateArray(data, qfstatisticsMax(initialOrder)+1);

    int N=qMin(initialOrder.size(), newOrder.size());
    for (int i=0; i<N; i++) {
        data[newOrder[i]]=c[initialOrder[i]];
    }

    qfFree(c);
}

template <typename T1, typename T2>
QList<T1> qfQPairListToFirstList(const QList<QPair<T1, T2> > &input) {
    QList<T1> res;
    for (int i=0; i<input.size(); i++) {
        res.append(input[i].first);
    }
    return res;
}

template <class T1, class T2>
QList<T2> qfQPairListToSecondList(const QList<QPair<T1, T2> > &input) {
    QList<T2> res;
    for (int i=0; i<input.size(); i++) {
        res.append(input[i].second);
    }
    return res;
}

template <class T1, class T2, class T3>
QList<T1> qfQTripleListToFirstList(const QList<QTriple<T1, T2, T3> > &input) {
    QList<T1> res;
    for (int i=0; i<input.size(); i++) {
        res.append(input[i].first);
    }
    return res;
}

template <class T1, class T2, class T3>
QList<T2> qfQTripleListToSecondList(const QList<QTriple<T1, T2, T3> > &input) {
    QList<T2> res;
    for (int i=0; i<input.size(); i++) {
        res.append(input[i].second);
    }
    return res;
}

template <class T1, class T2, class T3>
QList<T3> qfQTripleListToThirdList(const QList<QTriple<T1, T2, T3> > &input) {
    QList<T3> res;
    for (int i=0; i<input.size(); i++) {
        res.append(input[i].third);
    }
    return res;
}

enum QFImageHalf {
    qfihNone=-1,
    qfihLeft=0,
    qfihRight=1,
    qfihTop=2,
    qfihBottom=3,

    qfihAny=qfihNone
};

/*! \brief copy one half of an image
    \ingroup qf3lib_tools

    input has to be of size inWidth*inHeight, ahereas output is expected to be of size inWidth*inHeight/2!
*/
template <typename T1, typename T2>
void qfCopyImageHalf(T1* output, const T2* input, int inWidth, int inHeight, QFImageHalf imageHalf) {
    int width=inWidth;
    int height=inHeight;
    int shiftx=0;
    int shifty=0;
    bool ok=false;
    if (imageHalf==qfihLeft) {
        width=inWidth/2;
        height=inHeight;
        shiftx=0;
        shifty=0;
        ok=true;
    } else if (imageHalf==qfihRight) {
        width=inWidth/2;
        height=inHeight;
        shiftx=width;
        shifty=0;
        ok=true;
    } else if (imageHalf==qfihTop) {
        width=inWidth;
        height=inHeight/2;
        shiftx=0;
        shifty=0;
        ok=true;
    } else if (imageHalf==qfihBottom) {
        width=inWidth;
        height=inHeight/2;
        shiftx=0;
        shifty=height;
        ok=true;
    }
    for (int y=0; y<height; y++) {
        for (int x=0; x<width; x++) {
            const int idxIn=(y+shifty)*inWidth+x+shiftx;
            const int idxOut=y*width+x;
            output[idxOut]=input[idxIn];
        }
    }
}

/*! \brief converts a light wavelength (in nanometers) into a color representing this wavelength
    \ingroup qf3lib_tools

    This is based on code from:
       - http://www.efg2.com/Lab/ScienceAndEngineering/Spectra.htm
       - http://www.midnightkite.com/color.html
    .
 */
QFLIB_EXPORT QColor wavelengthToColor(double wavelength);


QFLIB_EXPORT QIcon qfGetColorIcon(QColor color, QSize size=QSize(16,16));

/*! \brief shorten the given string to the given maxLen and keeping \a keepend characters at the end
    \ingroup qf3lib_tools

 */
QFLIB_EXPORT QString qfShortenString(const QString& input, int maxLen=250, int keepend=0, const QString& ellipsis=QString("..."));




typedef double (*qfDoubleVectorToDoubleFunc)(const QVector<double>&);


/*! \brief group the data in \a input according to the labels given in \a index. Then return a vector where the function \a func is applied to every vector of values from \input, which all have the same index in \a index.
    \ingroup qf3lib_tools

 */
template <typename T, typename TIDX, typename TFUNC>
QVector<T> qfUniqueApplyFunction(const QVector<T>& input, const QVector<TIDX>& index, TFUNC func) {
    QVector<T> res;
    QMap<TIDX, QVector<T> > dataset;
    QList<TIDX> idxl;
    for (int i=0; i<qMin(input.size(), index.size()); i++) {
        dataset[index[i]].append(input[i]);
        if (!idxl.contains(index[i])) idxl<<index[i];
    }
    for (int i=0; i<idxl.size(); i++) {
        res<<func(dataset[idxl[i]]);
    }
    return res;
}

/*! \brief group the data in \a input according to the labels given in \a index. Then return a vector where the function \a func is applied to every vector of values from \input, which all have the same index in \a index.
    \ingroup qf3lib_tools

 */template <typename T, typename TIDX, typename TFUNC>
QList<T> qfUniqueApplyFunction(const QList<T>& input, const QList<TIDX>& index, TFUNC func) {
    QList<T> res;
    QMap<TIDX, QList<T> > dataset;
    QList<TIDX> idxl;
    for (int i=0; i<qMin(input.size(), index.size()); i++) {
        dataset[index[i]].append(input[i]);
        if (!idxl.contains(index[i])) idxl<<index[i];
    }
    for (int i=0; i<idxl.size(); i++) {
        res<<func(dataset[idxl[i]]);
    }
    return res;
}

/*! \brief group the data in \a input according to the labels given in \a index. Then return a vector where the function \a func is applied to every vector of values from \input, which all have the same index in \a index.
    \ingroup qf3lib_tools

 */template <typename T, typename TIDX, typename TFUNC>
QVector<T> qfUniqueApplyFunction(const QVector<T>& input, const QList<TIDX>& index, TFUNC func) {
    QVector<T> res;
    QMap<TIDX, QVector<T> > dataset;
    QList<TIDX> idxl;
    for (int i=0; i<qMin(input.size(), index.size()); i++) {
        dataset[index[i]].append(input[i]);
        if (!idxl.contains(index[i])) idxl<<index[i];
    }
    for (int i=0; i<idxl.size(); i++) {
        res<<func(dataset[idxl[i]]);
    }
    return res;
}


/*! \brief group the data in \a input according to the labels given in \a index. Then return a vector where the function \a func is applied to every vector of values from \input, which all have the same index in \a index.
    \ingroup qf3lib_tools

 */
template <typename T, typename TIDX, typename TFUNC, typename TFUNCPARAM>
QVector<T> qfUniqueApplyFunction(const QVector<T>& input, const QVector<TIDX>& index, TFUNC func, TFUNCPARAM funcParam) {
    QVector<T> res;
    QMap<TIDX, QVector<T> > dataset;
    QList<TIDX> idxl;
    for (int i=0; i<qMin(input.size(), index.size()); i++) {
        dataset[index[i]].append(input[i]);
        if (!idxl.contains(index[i])) idxl<<index[i];
    }
    for (int i=0; i<idxl.size(); i++) {
        res<<func(dataset[idxl[i]], funcParam);
    }
    return res;
}

/*! \brief group the data in \a input according to the labels given in \a index. Then return a vector where the function \a func is applied to every vector of values from \input, which all have the same index in \a index.
    \ingroup qf3lib_tools

 */
template <typename T, typename TIDX, typename TFUNC, typename TFUNCPARAM>
QList<T> qfUniqueApplyFunction(const QList<T>& input, const QList<TIDX>& index, TFUNC func, TFUNCPARAM funcParam) {
    QList<T> res;
    QMap<TIDX, QList<T> > dataset;
    QList<TIDX> idxl;
    for (int i=0; i<qMin(input.size(), index.size()); i++) {
        dataset[index[i]].append(input[i]);
        if (!idxl.contains(index[i])) idxl<<index[i];
    }
    for (int i=0; i<idxl.size(); i++) {
        res<<func(dataset[idxl[i]], funcParam);
    }
    return res;
}

/*! \brief group the data in \a input according to the labels given in \a index. Then return a vector where the function \a func is applied to every vector of values from \input, which all have the same index in \a index.
    \ingroup qf3lib_tools

 */
template <typename T, typename TIDX, typename TFUNC, typename TFUNCPARAM>
QVector<T> qfUniqueApplyFunction(const QVector<T>& input, const QList<TIDX>& index, TFUNC func, TFUNCPARAM funcParam) {
    QVector<T> res;
    QMap<TIDX, QVector<T> > dataset;
    QList<TIDX> idxl;
    for (int i=0; i<qMin(input.size(), index.size()); i++) {
        dataset[index[i]].append(input[i]);
        if (!idxl.contains(index[i])) idxl<<index[i];
    }
    for (int i=0; i<idxl.size(); i++) {
        res<<func(dataset[idxl[i]], funcParam);
    }
    return res;
}


















/*! \brief group the data in \a input and \a inputY according to the labels given in \a index. Then return a vector where the function \a func is applied to every vector of values from \a input and \a inputY, which all have the same index in \a index.
    \ingroup qf3lib_tools

 */
template <typename T, typename TIDX, typename TFUNC>
QVector<T> qfUniqueApplyFunction2I(const QVector<T>& input, const QVector<T>& inputY, const QVector<TIDX>& index, TFUNC func) {
    QVector<T> res;
    QMap<TIDX, QPair<QVector<T> , QVector<T> > > dataset;
    QList<TIDX> idxl;
    for (int i=0; i<qMin(input.size(), index.size()); i++) {
        dataset[index[i]].first.append(input[i]);
        dataset[index[i]].second.append(inputY[i]);
        if (!idxl.contains(index[i])) idxl<<index[i];
    }
    for (int i=0; i<idxl.size(); i++) {
        res<<func(dataset[idxl[i]].first, dataset[idxl[i]].second);
    }
    return res;
}

/*! \brief group the data in \a input and \a inputY according to the labels given in \a index. Then return a vector where the function \a func is applied to every vector of values from \a input and \a inputY, which all have the same index in \a index.
    \ingroup qf3lib_tools

 */template <typename T, typename TIDX, typename TFUNC>
QList<T> qfUniqueApplyFunction2I(const QList<T>& input, const QVector<T>& inputY, const QList<TIDX>& index, TFUNC func) {
    QList<T> res;
    QMap<TIDX, QPair<QVector<T> , QVector<T> > > dataset;
    QList<TIDX> idxl;
    for (int i=0; i<qMin(input.size(), index.size()); i++) {
        dataset[index[i]].first.append(input[i]);
        dataset[index[i]].second.append(inputY[i]);
        if (!idxl.contains(index[i])) idxl<<index[i];
    }
    for (int i=0; i<idxl.size(); i++) {
        res<<func(dataset[idxl[i]].first, dataset[idxl[i]].second);
    }
    return res;
}

/*! \brief group the data in \a input and \a inputY according to the labels given in \a index. Then return a vector where the function \a func is applied to every vector of values from \a input and \a inputY, which all have the same index in \a index.
    \ingroup qf3lib_tools

 */
template <typename T, typename TIDX, typename TFUNC>
QVector<T> qfUniqueApplyFunction2I(const QVector<T>& input, const QVector<T>& inputY, const QList<TIDX>& index, TFUNC func) {
    QVector<T> res;
    QMap<TIDX, QPair<QVector<T> , QVector<T> > > dataset;
    QList<TIDX> idxl;
    for (int i=0; i<qMin(input.size(), index.size()); i++) {
        dataset[index[i]].first.append(input[i]);
        dataset[index[i]].second.append(inputY[i]);
        if (!idxl.contains(index[i])) idxl<<index[i];
    }
    for (int i=0; i<idxl.size(); i++) {
        res<<func(dataset[idxl[i]].first, dataset[idxl[i]].second);
    }
    return res;
}

/*! \brief save the given string to a file
    \ingroup qf3lib_tools


 */
QFLIB_EXPORT void saveStringToFile(const QString& filename, const QString& text, const QString& codec=QString("ISO-8859-1"));
/*! \brief returns a list of all files in a directory, recursively
    \ingroup qf3lib_tools


 */
QFLIB_EXPORT QStringList qfDirListFilesRecursive(QDir& dir, const QStringList& filters=QStringList());

/*! \brief extract FAQs from a faq.html file
    \ingroup qf3lib_tools


 */
QFLIB_EXPORT void parseFAQ(const QString& filename, const QString& pluginID, QMap<QString, QFFAQData> &faqs);

QFLIB_EXPORT void parseTooltips(const QString& directory, QMap<QString, QFToolTipsData>& tooltips);

QFLIB_EXPORT void parseAutolinks(const QString& directory, QMap<QString, QString>& autolinks);

QFLIB_EXPORT void parseGlobalreplaces(const QString& directory);
/*! \brief returns a filename for a temporary file
    \ingroup qf3lib_tools


 */
QFLIB_EXPORT QString qfGetTempFilename(const QString& templateName=QString("qf_temp_XXXXXX"));

/*! \brief copies data (in chunks) from one file to another
    \ingroup qf3lib_tools


 */
QFLIB_EXPORT bool qfCopyData( QIODevice* in, QIODevice* out);



class QFLIB_EXPORT QFProgressMinorProgress {
    public:
        virtual ~QFProgressMinorProgress() {}
        virtual void setMinorProgress(int value)=0;
        virtual void setMinorProgressRange(int min=0, int max=100)=0;
        virtual bool wasMinorProgressCanceled() const=0;
};

/*! \brief copies data (in chunks) from one file to another
    \ingroup qf3lib_tools


 */
QFLIB_EXPORT bool qfCopyData( QIODevice* in, QIODevice* out, QFProgressMinorProgress* pdlg);

/*! \brief reads the contents of the given file and compares it to the given string. Returns \c true, if the two are equal
    \ingroup qf3lib_tools


 */
QFLIB_EXPORT bool qfFileEqualsString( const QString& filename, const QByteArray& data);

/*! \brief escape \\n, \\r, \\", \\', \\\\, \\t...
    \ingroup qf3lib_tools


 */
QFLIB_EXPORT QString qfCEscaped(const QString& data);


/*! \brief return a shuffled version of a given vector
    \ingroup qf3lib_tools

*/
template <typename T>
QVector<T> qfShuffledVector(const QVector<T>& value) {
    std::vector<T> mm=value.toStdVector();
    std::random_shuffle( mm.begin(), mm.end() );
    return QVector<T>::fromStdVector(mm);
}

/*! \brief return a shuffled version of a given list
    \ingroup qf3lib_tools

*/
template <typename T>
QList<T> qfShuffledVector(const QList<T>& value) {
   std::vector<T> mm;
   mm.resize(value.size());
   for (int i=0; i<value.size(); i++) {
       mm[i]=value[i];
   }
   std::random_shuffle( mm.begin(), mm.end() );

   QList<T> ret;
   ret.reserve(mm.size());
   for (size_t i=0; i<mm.size(); i++) {
       ret.append(mm[i]);
   }
   return ret;
}

/*! \brief shuffle a given list
    \ingroup qf3lib_tools

*/
template <typename T>
void qfShuffleInplace(QList<T>& value) {
   std::vector<T> mm;
   mm.resize(value.size());
   for (int i=0; i<value.size(); i++) {
       mm[i]=value[i];
   }
   std::random_shuffle( mm.begin(), mm.end() );

   for (size_t i=0; i<mm.size(); i++) {
       value[i]=mm[i];
   }
}

/*! \brief shuffle a given list
    \ingroup qf3lib_tools

*/
template <typename T>
void qfShuffleInplace(QVector<T>& value) {
   std::vector<T> mm;
   mm.resize(value.size());
   for (int i=0; i<value.size(); i++) {
       mm[i]=value[i];
   }
   std::random_shuffle( mm.begin(), mm.end() );

   for (size_t i=0; i<mm.size(); i++) {
       value[i]=mm[i];
   }
}



#endif // QFTOOLS_H



