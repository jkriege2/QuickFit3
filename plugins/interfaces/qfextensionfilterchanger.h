#ifndef QFEXTENSIONFILTERCHANGER_H
#define QFEXTENSIONFILTERCHANGER_H


#include <QtPlugin>
#include <QWidget>
#include <QSettings>
#include "qfpluginservices.h"
#include <stdint.h>



/*! \brief QuickFit QFExtension to control filter wheels and comparable devices
    \ingroup qf3extensionplugins


 */
class QFExtensionFilterChanger {
    public:
        virtual ~QFExtensionFilterChanger() {}

        /** \brief returns the number of filter wheels controlable by this plugin */
        virtual unsigned int  getFilterChangerCount()  =0;
        /** \brief connect to a filter wheel controller/driver/.... */
        virtual void filterChangerConnect(unsigned int filterChanger)=0;
        /** \brief disconnect from a filter wheel controller/driver/.... */
        virtual void filterChangerDisonnect(unsigned int filterChanger)=0;
        /** \brief set QFPluginLogServices to use (or \c NULL) for message/error logging */
        virtual void setFilterChangerLogging(QFPluginLogService* logService)=0;

        /** \brief returns \c true if the device is connected */
        virtual bool isFilterChangerConnected(unsigned int filterChanger) =0;

        /** \brief return the number of filters selctable by the filter wheel */
        virtual unsigned int getFilterChangerFilterCount(unsigned int filterChanger) =0;

        /** \brief set filter in the given filter wheel */
        virtual void setFilterChangerFilter(unsigned int filterChanger, unsigned int filter) =0;

        /** \brief return the currently selected filter */
        virtual unsigned int getFilterChangerCurrentFilter(unsigned int filterChanger) =0;

        /** \brief return \c true if the last command, sent to the given filter wheel was executet, i.e. the filter wheel has settled to its new position
         *
         *  \note Use this to check, whether the filter wheel has reached its target position after a call of setFilterChangerFilter(), as the second
         *        one might not be defined (and return a wrong value) during the filter wheel movement!
         */
        virtual bool isLastFilterChangerActionFinished(unsigned int filterChanger)=0;
        /** \brief return a human-readable description for the given filter wheel */
        virtual QString getFilterChangerDescription(unsigned int filterChanger) =0;
        /** \brief return a human-readable short name for the given filter wheel */
        virtual QString getFilterChangerShortName(unsigned int filterChanger) =0;
        /*! \brief displays a modal dialog which allows the user to set the configuration options
                   of the controled filter wheel.

            The options are stored internally and written/read using QFExtension::readSettings() and
            QFExtension::writeSettings().

            \param[in] filterChanger the filter wheel for which to display the dialog
            \param[in] parent parent widget for the returned QWidget
         */
         virtual void showFilterChangerSettingsDialog(unsigned int filterChanger, QWidget* parent=NULL)=0;
};



Q_DECLARE_INTERFACE( QFExtensionFilterChanger,
                     "www.dkfz.de.b040.quickfit3.extensions.QFExtensionFilterChanger/1.0")

#endif // QFEXTENSIONFILTERCHANGER_H
