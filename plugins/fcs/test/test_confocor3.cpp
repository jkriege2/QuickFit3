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
        qDebug()<<"  positions = "<<conf.getData().positions;
        qDebug()<<"  kinetics = "<<conf.getData().kinetics;
        qDebug()<<"  repetitions = "<<conf.getData().repetitions;
        qDebug()<<"  channels = "<<conf.getData().channels;
        qDebug()<<"  hasPCH = "<<conf.getData().hasPCH;
        qDebug()<<"  hasFCS = "<<conf.getData().hasFCS;
        qDebug()<<"  hasCountrate = "<<conf.getData().hasCountrate;
        qDebug()<<"  channelNames = "<<conf.getData().channelNames;
        qDebug()<<"  datacolumns.size = "<<conf.getData().datacolumns.size();
        qDebug()<<"  datacolumnid = "<<conf.getData().datacolumnid.size();
        qDebug()<<"  fcsdatasets.size = "<<conf.getData().fcsdatasets.size();
        for (int i=0; i<conf.getData().fcsdatasets.size(); i++) {
            qDebug()<<"    fcsdatasets["<<i<<"].acqtime="<<conf.getData().fcsdatasets[i].acqtime;
            qDebug()<<"    fcsdatasets["<<i<<"].channel="<<conf.getData().fcsdatasets[i].channel;
            qDebug()<<"    fcsdatasets["<<i<<"].repetition="<<conf.getData().fcsdatasets[i].repetition;
            qDebug()<<"    fcsdatasets["<<i<<"].kinetic="<<conf.getData().fcsdatasets[i].kinetic;
            qDebug()<<"    fcsdatasets["<<i<<"].position="<<conf.getData().fcsdatasets[i].position;
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
        qDebug()<<"  positions = "<<conf.getData().positions;
        qDebug()<<"  kinetics = "<<conf.getData().kinetics;
        qDebug()<<"  repetitions = "<<conf.getData().repetitions;
        qDebug()<<"  channels = "<<conf.getData().channels;
        qDebug()<<"  hasPCH = "<<conf.getData().hasPCH;
        qDebug()<<"  hasFCS = "<<conf.getData().hasFCS;
        qDebug()<<"  hasCountrate = "<<conf.getData().hasCountrate;
        qDebug()<<"  channelNames = "<<conf.getData().channelNames;
        qDebug()<<"  datacolumns.size = "<<conf.getData().datacolumns.size();
        qDebug()<<"  datacolumnid = "<<conf.getData().datacolumnid.size();
        qDebug()<<"  fcsdatasets.size = "<<conf.getData().fcsdatasets.size();
        for (int i=0; i<conf.getData().fcsdatasets.size(); i++) {
            qDebug()<<"    fcsdatasets["<<i<<"].acqtime="<<conf.getData().fcsdatasets[i].acqtime;
            qDebug()<<"    fcsdatasets["<<i<<"].channel="<<conf.getData().fcsdatasets[i].channel;
            qDebug()<<"    fcsdatasets["<<i<<"].repetition="<<conf.getData().fcsdatasets[i].repetition;
            qDebug()<<"    fcsdatasets["<<i<<"].kinetic="<<conf.getData().fcsdatasets[i].kinetic;
            qDebug()<<"    fcsdatasets["<<i<<"].position="<<conf.getData().fcsdatasets[i].position;
        }

    } catch (std::exception& E) {
        qDebug()<<fn<<": \n   ERROR: "<<E.what();
    }

    return 0;
}
