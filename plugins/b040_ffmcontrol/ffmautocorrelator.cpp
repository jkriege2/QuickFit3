#include "ffmautocorrelator.h"

FFMAutoCorrelator::FFMAutoCorrelator(QObject *parent) :
    QObject(parent)
{

}

FFMAutoCorrelator::~FFMAutoCorrelator()
{
    /*
    delete sm_data_out;
    delete sm_data_in;
    delete sm_fitdata_out;
    delete sm_fitdata_in;
    */
}

void FFMAutoCorrelator::startAlv()
{
    //void setLog(QtLogFile *logfile);
    AlvSoftware = new QProcess(this);
    AlvSoftware->setWorkingDirectory(QString("C:/Programme/ALV_V3.0"));
    AlvSoftware->start(QString("C:/Programme/ALV_V3.0/a5e32dr.exe"), QIODevice::ReadOnly);
}

void FFMAutoCorrelator::prepare_shared_memory()
{
    sm_data_out = new QSharedMemory(QString("A5E32 Data Out") , this);
    sm_data_in = new QSharedMemory(QString("A5E32 Data In") , this);
    sm_fitdata_out = new QSharedMemory(QString("A5E32 Fit Data Out") , this);
    sm_fitdata_in = new QSharedMemory(QString("A5E32 Fit Data In") , this);

    //qDebug() << "autocorrelator: called 2";
    bool ok = sm_data_out->create(8192, QSharedMemory::ReadWrite);
   // if(!ok) log->log_error_linebreak(QString("An Error occured while creating the shared memory \'A5E32 Data Out\':") + sm_data_out->errorString());
    ok = sm_data_in->create(128, QSharedMemory::ReadWrite);
   // if(!ok) log->log_error_linebreak(QString("An Error occured while creating the shared memory \'A5E32 Data In\':") + sm_data_in->errorString());
    ok = sm_fitdata_out->create(8192, QSharedMemory::ReadWrite);
   // if(!ok) log->log_error_linebreak(QString("An Error occured while creating the shared memory \'A5E32 Fit Data Out\':") + sm_fitdata_out->errorString());
    ok = sm_fitdata_in->create(128, QSharedMemory::ReadWrite);
   // if(!ok) log->log_error_linebreak(QString("An Error occured while creating the shared memory \'A5E32 Fit Data In\':") + sm_fitdata_in->errorString());

}
///////////////// END TESTFUNCTION
