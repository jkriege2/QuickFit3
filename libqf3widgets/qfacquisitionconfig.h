#ifndef QFACQUISITIONCONFIG_H
#define QFACQUISITIONCONFIG_H

#include <QGroupBox>
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

#include "programoptions.h"
#include "jkqttools.h"


#include "qtlogfile.h"
#include "qfextensionmanager.h"
#include "../interfaces/qfextensionacquisition.h"
#include "qfextension.h"
#include "tools.h"
#include "qfacquisitioncombobox.h"
#include "qfacquisitionconfigcombobox.h"
#include "qfextensionmeasurementdevice.h"


class QFAcquisitionConfig : public QGroupBox, public QFAcquisitionConfigComboBoxStartResume {
        Q_OBJECT
    protected:
        /** \brief states used in conjunction with displayStates() */
        enum States {Disconnected, Connected, Inactive,Locked};
    public:



        /** Default constructor */
        QFAcquisitionConfig(QWidget* parent);
        /** Default destructor */
        virtual ~QFAcquisitionConfig();

        void setLog(QFPluginLogService* logger);

        void init(QFPluginServices *pluginServices, QString configDirectory);

        /** \brief load settings */
        void loadSettings(QSettings& settings, QString prefix);
        /** \brief save settings */
        void storeSettings(QSettings& settings, QString prefix);

        /*! \brief returns a pointer to the QFExtensionAcquisition and ensures exclusive access to one camera therein.

            This method first stops any preview or use of the <QFExtensionAcquisition, camera> pair managed by this class.
            Then it returns a pointer to QFExtensionAcquisition and which camera to use inside it. This object is then
            locked for further access until releaseCamera() is called.
            While the camera is locked, a second call to lockCamera() will always return \c false and NULL-pointers!

            \b Note that the camera has to be connected already! Else this function will return \c false.

            \return \c true if camera is available \c false else
            \param[out] extension pointer to the extension representing the camera
            \param[out] ecamera pointer to the QFExtensionAcquisition representing the camera (basically a typecast from \a extension
            \param[out] camera which camera inside \a ecamera to use.
            \param[out] acquisitionSettingsFilename filename of the acquisition settings selected by the suer
        */
        bool lockDevice(QFExtension** extension, QFExtensionAcquisition** ecamera, int* camera);

        /*! \brief release a locked camera, for more details see lockCamera() */
        void releaseDevice();

        virtual void stop();
        virtual void resume();

        void connectAcquition();
        void disconnectAcquition();
        bool isAcquitionConnected() const;

        /** \brief if the camera has measureable values, these are stored in the given array by this function
         *
         *  If the camera is locked: Make sure to only call this from the thread that locked the camera, as it may NOT be thread safe!!!
         */
        void storeMeasurements(QMap<QString, QVariant>& data, const QString& prefix=QString(""));

        QFAcquisitionComboBox* cameraComboBox() const { return cmbAcquisitionDevice; }
    public slots:
        void setReadOnly(bool readonly);


    signals:
        /** \brief emitted when the set of configuration files changes */
        void configFilesChanged();

    protected:
        QFExtensionManager* m_extManager;


    protected slots:

    protected:


        QFPluginServices* m_pluginServices;

        QFPluginLogService* m_log;


        /** \brief indicates whether the camera is locked */
        bool locked;

        /*! \brief combobox to select an acquisition device

            The data() property of this QComboBox is used to store/specify the device (and item therein) to use.
            It contains a QPoint, where x specifies the device in cameras and y the camera inside the selected
            device.
        */
        QFAcquisitionComboBox* cmbAcquisitionDevice;
        /** \brief combobox for the selected camera acquisition configuration */
        QFAcquisitionConfigEditorWidget* cmbAcquisitionConfiguration;


        /** \brief button to connect/disconnect to a device */
        QToolButton* btnConnect;

        /** \brief action to connect/disconnect to acquisition device */
        QAction* actDisConnect;




        /** \brief handles the close event, also close all camera views in camViews
         *
         *  disconnect devices and close camera view
         */
        void closeEvent ( QCloseEvent * event );

        void showEvent ( QShowEvent * event );

        /** \brief Create all widgets on this window, called in the constructor before createActions() */
        void createWidgets();
        /** \brief Create ll QActions for this window, clled after createWidgets() in the constructor.
         *         So you also have to connect and add (e.g. to toolbars) QAction's here! */
        void createActions();

        /** \brief disconnect camera device \a camView

            \param camView work with the given camera view
        */
        void disconnectDevice();


        /*! \brief connect camera view \a camView + camera  \a camView widgets to a given acquisition device and camera therein

            \param extension the extension objectof thecamera to connect to
            \param cam same as extension, but mapped to QFExtensionAcquisition
            \param camera the camera inside \a device to connect to
            \return \c true on success
        */
        bool connectDevice(QFExtension* extension, QFExtensionAcquisition* cam, int camera, QObject* object);

    protected slots:

        /** \brief connect/disconnect to acquisition device
         *
         * This method mainly sets the enabled properties of the QActions and widgets that play a role in device control.
         *
         * \note this function is a wrapper slot function and calls the respective protected function connectDevice()
                 or disconnectDevice().
         */
        void disConnectAcquisitionDevice();

        /** \brief set enabled/disabled states of the actions and widgets according to the given parameter */
        void displayStates(States state);

    private:
};

#endif // QFACQUISITIONCONFIG_H
