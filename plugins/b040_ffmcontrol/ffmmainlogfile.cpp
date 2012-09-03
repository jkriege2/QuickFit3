#include "ffmmainlogfile.h"

FFMMainLogFile* FFMMainLogFile::logfile_pointer = NULL;
bool FFMMainLogFile::instanceExists;

FFMMainLogFile::FFMMainLogFile(QWidget *parent)
{
    log_text_linebreak("Logfile created.");
    instanceExists = true;
}

FFMMainLogFile::~FFMMainLogFile()
{
 //   logfile_pointer=NULL;
    //exists = false;
}
