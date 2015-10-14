#include "nidriver.h"
#include "mainwindow.h"

MainWindow* NIMainWindow=NULL;
int NIchannels=0;

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 /*everyNsamplesEventType*/, uInt32 nSamples, void */*callbackData*/)
{
    bool hasError=false;
    int32       error=0;
    char        errBuff[2048]={'\0'};
    static int  totalRead=0;
    int32       read=0;
    //printf("EveryNCallback(%d, %u): ", everyNsamplesEventType, nSamples);
    if (nSamples>0) {
        float64*     data=(float64*)malloc(nSamples*NIchannels*sizeof(float64));

        /*********************************************/
        // DAQmx Read Code
        /*********************************************/
        DAQmxErrChk (DAQmxReadAnalogF64(taskHandle,nSamples,DAQmx_Val_WaitInfinitely,DAQmx_Val_GroupByScanNumber,data,nSamples*NIchannels,&read,NULL));
        if( read>0 ) {
            QList<QVector<double> > d;
            for (int i=0; i<NIchannels; i++) {
                d.append(QVector<double>());
            }
            for (int32 t=0; t<read; t++) {
                for (int i=0; i<NIchannels; i++) {
                    d[i].append(data[t*NIchannels+i]);
                }
            }
            NIMainWindow->sendData(d);
            //printf("Acquired %d samples. Total %d\r",read,totalRead+=read);
            //fflush(stdout);
        }

        if (hasError) {
            if( DAQmxFailed(error) ) {
                DAQmxGetExtendedErrorInfo(errBuff,2048);
                /*********************************************/
                // DAQmx Stop Code
                /*********************************************/
                DAQmxStopTask(taskHandle);
                DAQmxClearTask(taskHandle);
                {
                    NIMainWindow->sendLogMessage(QString("DAQmx Error: %1").arg(errBuff));
                    NIMainWindow->sendSetAcquisitionEnabled(false);
                }
            }
        }
        free(data);
    }
    return 0;
}

int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void */*callbackData*/)
{
    bool hasError=false;
    int32   error=0;
    char    errBuff[2048]={'\0'};


    NIMainWindow->sendLogMessage(QString("Acquisition Completed!!!"));
    NIMainWindow->sendSetAcquisitionEnabled(false);

    // Check to see if an error stopped the task.
    DAQmxErrChk (status);

    if (hasError) {
        if( DAQmxFailed(error) ) {
            DAQmxGetExtendedErrorInfo(errBuff,2048);
            DAQmxClearTask(taskHandle);
            {
                NIMainWindow->sendLogMessage(QString("DAQmx Error: %1").arg(errBuff));
                NIMainWindow->getAcquisitionAction()->setChecked(false);
            }
        }
    }
    return 0;
}


TaskHandle setupNI(MainWindow *win)
{
    bool hasError=false;
    int32       error=0;
    char        errBuff[2048]={'\0'};
    NIMainWindow->addLogMessage(QString("DAQmx Start Acquisition Setup ..."));

    NIMainWindow=win;
    TaskHandle taskHandle=0;

    DAQmxErrChk (DAQmxCreateTask("NIDAQmx_AnalogReader_NITask",&taskHandle));
    NIchannels=0;
    for (int i=0; i<NIMainWindow->getChannels()->getChannelCount(); i++) {
        int32 term=DAQmx_Val_Cfg_Default;
        switch(NIMainWindow->getChannels()->getChannel(i).type) {
            case NIC_Analog_SingleEnded:
                term=DAQmx_Val_RSE;
                break;
            case NIC_Analog_NR_SingleEnded:
                term=DAQmx_Val_NRSE;
                break;
            case NIC_Analog_Differential:
                term=DAQmx_Val_Diff;
                break;
            case NIC_Analog_Pseudo_Differential:
                term=DAQmx_Val_PseudoDiff;
                break;
            default:
                term=DAQmx_Val_Cfg_Default;
                break;
        }

        DAQmxErrChk (DAQmxCreateAIVoltageChan(taskHandle, NIMainWindow->getChannels()->getChannel(i).physicalChannel.toLatin1().data(),
                                                          "",//NIMainWindow->getChannels()->getChannel(i).name.toLatin1().data(),
                                                          term,
                                                          NIMainWindow->getChannels()->getChannel(i).minVal,
                                                          NIMainWindow->getChannels()->getChannel(i).maxVal,DAQmx_Val_Volts,NULL));
        if (hasError) break;
        NIMainWindow->addLogMessage(QString("    added channel '%1' (physical: '%2')").arg(NIMainWindow->getChannels()->getChannel(i).name).arg(NIMainWindow->getChannels()->getChannel(i).physicalChannel));
        NIchannels++;
    }

    DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle,NIMainWindow->getClockSource().toLocal8Bit().data(),NIMainWindow->getSampleClock(),DAQmx_Val_Rising ,DAQmx_Val_FiniteSamps,win->getSamples()));
    if (NIMainWindow->getStartTrigger()) {
        int32 edge=DAQmx_Val_Rising;
        if (NIMainWindow->getStartTriggerEdge()==1) edge=DAQmx_Val_Falling;
        DAQmxErrChk (DAQmxCfgDigEdgeStartTrig(taskHandle,NIMainWindow->getStartTriggerChannel().toLatin1().data(),edge));
        DAQmxErrChk (DAQmxSetStartTrigRetriggerable(taskHandle,win->getStartTriggerRetriggerable()?1:0));
    }
    if (NIMainWindow->getRefTrigger()) {
        int32 edge=DAQmx_Val_Rising;
        if (NIMainWindow->getRefTriggerEdge()==1) edge=DAQmx_Val_Falling;
        DAQmxErrChk (DAQmxCfgDigEdgeRefTrig(taskHandle,NIMainWindow->getRefTriggerChannel().toLatin1().data(),edge, win->getPretriggerSamples()));
    }


    DAQmxErrChk (DAQmxRegisterEveryNSamplesEvent(taskHandle,DAQmx_Val_Acquired_Into_Buffer,NIMainWindow->getUpdatePlotSamples(),0,EveryNCallback,NULL));
    DAQmxErrChk (DAQmxRegisterDoneEvent(taskHandle,0,DoneCallback,NULL));

    if (!hasError) NIMainWindow->addLogMessage(QString("DAQmx Setup complete !"));

    if (hasError) {
        if( DAQmxFailed(error) )
            DAQmxGetExtendedErrorInfo(errBuff,2048);
        if( taskHandle!=0 ) {
            /*********************************************/
            // DAQmx Stop Code
            /*********************************************/
            DAQmxStopTask(taskHandle);
            DAQmxClearTask(taskHandle);
        }
        if( DAQmxFailed(error) ){
            NIMainWindow->addLogMessage(QString("DAQmx Error: %1").arg(errBuff));
            NIMainWindow->getAcquisitionAction()->setChecked(false);
        }
        taskHandle=0;
    }
    return taskHandle;
}

TaskHandle setupNIPreview(MainWindow *win)
{
    bool hasError=false;
    int32       error=0;
    char        errBuff[2048]={'\0'};
    NIMainWindow->addLogMessage(QString("DAQmx Start Preview Setup ..."));

    NIMainWindow=win;
    TaskHandle taskHandle=0;

    DAQmxErrChk (DAQmxCreateTask("NIDAQmx_AnalogReader_NITask",&taskHandle));
    NIchannels=0;
    for (int i=0; i<NIMainWindow->getChannels()->getChannelCount(); i++) {
        int32 term=DAQmx_Val_Cfg_Default;
        bool addChannel=true;
        switch(NIMainWindow->getChannels()->getChannel(i).type) {
            case NIC_Analog_SingleEnded:
                term=DAQmx_Val_RSE;
                break;
            case NIC_Analog_NR_SingleEnded:
                term=DAQmx_Val_NRSE;
                break;
            case NIC_Analog_Differential:
                term=DAQmx_Val_Diff;
                break;
            case NIC_Analog_Pseudo_Differential:
                term=DAQmx_Val_PseudoDiff;
                break;
            default:
                //term=DAQmx_Val_Cfg_Default;
                addChannel=false;
                break;
        }
        if (addChannel) {
            DAQmxErrChk (DAQmxCreateAIVoltageChan(taskHandle, NIMainWindow->getChannels()->getChannel(i).physicalChannel.toLatin1().data(),
                                                  "",//NIMainWindow->getChannels()->getChannel(i).name.toLatin1().data(),
                                                  term,
                                                  NIMainWindow->getChannels()->getChannel(i).minVal,
                                                  NIMainWindow->getChannels()->getChannel(i).maxVal,DAQmx_Val_Volts,NULL));
            if (hasError) break;
            NIMainWindow->addLogMessage(QString("    added channel '%1' (physical: '%2')").arg(NIMainWindow->getChannels()->getChannel(i).name).arg(NIMainWindow->getChannels()->getChannel(i).physicalChannel));
            NIchannels++;
        }
    }

    DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle,NIMainWindow->getClockSource().toLocal8Bit().data(),NIMainWindow->getPreviewSampleClock(),DAQmx_Val_Rising ,DAQmx_Val_ContSamps , win->getUpdatePreviewPlotSamples()));

    DAQmxErrChk (DAQmxRegisterEveryNSamplesEvent(taskHandle,DAQmx_Val_Acquired_Into_Buffer,NIMainWindow->getUpdatePreviewPlotSamples(),0,EveryNCallback,NULL));
    DAQmxErrChk (DAQmxRegisterDoneEvent(taskHandle,0,DoneCallback,NULL));

    if (!hasError) NIMainWindow->addLogMessage(QString("DAQmx Preview Setup complete !"));

    if (hasError) {
        if( DAQmxFailed(error) )
            DAQmxGetExtendedErrorInfo(errBuff,2048);
        if( taskHandle!=0 ) {
            /*********************************************/
            // DAQmx Stop Code
            /*********************************************/
            DAQmxStopTask(taskHandle);
            DAQmxClearTask(taskHandle);
        }
        if( DAQmxFailed(error) ){
            NIMainWindow->addLogMessage(QString("DAQmx Error: %1").arg(errBuff));
            NIMainWindow->getAcquisitionAction()->setChecked(false);
        }
        taskHandle=0;
    }
    return taskHandle;
}

void startNI(TaskHandle& taskHandle)
{
    bool hasError=false;
    int32       error=0;
    char        errBuff[2048]={'\0'};
    DAQmxErrChk (DAQmxStartTask(taskHandle));
    NIMainWindow->addLogMessage(QString("DAQmx Started !"));
    if (hasError) {
        if( DAQmxFailed(error) )
            DAQmxGetExtendedErrorInfo(errBuff,2048);
        if( taskHandle!=0 ) {
            /*********************************************/
            // DAQmx Stop Code
            /*********************************************/
            DAQmxStopTask(taskHandle);
            DAQmxClearTask(taskHandle);
        }
        if( DAQmxFailed(error) ){
            NIMainWindow->addLogMessage(QString("DAQmx Error: %1").arg(errBuff));
            NIMainWindow->getAcquisitionAction()->setChecked(false);
        }
        taskHandle=0;
    }
    return ;
}


void stopNI(TaskHandle &taskHandle)
{
    bool hasError=false;
    int32       error=0;
    char        errBuff[2048]={'\0'};
    if (taskHandle!=0) {
        DAQmxStopTask(taskHandle);
        DAQmxClearTask(taskHandle);
        if( DAQmxFailed(error) ){
            NIMainWindow->addLogMessage(QString("DAQmx Error: %1").arg(errBuff));
            NIMainWindow->getAcquisitionAction()->setChecked(false);
        }
    }
    taskHandle=0;

}
