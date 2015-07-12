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

#ifndef CAM_RH2V2_CORDLG_H
#define CAM_RH2V2_CORDLG_H

#include <QDialog>
#include "jkqtfastplotter.h"
#include <QDoubleSpinBox>

namespace Ui {
class cam_rh2v2_cordlg;
}

class cam_rh2v2_cordlg : public QDialog
{
    Q_OBJECT

private:
    Ui::cam_rh2v2_cordlg *ui;
    JKQTFPLinePlot **plots;
    double *taus;
    double **data;
    int width;
    int height;
    JKQTFastPlotter** plotters;
    int camera;

    int calcPosition(int width, int n, int k);

    
public:
    explicit cam_rh2v2_cordlg(QWidget *parent = 0);
    ~cam_rh2v2_cordlg();
    JKQTFPLinePlot **getPlots();
    JKQTFastPlotter *getPlotter(int index = 0);
    double *getTaus();
    double **getData();

    QDoubleSpinBox *get_dsbMin();
    QDoubleSpinBox *get_dsbMax();

    void reconfigure(int widthNew, int heightNew);
    void setCamera(int camera);
    int getPlotterCount(){return width*height;}
signals:
    void btnReset();
    void sndCmd(uint16_t);
    void reconfigure(int);

protected slots:
    void on_pbReset_clicked(bool checked);
    void on_pbCmd_clicked(bool checked);

    void on_sbXRes_valueChanged(int value);
    void on_sbYRes_valueChanged(int value);

};

#endif // CAM_RH2V2_CORDLG_H
