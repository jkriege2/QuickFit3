#ifndef ANDORSETTINGSDIALOG_H
#define ANDORSETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>

namespace Ui {
    class AndorSettingsDialog;
}

/*! \brief settings dialog for andor camera
    \ingroup qf3ext_andor

    Note that this class is designed to be initialized for a given camera. The selected camera may NOT be changed.
    So the savest way to use this dialog is:
      -# create it
      -# use it
      -# destroy it
    .
    for every single use of the dialog
 */
class AndorSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AndorSettingsDialog(int camera, QWidget *parent = 0);
    ~AndorSettingsDialog();

    /** \brief read dialog contents from QSettings object */
    void readSettings(const QSettings& settings, const QString& prefix);
    /** \brief write dialog contents to QSettings object */
    void writeSettings(QSettings& settings, const QString& prefix) const;

    /** \brief set the camera information */
    void setInfo(const QString& info);
private:
    Ui::AndorSettingsDialog *ui;
    QString m_headModel;
    int m_sensorWidth;
    int m_sensorHeight;
    int m_camera;

    /** select an Andor camera */
    void selectCamera(int iSelectedCamera);
};

#endif // ANDORSETTINGSDIALOG_H
