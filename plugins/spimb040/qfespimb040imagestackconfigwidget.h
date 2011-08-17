#ifndef QFESPIMB040IMAGESTACKCONFIGWIDGET_H
#define QFESPIMB040IMAGESTACKCONFIGWIDGET_H

#include <QSettings>
#include <QAction>
#include <QToolButton>
#include <QWidget>
#include "../interfaces/qfextensionlinearstage.h"

class QFESPIMB040MainWindow; // forward
class QFExtensionServices; // forward

namespace Ui {
    class QFESPIMB040ImageStackConfigWidget;
}


/*! \brief widget that allows to configure an image stack acquisition
    \ingroup qf3ext_spimb040
 */
class QFESPIMB040ImageStackConfigWidget : public QWidget {
        Q_OBJECT

    public:
        explicit QFESPIMB040ImageStackConfigWidget(QFESPIMB040MainWindow* parent, QFExtensionServices* pluginServices);
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

        QFExtensionLinearStage* stage() const;
        QFExtension* stageExtension() const;
        int currentAxisID() const;
        int stackCount() const;
        double stackStart() const;
        double stackDelta() const;
    protected slots:
        void on_btnAcquire_clicked();
        void on_spinStart_valueChanged(double value);
        void on_spinDelta_valueChanged(double value);
        void on_spinSteps_valueChanged(int value);
        void on_btnGetCurrent_clicked();
        void on_btnConfig_clicked();
        void on_btnConnect_clicked();
        void checkStage();
        void updateLabel();
    private:
        Ui::QFESPIMB040ImageStackConfigWidget *ui;
        QFESPIMB040MainWindow* m_parent;
        QFExtensionServices* m_pluginServices;

};

#endif // QFESPIMB040IMAGESTACKCONFIGWIDGET_H
