#ifndef QFPLUGINSERVICES_H
#define QFPLUGINSERVICES_H

#include <QString>
#include <QSettings>
#include "qffitalgorithmmanager.h"
#include "qffitfunctionmanager.h"
#include "programoptions.h"
#include "qfextensionmanager.h"

/*! \brief wrapper class that allows plugins to access basic logging services
    \ingroup qf3plugintools
*/
class QFPluginLogService {
    public:
        /** Default destructor */
        virtual ~QFPluginLogService() {}

        /** \brief indent all following lines in the logging pane */
        //virtual void log_indent()=0;
        /** \brief undo former log_indent() */
        //virtual void log_unindent()=0;

        /** \brief log project text message
         *  \param message the message to log
         */
        virtual void log_text(QString message)=0;
        /** \brief log project warning message
         *  \param message the warning message to log
         */
        virtual void log_warning(QString message)=0;
        /** \brief log project error message
         *  \param message the error message to log
         */
        virtual void log_error(QString message)=0;
};

/*! \brief wrapper class that allows plugins to access basic services of the QuickFit application
    \ingroup qf3plugintools
*/
class QFPluginServices {
    public:
        /** Default destructor */
        virtual ~QFPluginServices() {}

        /** \brief indent all following lines in the logging pane */
        virtual void log_indent()=0;
        /** \brief undo former log_indent() */
        virtual void log_unindent()=0;

        /** \brief log project text message
         *  \param message the message to log
         */
        virtual void log_text(QString message)=0;
        /** \brief log project warning message
         *  \param message the warning message to log
         */
        virtual void log_warning(QString message)=0;
        /** \brief log project error message
         *  \param message the error message to log
         */
        virtual void log_error(QString message)=0;
        /** \brief set status message */
        virtual void setStatusMessage(QString message)=0;
        /** \brief set range of progress bar
         *  \param minP minimum of progress range
         *  \param maxP maximum of progress range
         */
        virtual void setProgressRange(int minP, int maxP)=0;
        /** \brief set progress barto \a value
         *  \param value the progress value (in the range given by setProgressRange() )
         */
        virtual void setProgress(int value)=0;
        /** increment progress bar by 1  */
        virtual void incProgress()=0;
        /** \brief return a pointer to a fit function manager object */
        virtual QFFitFunctionManager* getFitFunctionManager()=0;
        /** \brief return a pointer to a fit function manager object */
        virtual QFFitAlgorithmManager* getFitAlgorithmManager()=0;
        /** \brief return a pointer to a valid QSettings object for the main settings INI file */
        virtual QSettings* getSettings()=0;
        /** \brief return a pointer to the applications ProgramOptions object */
        virtual ProgramOptions* getOptions()=0;
};



/*! \brief wrapper class that allows plugins to access basic \& advanced services of the QuickFit application, extends QFPluginServices
    \ingroup qf3plugintools
*/
class QFExtensionServices: public QFPluginServices {
    public:
        /** \brief class destructor */
        ~QFExtensionServices() {};

        /** \brief indent all following lines in the global logging pane */
        virtual void log_global_indent()=0;
        /** \brief undo former log_global_indent() */
        virtual void log_global_unindent()=0;

        /** \brief log global text message
         *  \param message the message to log
         */
        virtual void log_global_text(QString message)=0;
        /** \brief log global warning message
         *  \param message the warning message to log
         */
        virtual void log_global_warning(QString message)=0;
        /** \brief log global error message
         *  \param message the error message to log
         */
        virtual void log_global_error(QString message)=0;

        /*! \brief return a pointer to a given menu, or NULL

            \param menu the menu to return. Some common menus are predefined: \c \"file\", \c \"data\",
                   \c \"help\", \c \"extensions\"
            \return a pointer to the given menu, or NULL if an invalid \a menu name was given.


         */
        virtual QMenu* getMenu(QString menu)=0;

        /*! \brief return a pointer to a given toolbar, or NULL

            \param toolbar the toolbar to return. Some common toolbars are predefined: \c \"file\", \c \"data\",
                   \c \"extensions\"
            \return a pointer to the given toolbar, or NULL if an invalid \a toolbar name was given.

         */
        virtual QToolBar* getToolbar(QString toolbar)=0;

        /** \brief insert a new menu on the right of \a before, or at the left border when \c before=NULL
         *  \param menuname a name designator used to access the menu later on by getMenu()
         *  \param newMenu the menu to insert
         *  \param before insert \a newMenu before this menu
         */
        virtual void insertMenu(QString menuname, QMenu* newMenu, QMenu* before=NULL)=0;

        /** \brief insert a new menu on the right of \a before, or at the left border when \c before=NULL
         *  \param toolbarname a name designator used to access the toolbar later on by getToolbar()
         *  \param newToolbar the toolbar to insert
         */
        virtual void insertToolBar(QString toolbarname, QToolBar* newToolbar)=0;

        /** \brief returns a QFExtensionManager object that allows access to all currently loaded QFExtensions */
        virtual QFExtensionManager* getExtensionManager()=0;
};


#endif // QFPLUGINSERVICES_H
