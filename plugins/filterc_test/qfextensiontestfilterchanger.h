#ifndef QFEXTENSIONTESTFILTERCHANGER_H
#define QFEXTENSIONTESTFILTERCHANGER_H

#include <time.h>
#include <QObject>
#include "qfextension.h"
#include "qfextensionfilterchanger.h"

/*!
    \defgroup qf3ext_filterc_test QFExtensioFilterChanger test device
    \ingroup qf3extensionplugins
*/

/*! \brief QFExtensioFilterChanger test device implementation
    \ingroup qf3ext_filterc_test
 */
class QFExtensionTestFilterChanger : public QObject, public QFExtensionBase, public QFExtensionFilterChanger {
        Q_OBJECT
        Q_INTERFACES(QFExtension QFExtensionFilterChanger)
    public:
        /** Default constructor */
        QFExtensionTestFilterChanger(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFExtensionTestFilterChanger();


    /////////////////////////////////////////////////////////////////////////////
    // QFExtension routines
    /////////////////////////////////////////////////////////////////////////////
        /** \copydoc QFExtension::getID() */
        virtual QString getID() const  { return QString("filterc_test"); };
        /** \copydoc QFExtension::getName() */
        virtual QString getName() const  { return tr("Test Filter Changer"); };
        /** \copydoc QFExtension::getDescription() */
        virtual QString getDescription() const  { return tr("test filter changer device"); };
        /** \copydoc QFExtension::getAuthor() */
        virtual QString getAuthor() const  { return tr("Jan Krieger"); };
        /** \copydoc QFExtension::getCopyright() */
        virtual QString getCopyright() const  { return tr("(c) 2012 by J. Krieger"); };
        /** \copydoc QFExtension::getWeblink() */
        virtual QString getWeblink() const  { return tr("http://www.dkfz.de/Macromol/quickfit/"); };
        /** \copydoc QFExtension::getIconFilename() */
        virtual QString getIconFilename() const  { return QString(":/filterc_test/qfextensiontestfilterchanger.png"); };
        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=0;
        };
        /** \copydoc QFExtension::deinit() */
        virtual void deinit();














        /** \copydoc QFExtensionFilterChanger::getFilterChangerCount() */
        virtual unsigned int getFilterChangerCount()  ;
        /** \copydoc QFExtensionFilterChanger::filterChangerConnect() */
        virtual void filterChangerConnect(unsigned int filterChanger);
        /** \copydoc QFExtensionFilterChanger::filterChangerDisonnect() */
        virtual void filterChangerDisonnect(unsigned int filterChanger);
        /** \copydoc QFExtensionFilterChanger::setFilterChangerLogging() */
        virtual void setFilterChangerLogging(QFPluginLogService* logService);

        /** \copydoc QFExtensionFilterChanger::isFilterChangerConnected() */
        virtual bool isFilterChangerConnected(unsigned int filterChanger) ;

        /** \copydoc QFExtensionFilterChanger::getFilterChangerFilterCount() */
        virtual unsigned int getFilterChangerFilterCount(unsigned int filterChanger) ;

        /** \copydoc QFExtensionFilterChanger::setFilterChangerFilter() */
        virtual void setFilterChangerFilter(unsigned int filterChanger, unsigned int filter) ;

        /** \copydoc QFExtensionFilterChanger::getFilterChangerCurrentFilter() */
        virtual unsigned int getFilterChangerCurrentFilter(unsigned int filterChanger) ;

        /** \copydoc QFExtensionFilterChanger::isLastFilterChangerActionFinished() */
        virtual bool isLastFilterChangerActionFinished(unsigned int filterChanger);
        /** \copydoc QFExtensionFilterChanger::getFilterChangerDescription() */
        virtual QString getFilterChangerDescription(unsigned int filterChanger) ;
        /** \copydoc QFExtensionFilterChanger::getFilterChangerShortName() */
        virtual QString getFilterChangerShortName(unsigned int filterChanger) ;
        /** \copydoc QFExtensionFilterChanger::showFilterChangerSettingsDialog() */
        virtual void showFilterChangerSettingsDialog(unsigned int filterChanger, QWidget* parent=NULL);
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
        bool connected[2];
        unsigned int filter[2];
        QTime lastAction[2];

};

#endif // QFEXTENSIONTESTFILTERCHANGER_H
