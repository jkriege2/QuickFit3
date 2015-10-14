#ifndef NIDRIVER_H
#define NIDRIVER_H
#include <stdint.h>
#include <QDebug>
#include <NIDAQmx.h>

class MainWindow; // forward

extern MainWindow* NIMainWindow;

TaskHandle setupNI(MainWindow* win);
TaskHandle setupNIPreview(MainWindow* win);
void startNI(TaskHandle& taskHandle);
void stopNI(TaskHandle& taskHandle);

#define DAQmxErrChk(functionCall) if(!hasError && DAQmxFailed(error=(functionCall)) ) {hasError=true;}

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData);
int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData);


#endif // NIDRIVER_H
