#ifndef SPIMB040_H
#define SPIMB040_H

#include "qfextension.h"
#include "qfespimb040mainwindow.h"
#include "qfespimb040mainwindow2.h"
#include <QAction>

/*!
    \defgroup qf3ext_spimb040 SPIM Control Extension (B040, DKFZ Heidelberg)
    \ingroup qf3extensionplugins
*/

/*! \brief SPIM Control Extension (B040, DKFZ Heidelberg) plugin class
    \ingroup qf3ext_spimb040
 */
class QFESPIMB040 : public QObject, public QFExtensionBase {
        Q_OBJECT
        Q_INTERFACES(QFExtension)
    public:
        /** Default constructor */
        QFESPIMB040(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFESPIMB040();

        /** \copydoc QFExtension::getID() */
        virtual QString getID() const  { return tr("ext_spimb040"); };

        /** \copydoc QFExtension::getName() */
        virtual QString getName() const  { return tr("B040 SPIM Control Extension"); };

        /** \copydoc QFExtension::getDescription() */
        virtual QString getDescription() const  { return tr("controls the SPIM microscope in dep. B040 of DKFZ, Heidelberg"); };

        /** \copydoc QFExtension::getAuthor() */
        virtual QString getAuthor() const  { return tr("Jan Krieger"); };

        /** \copydoc QFExtension::getCopyright() */
        virtual QString getCopyright() const  { return tr("(c) 2011 by Jan Krieger"); };

        /** \copydoc QFExtension::getWeblink() */
        virtual QString getWeblink() const  { return tr(""); };

        /** \copydoc QFExtension::getIconFilename() */
        virtual QString getIconFilename() const  { return QString(":/spimb040_logo.png"); };
        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=0;
        };


        /** \copydoc QFExtension::deinit() */
        virtual void deinit();

    protected:
        QAction* actStartPlugin;
        QAction* actStartPluginOld;

        QFESPIMB040MainWindow2* main;
        QFESPIMB040MainWindow* main_old;

        /** \copydoc QFExtensionBase::projectChanged() */
        virtual void projectChanged(QFProject* oldProject, QFProject* project);

        /** \copydoc QFExtensionBase::initExtension() */
        virtual void initExtension();

        /** \copydoc QFExtensionBase::loadSettings() */
        virtual void loadSettings(ProgramOptions* settings);

        /** \copydoc QFExtensionBase::storeSettings() */
        virtual void storeSettings(ProgramOptions* settings);
    protected slots:
        void startPlugin();
        void startPluginOld();
};

#endif // SPIMB040_H
