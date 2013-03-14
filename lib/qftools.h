#ifndef QFTOOLS_H
#define QFTOOLS_H

#include "qfmathtools.h"

#include <cmath>
#include <cfloat>
#include <QSettings>
#include <QWidget>
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
QFLIB_EXPORT QString doubleToQString(double value, int prec = 10, char f = 'g', QChar decimalSeparator='.' );
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

/*! \brief convert the given string to a string suitable for use in filenames
    \ingroup qf3lib_tools
*/
QFLIB_EXPORT QString cleanStringForFilename(const QString& text, int maxLen=127, bool removeDot=true, bool removeSlash=true);

/*! \brief convert the given string to a string suitable for use in filenames
    \ingroup qf3lib_tools
*/
QFLIB_EXPORT QListWidgetItem* addCheckableQListWidgetItem(QListWidget* listWidget, const QString& text, Qt::CheckState checked=Qt::Unchecked, const QVariant& userData=QVariant());


/*! \brief remove all actions from a QWidget
    \ingroup qf3lib_tools
*/
QFLIB_EXPORT void removeAllActions(QWidget* widget);

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

/*! \brief duplicate a memory array, i.e. reserve the data of the array a second time using malloc() and copy the contents
    \ingroup qf3lib_tools

*/
template <class T>
T* duplicateArray(const T* input, long long N) {
    T* out=(T*)malloc(N*sizeof(T));
    memcpy(out, input, N*sizeof(T));
    return out;
}
#endif // QFTOOLS_H
