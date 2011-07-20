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

    /** \brief calculate the current image rect, based on the current widget settings */
    QRect calcImageRect();

    /** \brief resize subregion to given size and keep the center */
    void resizeSubregion(int width, int height);

private slots:
    /** \brief update widgets that allow to set subregion */
    void updateSubregion();





    void on_cmbReadMode_currentIndexChanged(int currentIndex);
    void on_btnCenter_clicked();
    void on_btnFull_clicked();
    void on_btn32_clicked();
    void on_btn16_clicked();
    void on_btn8_clicked();
};

#endif // ANDORSETTINGSDIALOG_H
