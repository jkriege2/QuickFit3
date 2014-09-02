/*
Copyright (c) 2008-2014 Jan Buchholz, German Cancer Research Center

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

#include "cam_rh2v2_cordlg.h"
#include "ui_cam_rh2v2_cordlg.h"

cam_rh2v2_cordlg::cam_rh2v2_cordlg(QWidget *parent) :
    QDialog(parent),width(3),height(3),plotters(NULL),camera(-1),
    ui(new Ui::cam_rh2v2_cordlg)
{
    ui->setupUi(this);
    plots=(JKQTFPLinePlot **) calloc(1024,sizeof(JKQTFPLinePlot*));
    for(int i=0;i<1024;i++){
        plots[i]=new JKQTFPLinePlot((JKQTFastPlotter*)NULL,0,(double*)NULL,(double*)NULL,QColor(((i>>7)&0x7)*32,((i>>4)&0x7)*32,(i&0xF)*16));
    }

    reconfigure(3,3);


    data = static_cast<double **>(malloc(32*32*sizeof(double*)));
    for(int i=0;i<32*32;i++){
        data[i]=static_cast<double *>(calloc(14*8,sizeof(double)));
    }
    taus = static_cast<double *>(calloc(14*8,sizeof(double)));
}

void cam_rh2v2_cordlg::reconfigure(int widthNew,int heightNew){
    if(plotters!=NULL){
        for(int x=0;x<width;x++){
            for(int y=0;y<height;y++){
                int i=y*width+x;
                if(plotters[i]!=NULL){
                    ui->glPlotter->removeWidget(plotters[i]);
                    int xp=calcPosition(32,width,x);
                    int yp=calcPosition(32,height,y);
                    plotters[i]->deletePlot(plots[32*yp+xp]);
                    plots[32*yp+xp]->setParent(NULL);
                    delete plotters[i];
                }
            }
        }
        free(plotters);
    }
    width=widthNew;
    height=heightNew;
    plotters=static_cast<JKQTFastPlotter**>(malloc(width*height*sizeof(JKQTFastPlotter*)));
    for(int x=0;x<width;x++){
        for(int y=0;y<height;y++){
            int i=y*width+x;
            plotters[i]=(JKQTFastPlotter*) new JKQTFastPlotter(this);
            ui->glPlotter->addWidget(plotters[i],y,x);
            int xp=calcPosition(32,width,x);
            int yp=calcPosition(32,height,y);
            plotters[i]->addPlot(plots[32*yp+xp]);
            plotters[i]->set_yMin(ui->dsbMin->value());
            plotters[i]->set_yMax(ui->dsbMax->value());
            plots[32*yp+xp]->setParent(plotters[y*width+x]);
            QObject::connect(ui->dsbMin, SIGNAL(valueChanged(double)), plotters[i], SLOT(set_yMin(double)));
            QObject::connect(ui->dsbMax, SIGNAL(valueChanged(double)), plotters[i], SLOT(set_yMax(double)));
        }
    }
    emit reconfigure(this->camera);
}

void cam_rh2v2_cordlg::setCamera(int camera)
{
    this->camera=camera;
}




cam_rh2v2_cordlg::~cam_rh2v2_cordlg()
{
    delete ui;
}

inline QDoubleSpinBox* cam_rh2v2_cordlg::get_dsbMin(){return ui->dsbMin;}
inline QDoubleSpinBox* cam_rh2v2_cordlg::get_dsbMax(){return ui->dsbMax;}

JKQTFPLinePlot **cam_rh2v2_cordlg::getPlots(){return plots;}
JKQTFastPlotter *cam_rh2v2_cordlg::getPlotter(int index){
    return plotters[index];
}

double *cam_rh2v2_cordlg::getTaus(){return taus;}
double **cam_rh2v2_cordlg::getData(){return data;}


void cam_rh2v2_cordlg::on_pbReset_clicked(bool checked)
{
    emit btnReset();
}

void cam_rh2v2_cordlg::on_pbCmd_clicked(bool checked)
{
    emit sndCmd(0xF111);
}

void cam_rh2v2_cordlg::on_sbXRes_valueChanged(int value){
    reconfigure(value,height);
}

void cam_rh2v2_cordlg::on_sbYRes_valueChanged(int value){
    reconfigure(width,value);
}

int cam_rh2v2_cordlg::calcPosition(int width, int n, int k){
    return (width-n)*k/(n-1)+k;
}

