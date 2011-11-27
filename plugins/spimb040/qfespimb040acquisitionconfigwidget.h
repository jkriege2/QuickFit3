#ifndef QFESPIMB040ACQUISITIONCONFIGWIDGET_H
#define QFESPIMB040ACQUISITIONCONFIGWIDGET_H

#include <QWidget>
#include <QSettings>
#include <QAction>
#include <QToolButton>
#include <QWidget>
#include "../interfaces/qfextensionlinearstage.h"

class QFESPIMB040MainWindow; // forward
class QFPluginServices; // forward

namespace Ui {
    class QFESPIMB040AcquisitionConfigWidget;
}

class QFESPIMB040AcquisitionConfigWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QFESPIMB040AcquisitionConfigWidget(QWidget* parent, QFPluginServices* pluginServices);
    ~QFESPIMB040AcquisitionConfigWidget();
signals:
    void doAcquisition();

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
    bool overview() const;

protected slots:
    void on_btnAcquire_clicked();
    void on_chkUse1_clicked(bool enabled);
    void on_chkUse2_clicked(bool enabled);
private:
    QFPluginServices* m_pluginServices;
    Ui::QFESPIMB040AcquisitionConfigWidget *ui;
};

#endif // QFESPIMB040ACQUISITIONCONFIGWIDGET_H
