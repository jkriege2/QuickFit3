/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ANDORSETTINGSDIALOG_H
#define ANDORSETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>
#include <inttypes.h>

namespace Ui {
    class AndorSettingsDialog;
}

/*! \brief settings dialog for andor camera
    \ingroup qf3ext_andor

    \note Note that this class is designed to be initialized for a given camera. The selected camera may NOT be changed.
    So the savest way to use this dialog is:
      -# create it
      -# use it
      -# destroy it
    .
    for every single use of the dialog

    \note call setupWidgets() before showing the dialog, so all comboboxes and other widgets are setup properly.
 */
class AndorSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AndorSettingsDialog(int camera, QWidget *parent = 0);
    ~AndorSettingsDialog();

    /** \brief read dialog contents from QSettings object */
    void readSettings(QSettings& settings);
    /** \brief write dialog contents to QSettings object */
    void writeSettings(QSettings& settings) const;

    /** \brief set the camera information */
    void setInfo(const QString& info);

    /** \brief setup the contents of some of the widgets */
    void setupWidgets();

    /** \brief set an image to use for current image */
    void setImage(uint32_t* image);
private:
    Ui::AndorSettingsDialog *ui;
    QString m_headModel;
    int m_sensorWidth;
    int m_sensorHeight;
    int m_camera;
    bool m_updatingSubregion;
    bool m_updatingPreview;
    bool m_calcTiming;
    bool m_updatingSensorSetup;

    uint32_t* m_image;
    QImage m_pix;

    /** select an Andor camera */
    void selectCamera(int iSelectedCamera);

    /** \brief calculate the current image rect, based on the current widget settings */
    QRect calcImageRect();

    /** \brief resize subregion to given size and keep the center */
    void resizeSubregion(int width, int height);

    /*! \brief update sensor setting widgets

        if \c leaveout==-1 then all widgets are update
        if \c leaveout==0 then preamp gain is not updated
        if \c leaveout==1 then hor. shift speed is not updated
     */
    void updateSensorSetup(int leaveout=-1);

    /** \brief get the readmode, as set by the widgets */
    int getReadMode();

    /** \brief get the current preamp gain index */
    int getPreamp();

    /** \brief update the preview image */
    void updatePreview();

    /** \brief calculate actual image timing (may contain tampering with camera settings !) */
    void calcTiming();

protected:
    virtual void resizeEvent(QResizeEvent* event);
    virtual void showEvent(QShowEvent* event);

private slots:
    /** \brief update widgets that allow to set subregion */
    void updateSubregion();

    void on_cmbReadMode_currentIndexChanged(int currentIndex);
    void on_btnCenter_clicked();
    void on_btnFull_clicked();
    void on_btn32_clicked();
    void on_btn16_clicked();
    void on_btn8_clicked();
    void on_cmbADChannel_currentIndexChanged(int currentIndex);
    void on_cmbAmplifier_currentIndexChanged(int currentIndex);
    void on_cmbHorizontalShiftSpeed_currentIndexChanged(int currentIndex);
    void on_cmbPreampGain_currentIndexChanged(int currentIndex);
    void on_cmbVerticalShiftSpeed_currentIndexChanged(int currentIndex);

    void on_spinWidth_valueChanged(int value);
    void on_spinHeight_valueChanged(int value);
    void on_spinLeft_valueChanged(int value);
    void on_spinTop_valueChanged(int value);
    void on_spinHorizontalBinning_valueChanged(int value);
    void on_spinVerticalBinning_valueChanged(int value);

    void on_chkFrameTransfer_toggled(bool value);
    void on_chkBaselineClamp_toggled(bool value);
    void on_chkEMGain_toggled(bool value);
    void on_spinKineticCycles_valueChanged(int value);
    void on_spinAccumulates_valueChanged(int value);
    void on_spinExposure_valueChanged(double value);
    void on_spinKineticCycleTime_valueChanged(double value);
    void on_spinAccCycleTime_valueChanged(double value);
    void on_chkEnableExtendedEMGAIN_toggled(bool value);

    void updateDuration();

};

#endif // ANDORSETTINGSDIALOG_H
