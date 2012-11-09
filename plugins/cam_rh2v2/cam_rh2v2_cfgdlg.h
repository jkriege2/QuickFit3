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
    void on_sbFrameTime_valueChanged(double value);
    void on_cbBinning_currentIndexChanged(int index);
    void on_cbCorr_stateChanged(int state);

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
};

#endif // CAM_RH2V2_CFGDLG_H
