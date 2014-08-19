#ifndef QFHELPTOOLBUTTON_H
#define QFHELPTOOLBUTTON_H

#include <QToolButton>
#include "lib_imexport.h"


class QFLIB_EXPORT QFHelpToolButton : public QToolButton
{
        Q_OBJECT
    public:
        explicit QFHelpToolButton(QWidget *parent = 0);
        explicit QFHelpToolButton(const QString& plugin, const QString& filename, QWidget *parent = 0);
        explicit QFHelpToolButton(const QString& filename, QWidget *parent = 0);
        /** \brief link to a help page of a QuickFit plugin (located in plugin help dir) */
        void setHelp(const QString& plugin, const QString& filename);
        /** \brief link to a help page of QuickFit (located in main help dir) */
        void setHelp(const QString& filename);
    signals:

    public slots:
        void showHelp();

    protected:
        QString m_plugin;
        QString m_filename;
    private:
        void initWidget();

};

#endif // QFHELPTOOLBUTTON_H
