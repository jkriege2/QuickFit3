#ifndef QFESPECTRAVIEWER_H
#define QFESPECTRAVIEWER_H

#include <time.h>
#include <QObject>
#include "qfextension.h"
#include "qfpluginoptionsdialog.h"

/*!
    \defgroup qf3ext_qfe_spectraviewer QFExtension implementation
    \ingroup qf3extensionplugins
*/
class SpectrumManager;
class QFESpectraViewerDialog;

/*! \brief QFExtension implementation
    \ingroup qf3ext_qfe_spectraviewer
 */
class QFESpectraViewer : public QObject, public QFExtensionBase, public QFPluginOptionsDialogInterface {
        Q_OBJECT
        Q_INTERFACES(QFExtension QFPluginOptionsDialogInterface)
    public:
        /** Default constructor */
        QFESpectraViewer(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFESpectraViewer();


    /////////////////////////////////////////////////////////////////////////////
    // QFExtension routines
    /////////////////////////////////////////////////////////////////////////////
        /** \copydoc QFExtension::getID() */
        virtual QString getID() const  { return QString("qfe_spectraviewer"); };
        /** \copydoc QFExtension::getName() */
        virtual QString getName() const  { return tr("Spectra Viewer"); };
        /** \copydoc QFExtension::getDescription() */
        virtual QString getDescription() const  { return tr("displays fluorophore and filter spectra"); };
        /** \copydoc QFExtension::getAuthor() */
        virtual QString getAuthor() const  { return tr("Jan W. Krieger"); };
        /** \copydoc QFExtension::getCopyright() */
        virtual QString getCopyright() const  { return tr("(c) 2013 by Jan W. Krieger"); };
        /** \copydoc QFExtension::getWeblink() */
        virtual QString getWeblink() const  { return tr("http://www.dkfz.de/Macromol/quickfit/"); };
        /** \copydoc QFExtension::getIconFilename() */
        virtual QString getIconFilename() const  { return QString(":/qfe_spectraviewer/qfespectraviewer.png"); };
        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=0;
        };
        /** \copydoc QFExtension::deinit() */
        virtual void deinit();


        QString pluginOptionsName() const;
        QIcon pluginOptionsIcon() const;
        QFPluginOptionsWidget *createOptionsWidget(QWidget *parent);


        SpectrumManager* getSpectrumManager() const;

    protected:
        /** \copydoc QFExtensionBase::projectChanged() */
        virtual void projectChanged(QFProject* oldProject, QFProject* project);
        /** \copydoc QFExtensionBase::initExtension() */
        virtual void initExtension();
        /** \copydoc QFExtensionBase::loadSettings() */
        virtual void loadSettings(ProgramOptions* settings);
        /** \copydoc QFExtensionBase::storeSettings() */
        virtual void storeSettings(ProgramOptions* settings);



        /** \brief log project text message
         *  \param message the message to log
         */
        virtual void log_text(QString message);
        /** \brief log project warning message
         *  \param message the warning message to log
         */
        virtual void log_warning(QString message);
        /** \brief log project error message
         *  \param message the error message to log
         */
        virtual void log_error(QString message);

	protected:
        QFPluginLogService* logService;
        QFESpectraViewerDialog* dlg;
        SpectrumManager* manager;
		
	protected slots:
	    /** \brief target, used in example code in initExtension() */
        void showViewer();

};

#endif // QFESPECTRAVIEWER_H
