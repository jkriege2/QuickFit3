#ifndef QFESPIMB040OPTICSSETUP_H
#define QFESPIMB040OPTICSSETUP_H

#include <QWidget>
#include "programoptions.h"
#include "qfpluginservices.h"

#include "programoptions.h"
#include "jkimage.h"
#include "jkqttools.h"


#include "qtlogfile.h"
#include "qfextensionmanager.h"
#include "../interfaces/qfextensioncamera.h"
#include "qfextension.h"
#include "tools.h"
#include "objectives.h"
#include "qfextensionlinearstage.h"
#include "qfextensioncamera.h"


class QFESPIMB040MainWindow; // forward
namespace Ui {
    class QFESPIMB040OpticsSetup; // forward
}
/*! \brief SPIM Control Extension (B040, DKFZ Heidelberg): instrument setup widget
    \ingroup qf3ext_spimb040
 */
class QFESPIMB040OpticsSetup : public QWidget {
        Q_OBJECT

    public:
        explicit QFESPIMB040OpticsSetup(QFESPIMB040MainWindow* parent, QFPluginServices* pluginServices);
        ~QFESPIMB040OpticsSetup();

        void setLogging(QFPluginLogService* log);

        /** \brief load settings */
        void loadSettings(QSettings& settings, QString prefix);
        /** \brief save settings */
        void storeSettings(QSettings& settings, QString prefix);




        /*! \brief returns a pointer to the QFExtensionCamera and ensures exclusive access to one camera  therein.

            This method first stops any preview or use of the <QFExtensionCamera, camera> pair managed by this class.
            Then it returns a pointer to QFExtensionCamera and which camera to use inside it. This object is then
            locked for further access until releaseCamera() is called.
            While the camera is locked, a second call to lockCamera() will always return \c false and NULL-pointers!

            \b Note that the camera has to be connected already! Else this function will return \c false.

            \return \c true if camera is available \c false else
            \param[in] setup_cam which cam to lock (currently either 0 or 1)
            \param[out] extension pointer to the extension representing the camera
            \param[out] ecamera pointer to the QFExtensionCamera representing the camera (basically a typecast from \a extension
            \param[out] camera which camera inside \a ecamera to use.
            \param[out] acquisitionSettingsFilename filename of the acquisition settings selected by the suer
            \param[out] previewSettingsFilename filename of the preview settings selected by the suer
        */
        bool lockCamera(int setup_cam, QFExtension** extension, QFExtensionCamera** ecamera, int* camera, QString* acquisitionSettingsFilename, QString* previewSettingsFilename);

        /*! \brief release a locked camera n, for more details see lockCamera() */
        void releaseCamera(int setup_cam);

        /** \brief return a pointer to the x-axis stage class */
        QFExtensionLinearStage* getXStage();

        /** \brief return a pointer to the y-axis stage class */
        QFExtensionLinearStage* getYStage();

        /** \brief return a pointer to the z-axis stage class */
        QFExtensionLinearStage* getZStage();

        /** \brief return a pointer to the x-axis QFExtension class */
        QFExtension* getXStageExtension();

        /** \brief return a pointer to the y-axis QFExtension class */
        QFExtension* getYStageExtension();

        /** \brief return a pointer to the z-axis QFExtension class */
        QFExtension* getZStageExtension();



        /** \brief get the axis number of x-axis stage inside its class */
        int getXStageAxis();

        /** \brief get the axis number of y-axis stage inside its class */
        int getYStageAxis();

        /** \brief get the axis number of z-axis stage inside its class */
        int getZStageAxis();
    protected slots:
        void updateMagnifications();

    private:
        Ui::QFESPIMB040OpticsSetup *ui;
        QFESPIMB040MainWindow* m_parent;
        QFPluginServices* m_pluginServices;

        QFPluginLogService* m_log;
};

#endif // QFESPIMB040OPTICSSETUP_H
