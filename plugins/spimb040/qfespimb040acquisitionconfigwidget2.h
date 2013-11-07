#ifndef QFESPIMB040ACQUISITIONCONFIGWIDGET2_H
#define QFESPIMB040ACQUISITIONCONFIGWIDGET2_H

#include <QWidget>
#include <QSettings>
#include <QAction>
#include <QToolButton>
#include <QWidget>
#include "../interfaces/qfextensionlinearstage.h"
#include "qfespimb040opticssetup.h"
#include "qfespimb040filenametool.h"
#include "qtriple.h"
#include "qfespimb040acquisitiontools.h"
#include "libtiff_tools.h"

class QFESPIMB040MainWindow; // forward
class QFPluginServices; // forward

namespace Ui {
    class QFESPIMB040AcquisitionConfigWidget2;
}


/*! \brief widget that allows to configure an image acquisition
    \ingroup qf3ext_spimb040
 */
class QFESPIMB040AcquisitionConfigWidget2 : public QWidget, public QFESPIMB040FilenameTool {
        Q_OBJECT

    public:
        explicit QFESPIMB040AcquisitionConfigWidget2(QFESPIMB040AcquisitionTools* acqTools, QFPluginLogService* log, QWidget* parent, QFPluginServices* pluginServices, QFESPIMB040OpticsSetupBase* opticsSetup, QFESPIMB040AcquisitionDescription* acqDescription, QFESPIMB040ExperimentDescription* expDescription, QString configDirectory);
        ~QFESPIMB040AcquisitionConfigWidget2();

        /** \brief return the filename for the currently selected camera configuration */
        QString currentConfigFilename(int camera) const;
        /** \brief return the name (not the full filename with path) for the currently selected camera configuration */
        QString currentConfigName(int camera) const;
        /** \brief return the filename for the currently selected camera preview configuration */
        QString currentPreviewConfigFilename(int camera, int preview=0) const;
        /** \brief return the name (not the full filename with path) for the currently selected camera preview configuration */
        QString currentPreviewConfigName(int camera, int preview=0) const;
        /** \brief return the number of background frames */
        int currentBackgroundFrames(int camera) const;
        /** \brief return whether to save background frames */
        bool saveBackground() const;
    public slots:
        void lightpathesChanged(QFESPIMB040OpticsSetupItems lightpathes);

        void performAcquisition();

    signals:
        void doAcquisition();

    public:
        void loadSettings(QSettings& settings, QString prefix);
        void storeSettings(QSettings& settings, QString prefix) const;

        QVariant getCameraSetting1(QFExtensionCamera::CameraSetting which)const;
        void setCameraSetting1(QFExtensionCamera::CameraSetting which, QVariant value);
        QVariant getCameraSetting2(QFExtensionCamera::CameraSetting which)const;
        void setCameraSetting2(QFExtensionCamera::CameraSetting which, QVariant value);
        void clearCameraSettings1();
        void clearCameraSettings2();
        QMap<QFExtensionCamera::CameraSetting, QVariant> getCameraSettings1() const;
        QMap<QFExtensionCamera::CameraSetting, QVariant> getCameraSettings2() const;

    protected:

        QString prefix1() ;
        QString prefix2() ;
        bool use1() const;
        bool use2() const;
        bool overview() const;

        bool onlyAcquisition1() const;
        bool onlyAcquisition2() const;


        int frames1() const;
        int frames2() const;


        bool lightpathActivated() const;
        QString lightpathFilename() const;
        QString lightpath() const;
        int repeats() const;

        QString lightpathPreview(int preview=0);
        QString lightpathFilenamePreview(int preview=0);
        bool lightpathActivatedPreview(int preview=0);
        int previewCount();

        QString lightpathPreview4() const;
        QString lightpathFilenamePreview4() const;
        QString lightpathPreview3() const;
        QString lightpathFilenamePreview3() const;
        QString lightpathPreview2() const;
        QString lightpathFilenamePreview2() const;

        bool lightpathActivatedPreview2() const;
        bool lightpathActivatedPreview3() const;
        bool lightpathActivatedPreview4() const;
    protected slots:
        void on_btnSaveTemplate_clicked();
        void on_btnLoadTemplate_clicked();
        void on_btnAcquire_clicked();
        void on_btnAcquireNext_clicked();
        void on_chkUse1_toggled(bool enabled);
        void on_chkUse2_toggled(bool enabled);
        void on_chkBackground_toggled(bool enabled);
        void updateBackgroundWidgets();
        void updateReplaces();
        void on_btnReset1_clicked();
        void on_btnReset2_clicked();



        void on_btnAcq1Test_pressed();
        void on_btnAcq1Test_released();
        void on_btnAcq1TestPreview1_pressed();
        void on_btnAcq1TestPreview1_released();
        void on_btnAcq1TestPreview2_pressed();
        void on_btnAcq1TestPreview2_released();
        void on_btnAcq1TestPreview3_pressed();
        void on_btnAcq1TestPreview3_released();
        void on_btnAcq1TestPreview4_pressed();
        void on_btnAcq1TestPreview4_released();

        void on_btnAcq2Test_pressed();
        void on_btnAcq2Test_released();
        void on_btnAcq2TestPreview1_pressed();
        void on_btnAcq2TestPreview1_released();
        void on_btnAcq2TestPreview2_pressed();
        void on_btnAcq2TestPreview2_released();
        void on_btnAcq2TestPreview3_pressed();
        void on_btnAcq2TestPreview3_released();
        void on_btnAcq2TestPreview4_pressed();
        void on_btnAcq2TestPreview4_released();

    private:
        QFPluginServices* m_pluginServices;
        Ui::QFESPIMB040AcquisitionConfigWidget2 *ui;
        QFESPIMB040OpticsSetupBase* opticsSetup;
        QFESPIMB040AcquisitionDescription* acqDescription;
        QFESPIMB040ExperimentDescription* expDescription;
        QMap<QFExtensionCamera::CameraSetting, QVariant> camset1, camset2;

        QFPluginLogService* log;
        QFESPIMB040AcquisitionTools* acqTools;
};

#endif // QFESPIMB040ACQUISITIONCONFIGWIDGET2_H

