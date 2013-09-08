#ifndef QFMANYFILESSETTINGS_H
#define QFMANYFILESSETTINGS_H
#include "lib_imexport.h"
#include <QSettings>
#include <QString>
#include <QList>
#include <QVariant>
#include <QPointer>
#include <QSet>
#include <QSplitter>
/*! \brief this class accesses a hirarchy of settings files
    \ingroup qf3lib_tools

    This class manages a list of QSettings objects. When a key is accessed for read, the files are searched top-down
    (in reverse order of adding) for the value. Writing happens bottom up, caring for readonly file.

    E.g.: You may have:
       - a readonly system-global settings file A
       - a writeable automatic config file B
       - a writeable user-specified config file C
    .

    When \b reading now a \c key, it is search in the order <code>C -> B -> A</code> The first match is returned. If no
    match was found, a \c default value is returned.

    When \b writing a \c key, \c value pair, we first try to find it in the order <code>C -> B -> A</code> as for reading.
    The first occurence is replaced with the new value, if writing is possible, if not we try to write in ascending order.
    If the \c key is new to all levels, the first attempt is to write into A (it's read-only, so this fails). Then we
    try to write into B, if this succeeds, we stop, if not, we write into C ...
  */

class QFLIB_EXPORT QFManyFilesSettings: public QObject {
        Q_OBJECT
    public:
        QFManyFilesSettings(QObject* parent=NULL);
        virtual ~QFManyFilesSettings();

        void addSettings(QSettings* settings, bool takeOwnership=true, bool readony=false);
        void clear(bool deleteSettings=true);
        QSettings* getSettings(int level);

        QVariant value(const QString& key, const QVariant& defaultValue=QVariant(), int* level=NULL) const;
        bool setValue(const QString& key, const QVariant& value, int* level=NULL);

        QStringList childGroups() const;
        void beginGroup(const QString& prefix);
        void endGroup();

        int levels() const;

    protected:
        QList<QPointer<QSettings> > settings;
        QSet<QSettings*> readonly;
};

QFLIB_EXPORT void jksaveWidgetGeometry(QFManyFilesSettings& settings, QWidget* widget, QString prefix);
QFLIB_EXPORT void jkloadWidgetGeometry(QFManyFilesSettings& settings, QWidget* widget, QPoint defaultPosition, QSize defaultSize, QString prefix);
QFLIB_EXPORT void jkloadWidgetGeometry(QFManyFilesSettings& settings, QWidget* widget, QString prefix);
QFLIB_EXPORT void jksaveSplitter(QFManyFilesSettings& settings, QSplitter* splitter, QString prefix);
QFLIB_EXPORT void jkloadSplitter(QFManyFilesSettings& settings, QSplitter* splitter, QString prefix);

#endif // QFMANYFILESSETTINGS_H
