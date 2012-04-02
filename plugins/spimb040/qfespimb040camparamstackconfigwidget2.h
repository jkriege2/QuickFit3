#ifndef QFESPIMB040CAMPARAMSTACKCONFIGWIDGET2_H
#define QFESPIMB040CAMPARAMSTACKCONFIGWIDGET2_H

#include <QSettings>
#include <QAction>
#include <QToolButton>
#include <QWidget>
#include "../interfaces/qfextensionlinearstage.h"

class QFESPIMB040MainWindow; // forward
class QFPluginServices; // forward
class QFExtension;
class QFESPIMB040OpticsSetup;

namespace Ui {
    class QFESPIMB040CamParamStackConfigWidget2;
}


/*! \brief widget that allows to configure an image stack acquisition
    \ingroup qf3ext_spimb040
 */
class QFESPIMB040CamParamStackConfigWidget2 : public QWidget {
        Q_OBJECT

    public:
        explicit QFESPIMB040CamParamStackConfigWidget2(QWidget* parent, QFPluginServices* pluginServices, QFESPIMB040OpticsSetup* opticsSetup, QString configDirectory);
        ~QFESPIMB040CamParamStackConfigWidget2();

        /** \brief return the filename for the currently selected camera configuration */
        QString currentConfigFilename(int camera) const;
        /** \brief return the name (not the full filename with path) for the currently selected camera configuration */
        QString currentConfigName(int camera) const;

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

        int stackParameter() const;
        int stackMode() const;
        double stackStart() const;
        double stackDelta() const;
        double stackEnd() const;
        QList<double> stack() const;

    protected slots:
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
        QFESPIMB040OpticsSetup* opticsSetup;

};

#endif // QFESPIMB040CAMPARAMSTACKCONFIGWIDGET2_H
