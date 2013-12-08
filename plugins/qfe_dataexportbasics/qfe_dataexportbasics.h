#ifndef QFEDATAEXPORTBASICS_H
#define QFEDATAEXPORTBASICS_H

#include <time.h>
#include <QObject>
#include "qfextension.h"

/*!
    \defgroup qf3ext_qfe_dataexportbasics QFExtension implementation
    \ingroup qf3extensionplugins
*/

/*! \brief QFExtension implementation
    \ingroup qf3ext_qfe_dataexportbasics
 */
class QFEDataExportBasics : public QObject, public QFExtensionBase {
        Q_OBJECT
        Q_INTERFACES(QFExtension)
    public:
        /** Default constructor */
        QFEDataExportBasics(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFEDataExportBasics();


    /////////////////////////////////////////////////////////////////////////////
    // QFExtension routines
    /////////////////////////////////////////////////////////////////////////////
        /** \copydoc QFExtension::getID() */
        virtual QString getID() const  { return QString("qfe_dataexportbasics"); };
        /** \copydoc QFExtension::getName() */
        virtual QString getName() const  { return tr("data export basics plugin"); };
        /** \copydoc QFExtension::getDescription() */
        virtual QString getDescription() const  { return tr("allows to export QuickFit table contents to several data formats (Excel, ...)"); };
        /** \copydoc QFExtension::getAuthor() */
        virtual QString getAuthor() const  { return tr("Jan W. Krieger"); };
        /** \copydoc QFExtension::getCopyright() */
        virtual QString getCopyright() const  { return tr("(c) 2013 by Jan W. Krieger"); };
        /** \copydoc QFExtension::getWeblink() */
        virtual QString getWeblink() const  { return tr("http://www.dkfz.de/Macromol/quickfit/"); };
        /** \copydoc QFExtension::getIconFilename() */
        virtual QString getIconFilename() const  { return QString(":/qfe_dataexportbasics/qfe_dataexportbasics.png"); };
        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=0;
        };
        /** \copydoc QFExtension::deinit() */
        virtual void deinit();

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
		
	protected slots:
	    /** \brief target, used in example code in initExtension() */
	    //void startPlugin();

};

#endif // QFEDATAEXPORTBASICS_H
