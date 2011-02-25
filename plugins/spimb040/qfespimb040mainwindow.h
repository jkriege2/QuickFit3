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

#include "programoptions.h"
#include "jkimage.h"
#include "jkqtfastplotter.h"
#include "qfespimb040cameraview.h"
#include "jkqttools.h"

/*! \brief SPIM Control Extension (B040, DKFZ Heidelberg) main window
    \ingroup qf3ext_spimb040
 */
class QFESPIMB040MainWindow : public QWidget {
        Q_OBJECT
    public:
        QFESPIMB040MainWindow(QFExtensionManager* extManager, QWidget* parent=NULL);
        virtual ~QFESPIMB040MainWindow();

        /** \brief load settings */
        void loadSettings(ProgramOptions* settings);
        /** \brief save settings */
        void storeSettings(ProgramOptions* settings);
    protected:
	void closeEvent ( QCloseEvent * event );
        QTabWidget* tabMain;
        QFESPIMB040CameraView* camView1;

    private:
};

#endif // QFESPIMB040MAINWINDOW_H
