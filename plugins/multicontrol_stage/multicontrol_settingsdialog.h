#ifndef MULTICONTROL_SETTINGSDIALOG_H
#define MULTICONTROL_SETTINGSDIALOG_H

#include <QWidget>
#include<QtGlobal>
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include "qfpluginservices.h"


class MulticontrolSettingsDialog : public QDialog

{
    Q_OBJECT
public:
    explicit MulticontrolSettingsDialog(QSettings *newSettings, QWidget *parent = 0);
    ~MulticontrolSettingsDialog();

protected:
    QSettings *settings;
    QGroupBox *velGroup, *comGroup;
    QGridLayout *settingsLayout, *velGroupLayout, *comGroupLayout;
    QDoubleSpinBox *maxAccelBox, *maxVelBox, *maxJoyspeedBox;
    QPushButton *confirmer, *canceler;
    QComboBox *comCombo;
    //QString comPort;



    void log_text(QString message);
    void log_warning(QString message);
    void log_error(QString message);

signals:
    void valuesChanged();

public slots:
    void cancel();
    void confirm();

};

#endif // MULTICONTROL_SETTINGSDIALOG_H
