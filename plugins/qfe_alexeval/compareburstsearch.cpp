/*
Copyright (c) 2012-2015 by Sebastian Isbaner

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

#include "compareburstsearch.h"
#include "analysis.h"
#include <QDebug>

void compareBurstSearch(AlexData *ad)
{
    QVector<int> numberOfBursts;
    QVector<double> duration;
    QVector<double> pr;
    QVector<double> sr;
    QVector<int> burstSize;
    double temp;

    QString dir("./");

//    JKQtPlotter *plot=new JKQtPlotter(true,NULL,NULL);



    ad->burstSearchParamsGlobal.burstSearchMethodDual=BurstSearchParams_t::AllPhotons;


    qDebug()<<"T, Sliding, Combined";
    QVector<double> vectorT;
    temp=ad->burstSearchParamsGlobal.T;
    ad->burstSearchParamsGlobal.T=50e-6;
    ad->burstSearchParamsGlobal.burstSearchMethodSingle=BurstSearchParams_t::Sliding;

    while(ad->burstSearchParamsGlobal.T<=1000e-6) {
        vectorT.append(ad->burstSearchParamsGlobal.T);

        BurstSearch(ad);
        duration.append(ad->meanBurstDuration());
        burstSize.append(ad->meanNoOfPhotons());
        numberOfBursts.append(ad->burstVectorDual.size());

        analysisFRET(ad->burstVectorDual, ad->FRETparams);
        pr.append(ad->meanPR());
        sr.append(ad->meanSR());

        ad->burstSearchParamsGlobal.T+=50e-6;
    }
    ad->burstSearchParamsGlobal.T=temp;

    QFile file(dir + "compareBurstSearch_T_Sliding_Combined.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    QString str=ad->burstSearchParamsGlobal.toString();
    out<<"#"<<str.replace("\n","\t");
    out<<"\n#T in us\tnumber of bursts\tburst size\tduration in ms\tPR\tSR\n";
    for (int i=0; i<numberOfBursts.size(); ++i)
        out<<vectorT.at(i)*1e6<<"\t"<<numberOfBursts.at(i)<<"\t"<< burstSize.at(i)<<"\t"<<duration.at(i)*1e3<<"\t"<<pr.at(i)<<"\t"<<sr.at(i)<<"\n";

//    plotDensity(plot,ad);
//    plot->get_plotter()->saveAsPixelImage(dir + "density.png");


    qDebug()<<"M, Sliding, Combined";
    numberOfBursts.clear();
    duration.clear();
    pr.clear();
    sr.clear();
    burstSize.clear();
    QVector<double> vectorM;
    temp=ad->burstSearchParamsGlobal.M;
    ad->burstSearchParamsGlobal.M=1;
    ad->burstSearchParamsGlobal.burstSearchMethodSingle=BurstSearchParams_t::Sliding;


    while(ad->burstSearchParamsGlobal.M<=20) {
        vectorM.append(ad->burstSearchParamsGlobal.M);

        BurstSearch(ad);
        duration.append(ad->meanBurstDuration());
        burstSize.append(ad->meanNoOfPhotons());
        numberOfBursts.append(ad->burstVectorDual.size());

        analysisFRET(ad->burstVectorDual, ad->FRETparams);
        pr.append(ad->meanPR());
        sr.append(ad->meanSR());

        ad->burstSearchParamsGlobal.M+=1;
    }
    ad->burstSearchParamsGlobal.M=temp;

    file.setFileName(dir + "compareBurstSearch_M_Sliding_Combined.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    out.setDevice(&file);
    str=ad->burstSearchParamsGlobal.toString();
    out<<"#"<<str.replace("\n","\t");
    out<<"\n#M\tnumber of bursts\tburst size\tduration in ms\tPR\tSR\n";
    for (int i=0; i<numberOfBursts.size(); ++i)
        out<<vectorM.at(i)<<"\t"<<numberOfBursts.at(i)<<"\t"<< burstSize.at(i)<<"\t"<<duration.at(i)*1e3<<"\t"<<pr.at(i)<<"\t"<<sr.at(i)<<"\n";




    qDebug()<<"MinBurstSize, Sliding, Combined";
    numberOfBursts.clear();
    duration.clear();
    pr.clear();
    sr.clear();
    burstSize.clear();
    QVector<int> vectorMinBurstSize;
    temp=ad->burstSearchParamsGlobal.MinBurstSize;
    ad->burstSearchParamsGlobal.MinBurstSize=10;
    ad->burstSearchParamsGlobal.burstSearchMethodSingle=BurstSearchParams_t::Sliding;


    while(ad->burstSearchParamsGlobal.MinBurstSize<=200) {
        vectorMinBurstSize.append(ad->burstSearchParamsGlobal.MinBurstSize);

        BurstSearch(ad);
        duration.append(ad->meanBurstDuration());
        burstSize.append(ad->meanNoOfPhotons());
        numberOfBursts.append(ad->burstVectorDual.size());

        analysisFRET(ad->burstVectorDual, ad->FRETparams);
        pr.append(ad->meanPR());
        sr.append(ad->meanSR());

        ad->burstSearchParamsGlobal.MinBurstSize+=10;
    }
    ad->burstSearchParamsGlobal.MinBurstSize=temp;

    file.setFileName(dir + "compareBurstSearch_MinBurstSize_Sliding_Combined.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    out.setDevice(&file);
    str=ad->burstSearchParamsGlobal.toString();
    out<<"#"<<str.replace("\n","\t");
    out<<"\n#MinBurstSize\tnumber of bursts\tburst size\tduration in ms\tPR\tSR\n";
    for (int i=0; i<numberOfBursts.size(); ++i)
        out<<vectorMinBurstSize.at(i)<<"\t"<<numberOfBursts.at(i)<<"\t"<< burstSize.at(i)<<"\t"<<duration.at(i)*1e3<<"\t"<<pr.at(i)<<"\t"<<sr.at(i)<<"\n";
    file.close();





    qDebug()<<"MinBurstrate, Sliding, Combined";
    numberOfBursts.clear();
    duration.clear();
    pr.clear();
    sr.clear();
    burstSize.clear();
    QVector<int> vectorMinBurstrate;
    temp=ad->burstSearchParamsGlobal.MinBurstrate;
    ad->burstSearchParamsGlobal.MinBurstrate=2e3;
    ad->burstSearchParamsGlobal.burstSearchMethodSingle=BurstSearchParams_t::Sliding;


    while(ad->burstSearchParamsGlobal.MinBurstrate<=60e3) {
        vectorMinBurstrate.append(ad->burstSearchParamsGlobal.MinBurstrate);

        BurstSearch(ad);
        duration.append(ad->meanBurstDuration());
        burstSize.append(ad->meanNoOfPhotons());
        numberOfBursts.append(ad->burstVectorDual.size());

        analysisFRET(ad->burstVectorDual, ad->FRETparams);
        pr.append(ad->meanPR());
        sr.append(ad->meanSR());

        ad->burstSearchParamsGlobal.MinBurstrate+=2e3;
    }
    ad->burstSearchParamsGlobal.MinBurstrate=temp;

    file.setFileName(dir + "compareBurstSearch_MinBurstrate_Sliding_Combined.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    out.setDevice(&file);
    str=ad->burstSearchParamsGlobal.toString();
    out<<"#"<<str.replace("\n","\t");
    out<<"\n#MinBurstrate\tnumber of bursts\tburst size\tduration in ms\tPR\tSR\n";
    for (int i=0; i<numberOfBursts.size(); ++i)
        out<<vectorMinBurstrate.at(i)<<"\t"<<numberOfBursts.at(i)<<"\t"<< burstSize.at(i)<<"\t"<<duration.at(i)*1e3<<"\t"<<pr.at(i)<<"\t"<<sr.at(i)<<"\n";
    file.close();




    qDebug()<<"MaxBurstrate, Sliding, Combined";
    numberOfBursts.clear();
    duration.clear();
    pr.clear();
    sr.clear();
    burstSize.clear();
    QVector<int> vectorMaxBurstrate;
    temp=ad->burstSearchParamsGlobal.MaxBurstrate;
    ad->burstSearchParamsGlobal.MaxBurstrate=5e3;
    ad->burstSearchParamsGlobal.burstSearchMethodSingle=BurstSearchParams_t::Sliding;


    while(ad->burstSearchParamsGlobal.MaxBurstrate<=100e3) {
        vectorMaxBurstrate.append(ad->burstSearchParamsGlobal.MaxBurstrate);

        BurstSearch(ad);
        duration.append(ad->meanBurstDuration());
        burstSize.append(ad->meanNoOfPhotons());
        numberOfBursts.append(ad->burstVectorDual.size());

        analysisFRET(ad->burstVectorDual, ad->FRETparams);
        pr.append(ad->meanPR());
        sr.append(ad->meanSR());

        ad->burstSearchParamsGlobal.MaxBurstrate+=5e3;
    }
    ad->burstSearchParamsGlobal.MaxBurstrate=temp;

    file.setFileName(dir + "compareBurstSearch_MaxBurstrate_Sliding_Combined.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    out.setDevice(&file);
    str=ad->burstSearchParamsGlobal.toString();
    out<<"#"<<str.replace("\n","\t");
    out<<"\n#MaxBurstrate\tnumber of bursts\tburst size\tduration in ms\tPR\tSR\n";
    for (int i=0; i<numberOfBursts.size(); ++i)
        out<<vectorMaxBurstrate.at(i)<<"\t"<<numberOfBursts.at(i)<<"\t"<< burstSize.at(i)<<"\t"<<duration.at(i)*1e3<<"\t"<<pr.at(i)<<"\t"<<sr.at(i)<<"\n";
    file.close();




    qDebug()<<"MaxIPT, LeeFilter, Combined";
    numberOfBursts.clear();
    duration.clear();
    pr.clear();
    sr.clear();
    burstSize.clear();
    QVector<int> vectorMaxIPT;
    temp=ad->burstSearchParamsGlobal.MaxIPT;
    ad->burstSearchParamsGlobal.MaxIPT=50e-6;
    ad->burstSearchParamsGlobal.burstSearchMethodSingle=BurstSearchParams_t::LeeFilter;


    while(ad->burstSearchParamsGlobal.MaxIPT<=500e-6) {
        vectorMaxIPT.append(ad->burstSearchParamsGlobal.MaxIPT);

        BurstSearch(ad);
        duration.append(ad->meanBurstDuration());
        burstSize.append(ad->meanNoOfPhotons());
        numberOfBursts.append(ad->burstVectorDual.size());

        analysisFRET(ad->burstVectorDual, ad->FRETparams);
        pr.append(ad->meanPR());
        sr.append(ad->meanSR());

        ad->burstSearchParamsGlobal.MaxIPT+=50e-6;
    }
    ad->burstSearchParamsGlobal.MaxIPT=temp;

    file.setFileName(dir + "compareBurstSearch_MaxIPT_LeeFilter_Combined.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    out.setDevice(&file);
    str=ad->burstSearchParamsGlobal.toString();
    out<<"#"<<str.replace("\n","\t");
    out<<"\n#MaxIPT in us\tnumber of bursts\tburst size\tduration in ms\tPR\tSR\n";
    for (int i=0; i<numberOfBursts.size(); ++i)
        out<<vectorMaxIPT.at(i)*1e6<<"\t"<<numberOfBursts.at(i)<<"\t"<< burstSize.at(i)<<"\t"<<duration.at(i)*1e3<<"\t"<<pr.at(i)<<"\t"<<sr.at(i)<<"\n";
    file.close();




    qDebug()<<"LeeWinSize, LeeFilter, Combined";
    numberOfBursts.clear();
    duration.clear();
    pr.clear();
    sr.clear();
    burstSize.clear();
    QVector<int> vectorLeeWinSize;
    temp=ad->burstSearchParamsGlobal.LeeWinSize;
    ad->burstSearchParamsGlobal.LeeWinSize=2;
    ad->burstSearchParamsGlobal.burstSearchMethodSingle=BurstSearchParams_t::LeeFilter;


    while(ad->burstSearchParamsGlobal.LeeWinSize<=20) {
        vectorLeeWinSize.append(ad->burstSearchParamsGlobal.LeeWinSize);

        BurstSearch(ad);
        duration.append(ad->meanBurstDuration());
        burstSize.append(ad->meanNoOfPhotons());
        numberOfBursts.append(ad->burstVectorDual.size());

        analysisFRET(ad->burstVectorDual, ad->FRETparams);
        pr.append(ad->meanPR());
        sr.append(ad->meanSR());

        ad->burstSearchParamsGlobal.LeeWinSize+=2;
    }
    ad->burstSearchParamsGlobal.LeeWinSize=temp;

    file.setFileName(dir + "compareBurstSearch_LeeWinSize_LeeFilter_Combined.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    out.setDevice(&file);
    str=ad->burstSearchParamsGlobal.toString();
    out<<"#"<<str.replace("\n","\t");
    out<<"\n#LeeWinSize\tnumber of bursts\tburst size\tduration in ms\tPR\tSR\n";
    for (int i=0; i<numberOfBursts.size(); ++i)
        out<<vectorLeeWinSize.at(i)<<"\t"<<numberOfBursts.at(i)<<"\t"<< burstSize.at(i)<<"\t"<<duration.at(i)*1e3<<"\t"<<pr.at(i)<<"\t"<<sr.at(i)<<"\n";
    file.close();

}


