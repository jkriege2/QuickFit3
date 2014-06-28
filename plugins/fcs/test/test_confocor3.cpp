#include "qftools.h"
#include "../confocor3tools.h"
#include <QDebug>

int main(int argc, char *argv[])
{


    QString fn;


    try {
        fn="../examples/fcs_testdata/A549-TVA950+monoGFP_4.fcs";
        qDebug()<<fn;
        qDebug()<<"== READ ==================================================================";
        Confocor3Tools conf;
        qDebug()<<"  -> "<<conf.loadFile(fn);
        if (conf.wasError()) {
            qDebug()<<"ERROR WHILE READING:"<<conf.getLastErrors();
        }
        qDebug()<<"  filename = "<<conf.getData().filename;
        qDebug()<<"  name = "<<conf.getData().name;
        qDebug()<<"  comment = "<<conf.getData().comment;
        qDebug()<<"  sortorder = "<<conf.getData().sortorder;
        qDebug()<<"  fcsdatasets.size = "<<conf.getData().fcsdatasets.size();
        qDebug()<<"  groups = "<<conf.getData().getGroups();
        qDebug()<<"  roles = "<<conf.getData().getRoles();
        qDebug()<<"  repetitions = "<<conf.getData().getRepetitions();
        for (int i=0; i<conf.getData().fcsdatasets.size(); i++) {
            qDebug()<<"    fcsdatasets["<<i<<"].channel="<<conf.getData().fcsdatasets[i].channel;
            qDebug()<<"    fcsdatasets["<<i<<"].group="<<conf.getData().fcsdatasets[i].group;
            qDebug()<<"    fcsdatasets["<<i<<"].role="<<conf.getData().fcsdatasets[i].role;
            qDebug()<<"    fcsdatasets["<<i<<"].id="<<conf.getData().fcsdatasets[i].id;
            qDebug()<<"    fcsdatasets["<<i<<"].type="<<conf.getData().fcsdatasets[i].type;
            qDebug()<<"    fcsdatasets["<<i<<"].channelNo="<<conf.getData().fcsdatasets[i].channelNo;
            qDebug()<<"    fcsdatasets["<<i<<"].channelNo2="<<conf.getData().fcsdatasets[i].channelNo2;
            qDebug()<<"    fcsdatasets["<<i<<"].acqtime="<<conf.getData().fcsdatasets[i].acqtime;
            qDebug()<<"    fcsdatasets["<<i<<"].repetition="<<conf.getData().fcsdatasets[i].repetition;
            qDebug()<<"    fcsdatasets["<<i<<"].kinetic="<<conf.getData().fcsdatasets[i].kinetic;
            qDebug()<<"    fcsdatasets["<<i<<"].position="<<conf.getData().fcsdatasets[i].position;
            qDebug()<<"    fcsdatasets["<<i<<"].recCnt1="<<conf.getData().fcsdatasets[i].recCnt1;
            qDebug()<<"    fcsdatasets["<<i<<"].recCnt2="<<conf.getData().fcsdatasets[i].recCnt2;
            qDebug()<<"    fcsdatasets["<<i<<"].reverseFCCS="<<conf.getData().fcsdatasets[i].reverseFCCS;
            qDebug()<<"    fcsdatasets["<<i<<"].time="<<conf.getData().fcsdatasets[i].time.size();
            qDebug()<<"    fcsdatasets["<<i<<"].rate="<<conf.getData().fcsdatasets[i].rate.size();
            qDebug()<<"    fcsdatasets["<<i<<"].tau="<<conf.getData().fcsdatasets[i].tau.size();
            qDebug()<<"    fcsdatasets["<<i<<"].corr="<<conf.getData().fcsdatasets[i].corr.size();
            qDebug()<<"    fcsdatasets["<<i<<"].pch_photons="<<conf.getData().fcsdatasets[i].pch_photons.size();
            qDebug()<<"    fcsdatasets["<<i<<"].pch="<<conf.getData().fcsdatasets[i].pch.size();
        }


    } catch (std::exception& E) {
        qDebug()<<fn<<": \n   ERROR: "<<E.what();
    }



    try {
        fn="../examples/fcs_testdata/004_1xGFP-CP.fcs";
        qDebug()<<fn;
        qDebug()<<"== READ ==================================================================";
        Confocor3Tools conf;
        qDebug()<<"  -> "<<conf.loadFile(fn);
        if (conf.wasError()) {
            qDebug()<<"ERROR WHILE READING:"<<conf.getLastErrors();
        }
        qDebug()<<"  filename = "<<conf.getData().filename;
        qDebug()<<"  name = "<<conf.getData().name;
        qDebug()<<"  comment = "<<conf.getData().comment;
        qDebug()<<"  sortorder = "<<conf.getData().sortorder;
        qDebug()<<"  fcsdatasets.size = "<<conf.getData().fcsdatasets.size();
        qDebug()<<"  groups = "<<conf.getData().getGroups();
        qDebug()<<"  roles = "<<conf.getData().getRoles();
        qDebug()<<"  repetitions = "<<conf.getData().getRepetitions();
        for (int i=0; i<conf.getData().fcsdatasets.size(); i++) {
            qDebug()<<"    fcsdatasets["<<i<<"].channel="<<conf.getData().fcsdatasets[i].channel;
            qDebug()<<"    fcsdatasets["<<i<<"].group="<<conf.getData().fcsdatasets[i].group;
            qDebug()<<"    fcsdatasets["<<i<<"].role="<<conf.getData().fcsdatasets[i].role;
            qDebug()<<"    fcsdatasets["<<i<<"].id="<<conf.getData().fcsdatasets[i].id;
            qDebug()<<"    fcsdatasets["<<i<<"].type="<<conf.getData().fcsdatasets[i].type;
            qDebug()<<"    fcsdatasets["<<i<<"].channelNo="<<conf.getData().fcsdatasets[i].channelNo;
            qDebug()<<"    fcsdatasets["<<i<<"].channelNo2="<<conf.getData().fcsdatasets[i].channelNo2;
            qDebug()<<"    fcsdatasets["<<i<<"].acqtime="<<conf.getData().fcsdatasets[i].acqtime;
            qDebug()<<"    fcsdatasets["<<i<<"].repetition="<<conf.getData().fcsdatasets[i].repetition;
            qDebug()<<"    fcsdatasets["<<i<<"].kinetic="<<conf.getData().fcsdatasets[i].kinetic;
            qDebug()<<"    fcsdatasets["<<i<<"].position="<<conf.getData().fcsdatasets[i].position;
            qDebug()<<"    fcsdatasets["<<i<<"].recCnt1="<<conf.getData().fcsdatasets[i].recCnt1;
            qDebug()<<"    fcsdatasets["<<i<<"].recCnt2="<<conf.getData().fcsdatasets[i].recCnt2;
            qDebug()<<"    fcsdatasets["<<i<<"].reverseFCCS="<<conf.getData().fcsdatasets[i].reverseFCCS;
            qDebug()<<"    fcsdatasets["<<i<<"].time="<<conf.getData().fcsdatasets[i].time.size();
            qDebug()<<"    fcsdatasets["<<i<<"].rate="<<conf.getData().fcsdatasets[i].rate.size();
            qDebug()<<"    fcsdatasets["<<i<<"].tau="<<conf.getData().fcsdatasets[i].tau.size();
            qDebug()<<"    fcsdatasets["<<i<<"].corr="<<conf.getData().fcsdatasets[i].corr.size();
            qDebug()<<"    fcsdatasets["<<i<<"].pch_photons="<<conf.getData().fcsdatasets[i].pch_photons.size();
            qDebug()<<"    fcsdatasets["<<i<<"].pch="<<conf.getData().fcsdatasets[i].pch.size();
        }

    } catch (std::exception& E) {
        qDebug()<<fn<<": \n   ERROR: "<<E.what();
    }

    return 0;
}
