#ifndef QFCOMPLETERPROPERTIES_H
#define QFCOMPLETERPROPERTIES_H

#include <QCompleter>
#include "lib_imexport.h"
#include "qfproject.h"
#include <QStringListModel>
#include <QPointer>

/** \brief completer class that may complete a list of all currently available properties
 *  \ingroup qf3lib_widgets
 */
class QFLIB_EXPORT QFCompleterProperties : public QCompleter
{
        Q_OBJECT
    public:
        QFCompleterProperties(QObject * parent = 0);
        virtual ~QFCompleterProperties();
        /** \brief return the QStringListModel that holds the completer list */
        inline QStringListModel* stringlistModel() const { return m_stringlist; }
        inline QFProject* project() const { return m_project; }
        void setProject(QFProject* project);
    protected:
    private:
        QPointer<QFProject> m_project;
        QStringListModel* m_stringlist;
    protected slots:
        void propertiesChanged();

};

#endif // QFCOMPLETERPROPERTIES_H
