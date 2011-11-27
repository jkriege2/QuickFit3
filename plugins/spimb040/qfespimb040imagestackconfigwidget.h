#ifndef QFESPIMB040IMAGESTACKCONFIGWIDGET_H
#define QFESPIMB040IMAGESTACKCONFIGWIDGET_H

#include <QSettings>
#include <QAction>
#include <QToolButton>
#include <QWidget>
#include "../interfaces/qfextensionlinearstage.h"

class QFESPIMB040MainWindow; // forward
class QFPluginServices; // forward
class QFExtension;

namespace Ui {
    class QFESPIMB040ImageStackConfigWidget;
}


/*! \brief widget that allows to configure an image stack acquisition
    \ingroup qf3ext_spimb040
 */
class QFESPIMB040ImageStackConfigWidget : public QWidget {
        Q_OBJECT

    public:
        explicit QFESPIMB040ImageStackConfigWidget(QWidget* parent, QFPluginServices* pluginServices);
        ~QFESPIMB040ImageStackConfigWidget();

    signals:
        void doStack();

    public:
        void loadSettings(QSettings& settings, QString prefix);
        void storeSettings(QSettings& settings, QString prefix) const;

        /** \brief returns the current counter value */
        int counter() const;
        /** \brief increment the current counter value */
        void incCounter();

        QString prefix1() const;
        QString prefix2() const;
        bool use1() const;
        bool use2() const;

        bool useStage2() const;
        bool useStage3() const;

        int delay() const;
        QFExtensionLinearStage* stage() const;
        QFExtension* stageExtension() const;
        int currentAxisID() const;
        int stackCount() const;
        double stackStart() const;
        double stackDelta() const;


        QFExtensionLinearStage* stage2() const;
        QFExtension* stageExtension2() const;
        int currentAxisID2() const;
        int stackCount2() const;
        double stackStart2() const;
        double stackDelta2() const;


        QFExtensionLinearStage* stage3() const;
        QFExtension* stageExtension3() const;
        int currentAxisID3() const;
        int stackCount3() const;
        double stackStart3() const;
        double stackDelta3() const;
    protected slots:
        void on_btnAcquire_clicked();
        void on_chkUse1_clicked(bool enabled);
        void on_chkUse2_clicked(bool enabled);

        void on_spinStart_valueChanged(double value);
        void on_spinDelta_valueChanged(double value);
        void on_spinSteps_valueChanged(int value);
        void on_btnGetCurrent_clicked();
        void on_btnConfig_clicked();
        void on_btnConnect_clicked();

        void on_chkStage2_clicked(bool enabled);
        void on_chkStage3_clicked(bool enabled);

        void on_spinStart2_valueChanged(double value);
        void on_spinDelta2_valueChanged(double value);
        void on_spinSteps2_valueChanged(int value);
        void on_btnGetCurrent2_clicked();
        void on_btnConfig2_clicked();
        void on_btnConnect2_clicked();

        void on_spinStart3_valueChanged(double value);
        void on_spinDelta3_valueChanged(double value);
        void on_spinSteps3_valueChanged(int value);
        void on_btnGetCurrent3_clicked();
        void on_btnConfig3_clicked();
        void on_btnConnect3_clicked();

        void checkStage();
        void updateLabel();
    private:
        Ui::QFESPIMB040ImageStackConfigWidget *ui;
        QFPluginServices* m_pluginServices;

};

#endif // QFESPIMB040IMAGESTACKCONFIGWIDGET_H
