#ifndef QFESPIMB040MAINWINDOW_H
#define QFESPIMB040MAINWINDOW_H

#include <QWidget>
#include <QSplitter>
#include <QWidget>
#include <QComboBox>
#include <QSettings>
#include <QMenu>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <cmath>
#include <QLabel>
#include <QPushButton>
#include <QToolBar>
#include <QTime>
#include <QTabWidget>
#include <QGroupBox>

#include "../../extlibs/cimg.h"

#include "programoptions.h"
#include "jkimage.h"
#include "qfespimb040cameraview.h"
#include "jkqttools.h"


#include "qtlogfile.h"
#include "qfextensionmanager.h"
#include "../interfaces/qfextensioncamera.h"
#include "qfextension.h"
#include "tools.h"
#include "qfespimb040cameraconfig.h"

/*! \brief SPIM Control Extension (B040, DKFZ Heidelberg) main window
    \ingroup qf3ext_spimb040

    This window allows to configure a SPIM microscope setup. Images are displayed in sub windows.

    It also implements QFPluginLogService and may thus be used for logging.
 */
class QFESPIMB040MainWindow : public QWidget, public QFPluginLogService {
        Q_OBJECT
    public:
        QFESPIMB040MainWindow(QFExtensionManager* extManager, QWidget* parent=NULL);
        virtual ~QFESPIMB040MainWindow();

        /** \brief load settings */
        void loadSettings(ProgramOptions* settings);
        /** \brief save settings */
        void storeSettings(ProgramOptions* settings);

    protected:


        /** \brief main log widget */
        QtLogFile* logMain;


        QFESPIMB040CameraConfig* camConfig1;
        QFESPIMB040CameraConfig* camConfig2;


        /** \brief handles the close event, also close all camera views in camViews
         *
         *  disconnect devices and close camera view
         */
        void closeEvent ( QCloseEvent * event );


        /** \brief Create all widgets on this window, called in the constructor before createActions() */
        void createWidgets(QFExtensionManager* extManager);
        /** \brief Create ll QActions for this window, clled after createWidgets() in the constructor.
         *         So you also have to connect and add (e.g. to toolbars) QAction's here! */
        void createActions();






    public:

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
};

#endif // QFESPIMB040MAINWINDOW_H
