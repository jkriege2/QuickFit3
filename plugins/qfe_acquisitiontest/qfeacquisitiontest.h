#ifndef QFEACQUISITIONTEST_H
#define QFEACQUISITIONTEST_H

#include <time.h>
#include <QObject>
#include "qfextension.h"
#include "qfeacquisitiontestdialog.h"
/*!
    \defgroup qf3ext_qfe_acquisitiontest QFExtension implementation
    \ingroup qf3extensionplugins
*/

/*! \brief QFExtension implementation
    \ingroup qf3ext_qfe_acquisitiontest
 */
class QFEAcquisitionTest : public QObject, public QFExtensionBase {
        Q_OBJECT
        Q_INTERFACES(QFExtension)
    public:
        /** Default constructor */
        QFEAcquisitionTest(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFEAcquisitionTest();


    /////////////////////////////////////////////////////////////////////////////
    // QFExtension routines
    /////////////////////////////////////////////////////////////////////////////
        /** \copydoc QFExtension::getID() */
        virtual QString getID() const  { return QString("qfe_acquisitiontest"); };
        /** \copydoc QFExtension::getName() */
        virtual QString getName() const  { return tr("simple acquisitions from QFExtensionAcquisition plugins"); };
        /** \copydoc QFExtension::getDescription() */
        virtual QString getDescription() const  { return getName(); };
        /** \copydoc QFExtension::getAuthor() */
        virtual QString getAuthor() const  { return tr("Jan W. Krieger"); };
        /** \copydoc QFExtension::getCopyright() */
        virtual QString getCopyright() const  { return tr("(c) 2012 by Jan W. Krieger"); };
        /** \copydoc QFExtension::getWeblink() */
        virtual QString getWeblink() const  { return tr("http://www.dkfz.de/Macromol/quickfit/"); };
        /** \copydoc QFExtension::getIconFilename() */
        virtual QString getIconFilename() const  { return QString(":/qfe_acquisitiontest/qfeacquisitiontest.png"); };
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

        QFEAcquisitiontestDialog* dialog;
		
	protected slots:
	    /** \brief target, used in example code in initExtension() */
        void startPlugin();

};

#endif // QFEACQUISITIONTEST_H
