#ifndef FFMMAINLOGFILE_H
#define FFMMAINLOGFILE_H

#include <QObject>
#include <qtlogfile.h>

class FFMMainLogFile : public QtLogFile
{
    Q_OBJECT
public:
    explicit FFMMainLogFile(QWidget *parent = 0);
    ~FFMMainLogFile();

    static FFMMainLogFile *getInstance(){
        //if(!logfile_pointer){
        if(!instanceExists)
            logfile_pointer = new FFMMainLogFile();
        instanceExists = true;
        return logfile_pointer;
    }

private:
    static FFMMainLogFile *logfile_pointer;
public:// publio only for debugging
    static bool instanceExists;
};

#endif // FFMMAINLOGFILE_H
