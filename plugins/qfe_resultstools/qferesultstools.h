#ifndef QFERESULTSTOOLS_H
#define QFERESULTSTOOLS_H

#include <time.h>
#include <QObject>
#include "qfextension.h"
#include "qfertresultcalculator.h"
/*!
    \defgroup qf3ext_qfe_resultstools QFExtension implementation
    \ingroup qf3extensionplugins
*/

/*! \brief QFExtension implementation
    \ingroup qf3ext_qfe_resultstools
 */
class QFEResultsTools : public QObject, public QFExtensionBase {
        Q_OBJECT
        Q_INTERFACES(QFExtension)
    public:
        /** Default constructor */
        QFEResultsTools(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFEResultsTools();


    /////////////////////////////////////////////////////////////////////////////
    // QFExtension routines
    /////////////////////////////////////////////////////////////////////////////
        /** \copydoc QFExtension::getID() */
        virtual QString getID() const  { return QString("qfe_resultstools"); };
        /** \copydoc QFExtension::getName() */
        virtual QString getName() const  { return tr("Results Tools (Calculator ...)"); };
        /** \copydoc QFExtension::getDescription() */
        virtual QString getDescription() const  { return getName(); };
        /** \copydoc QFExtension::getAuthor() */
        virtual QString getAuthor() const  { return tr("Jan W. Krieger"); };
        /** \copydoc QFExtension::getCopyright() */
        virtual QString getCopyright() const  { return tr("(c)2013 by Jan W. Krieger"); };
        /** \copydoc QFExtension::getWeblink() */
        virtual QString getWeblink() const  { return tr("http://www.dkfz.de/Macromol/quickfit/"); };
        /** \copydoc QFExtension::getIconFilename() */
        virtual QString getIconFilename() const  { return QString(":/qfe_resultstools/qferesultstools.png"); };
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
        QFERTResultCalculator* dlgCalc;
		
	protected slots:
	    /** \brief target, used in example code in initExtension() */
        void startResultsCalculator();

};

#endif // QFERESULTSTOOLS_H
