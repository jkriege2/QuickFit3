#include "qftools.h"
#include "../flex_sin_tools.h"
#include <QDebug>

int main(int argc, char *argv[])
{


    QString fn;
    QString mode;
    unsigned int channelCount;
    unsigned int runCount;
    bool crossCorrelation, autoCorrelation;

    try {
        fn="../examples/fcs_testdata/A488_5nM_25uW_0_quad.sin";
        qDebug()<<"== ANALYZE ===============================================================";
        FLEX_analyze(fn, mode, channelCount, runCount, crossCorrelation, autoCorrelation);
        qDebug()<<fn<<": \n   mode="<<mode<<"  channelCount="<< channelCount<<"  runCOunt="<< runCount<<"  crossCorrelation="<< crossCorrelation<<"  autoCorrelation="<< autoCorrelation;
        qDebug()<<"== READ ==================================================================";
        FLEX_DATA data;
        FLEX_readFile(fn, data);
        //qDebug()<<"   tau: "<<data.corr_tau;
        //qDebug()<<"   counts_timeaxis: "<<data.counts_time;
        //qDebug()<<"   properties: "<<data.properties;
    } catch (std::exception& E) {
        qDebug()<<fn<<": \n   ERROR: "<<E.what();
    }

    try {
        fn="../examples/fcs_testdata/A488_5nM_25uW_0_single.sin";
        qDebug()<<"== ANALYZE ===============================================================";
        FLEX_analyze(fn, mode, channelCount, runCount, crossCorrelation, autoCorrelation);
        qDebug()<<fn<<": \n   mode="<<mode<<"  channelCount="<< channelCount<<"  runCOunt="<< runCount<<"  crossCorrelation="<< crossCorrelation<<"  autoCorrelation="<< autoCorrelation;
        qDebug()<<"== READ ==================================================================";
        FLEX_DATA data;
        FLEX_readFile(fn, data);
        //qDebug()<<"   tau: "<<data.corr_tau;
        //qDebug()<<"   counts_timeaxis: "<<data.counts_time;
        //qDebug()<<"   properties: "<<data.properties;
    } catch (std::exception& E) {
        qDebug()<<fn<<": \n   ERROR: "<<E.what();
    }
    return 0;
}
