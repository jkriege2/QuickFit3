#ifndef QFHELPACTION_H
#define QFHELPACTION_H

#include <QAction>
#include "lib_imexport.h"

class QFLIB_EXPORT QFHelpAction : public QAction
{
        Q_OBJECT
    public:
        explicit QFHelpAction(QObject *parent = 0);
        explicit QFHelpAction(const QString& plugin, const QString& filename, QObject *parent = 0);
        explicit QFHelpAction(const QString& filename, QObject *parent = 0);
        /** \brief link to a help page of a QuickFit plugin (located in plugin help dir) */
        void setHelp(const QString& plugin, const QString& filename);
        /** \brief link to a help page of QuickFit (located in main help dir) */
        void setMainHelp(const QString& filename);
        void setHelp(const QString& filename);
    public slots:
        void showHelp();

    signals:

    public slots:
    protected:
        QString m_plugin;
        QString m_filename;
        bool mainHelp;

};

#endif // QFHELPACTION_H
