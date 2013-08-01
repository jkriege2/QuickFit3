#ifndef QFESPIMB040CAMPARAMSTACKCONFIGWIDGET2_H
#define QFESPIMB040CAMPARAMSTACKCONFIGWIDGET2_H

#include <QSettings>
#include <QAction>
#include <QToolButton>
#include <QWidget>
#include "../interfaces/qfextensionlinearstage.h"
#include "qfespimb040filenametool.h"
#include "qtriple.h"
#include "qfespimb040opticssetup.h"
#include "qfespimb040acquisitiontools.h"
class QFESPIMB040MainWindow; // forward
class QFPluginServices; // forward
class QFExtension;
//class QFESPIMB040OpticsSetup;

namespace Ui {
    class QFESPIMB040CamParamStackConfigWidget2;
}


/*! \brief widget that allows to configure an image stack acquisition
    \ingroup qf3ext_spimb040
 */
class QFESPIMB040CamParamStackConfigWidget2 : public QWidget, public QFESPIMB040FilenameTool {
        Q_OBJECT

    public:
        explicit QFESPIMB040CamParamStackConfigWidget2(QFESPIMB040AcquisitionTools* acqTools, QFPluginLogService* log, QWidget* parent, QFPluginServices* pluginServices, QFESPIMB040OpticsSetupBase* opticsSetup, QFESPIMB040AcquisitionDescription* acqDescription, QFESPIMB040ExperimentDescription* expDescription, QString configDirectory);
        ~QFESPIMB040CamParamStackConfigWidget2();

        /** \brief return the filename for the currently selected camera configuration */
        QString currentConfigFilename(int camera) const;
        /** \brief return the name (not the full filename with path) for the currently selected camera configuration */
        QString currentConfigName(int camera) const;

        void updateReplaces();

    signals:
        void doStack();

    public slots:
        void lightpathesChanged(QFESPIMB040OpticsSetupItems lightpathes);
        void performStack();
    public:
        void loadSettings(QSettings& settings, QString prefix);
        void storeSettings(QSettings& settings, QString prefix) const;


    protected:
        QString prefix1();
        QString prefix2();
        bool use1() const;
        bool use2() const;
        bool saveMeasurements() const;
        bool previewMode() const;

        int images() const;

        int stackParameter() const;
        QString stackParameterName() const;
        int stackMode() const;
        QString stackModeName() const;
        double stackStart() const;
        double stackDelta() const;
        double stackEnd() const;
        QList<double> stack() const;

        bool lightpathActivated() const;
        QString lightpathFilename() const;

        QString lightpath() const;
    protected slots:
        void on_btnLoadTemplate_clicked();
        void on_btnSaveTemplate_clicked();
        void on_spinEnd_valueChanged(double value);
        void on_btnAcquire_clicked();
        void on_chkUse1_clicked(bool enabled);
        void on_chkUse2_clicked(bool enabled);

        void on_spinStart_valueChanged(double value);
        void on_spinDelta_valueChanged(double value);
        void on_cmbMode_currentIndexChanged(int index);

        void updateLabel();
    private:
        Ui::QFESPIMB040CamParamStackConfigWidget2 *ui;
        QFPluginServices* m_pluginServices;
        QFESPIMB040OpticsSetupBase* opticsSetup;
        QFESPIMB040AcquisitionDescription* acqDescription;
        QFESPIMB040ExperimentDescription* expDescription;
        QFPluginLogService* log;
        QFESPIMB040AcquisitionTools* acqTools;

};

#endif // QFESPIMB040CAMPARAMSTACKCONFIGWIDGET2_H
