/*
Copyright (c) 2008-2015 Jan Buchholz, German Cancer Research Center

    

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

#ifndef CAM_RH2V2_CFGDLG_H
#define CAM_RH2V2_CFGDLG_H

#include <QDialog>

namespace Ui {
class cam_rh2v2_cfgdlg;
}

class cam_rh2v2_cfgdlg : public QDialog
{
    Q_OBJECT
    
public:
    explicit cam_rh2v2_cfgdlg(QWidget *parent = 0);
    ~cam_rh2v2_cfgdlg();

protected:
    bool isCalculating;
    bool isEditing;

protected slots:
    void onChange();
    void on_sbRoiFirst_valueChanged(int value);
    void on_sbRoiLast_valueChanged(int value);
    void on_dsbFrameTime_valueChanged(double value);
    void on_cbBinning_currentIndexChanged(int index);
    void on_cbCorr_stateChanged(int state);
    void on_cbRaw_stateChanged(int state);
    void on_sbFrameCnt_valueChanged(double value);

    void mul2();
    void div2();

    void updateRuntime();

private:
    Ui::cam_rh2v2_cfgdlg *ui;

public:
    int getAccumulationCount();
    void setAccumulationCount(int value);
    int getFrameCount();
    void setFrameCount(int value);
    int getRoiFirst();
    void setRoiFirst(int value);
    int getRoiLast();
    void setRoiLast(int value);
    float getFrameTime();
    void setFrameTime(float value);
    int getBinning();
    void setBinning(int value);
    void setEditMode();
    void unsetEditMode();
    void checkUpdate();
    bool getCorrEnable();
    void setCorrEnable(bool value);
    bool getRawEnable();
    void setRawEnable(bool value);
};

#endif // CAM_RH2V2_CFGDLG_H
