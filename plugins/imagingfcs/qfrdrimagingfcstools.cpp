#include "qfrdrimagingfcstools.h"
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QDebug>


void readB040SPIMExperimentConfigFile(QSettings& set, double& frametime, double& baseline_offset, QString& backgroundfile, int& image_width, int& image_height) {
    if (set.contains("acquisition/image_width")) image_width=set.value("acquisition/image_width", image_width).toInt();
    if (set.contains("acquisition/image_height")) image_height=set.value("acquisition/image_height", image_height).toInt();
    if (set.contains("acquisition/frame_time")) frametime=set.value("acquisition/frame_time", frametime).toDouble()*1e6;
    else if (set.contains("acquisition/frame_rate")) frametime=1.0/set.value("acquisition/frame_rate", frametime).toDouble()*1e6;
    baseline_offset=set.value("acquisition/baseline_offset", baseline_offset).toDouble();
    //backgroundfile="";
    int fcnt=set.value("files/count", 0).toInt();
    for (int f=0; f<fcnt; f++) {
        QString fn=QFileInfo(set.fileName()).dir().absoluteFilePath(set.value("files/name"+QString::number(f), "").toString());
        //QString ft=set.value("files/type"+QString::number(f), "").toString();
        QString fd=set.value("files/description"+QString::number(f), "").toString();
        if (fd.toLower().simplified().contains("background")) {
            backgroundfile=fn;
        }
    }
}

void readB040SPIMExperimentConfigFileAssociatedFiles(QSettings& set, QStringList& files, QStringList& description,  QStringList& type) {
    int fcnt=set.value("files/count", 0).toInt();
    //qDebug()<<"fcnt="<<fcnt;
    for (int f=0; f<fcnt; f++) {
        QString fn=QFileInfo(set.fileName()).dir().absoluteFilePath(set.value("files/name"+QString::number(f), "").toString());
        QString ft=set.value("files/type"+QString::number(f), "").toString();
        QString fd=set.value("files/description"+QString::number(f), "").toString();
        files.append(fn);
        description.append(fd);
        type.append(ft);
    }
}


void appendCategorizedFilesFromB040SPIMConfig(QSettings& settings, QStringList& files, QStringList& files_types, QStringList& files_descriptions) {
    QStringList inifiles;
    QStringList inifiletypes;
    QStringList inifiledescriptions;
    readB040SPIMExperimentConfigFileAssociatedFiles(settings, inifiles, inifiledescriptions, inifiletypes);
    for (int i=0; i<inifiles.size(); i++) {
        if (inifiletypes[i].toLower().contains("tiff")) {
            if (inifiledescriptions[i].toLower().contains("overview") && inifiledescriptions[i].toLower().contains("before") && inifiledescriptions[i].toLower().contains("transmi")) {
                files<<inifiles[i];
                files_types<<"overview_before_transmission";
                files_descriptions<<inifiledescriptions[i];
            } else if (inifiledescriptions[i].toLower().contains("overview") && inifiledescriptions[i].toLower().contains("after") && inifiledescriptions[i].toLower().contains("transmi")) {
                files<<inifiles[i];
                files_types<<"overview_after_transmission";
                files_descriptions<<inifiledescriptions[i];
            } else if (inifiledescriptions[i].toLower().contains("overview") && inifiledescriptions[i].toLower().contains("before")) {
                files<<inifiles[i];
                files_types<<"overview_before";
                files_descriptions<<inifiledescriptions[i];
            } else if (inifiledescriptions[i].toLower().contains("overview") && inifiledescriptions[i].toLower().contains("after")) {
                files<<inifiles[i];
                files_types<<"overview_after";
                files_descriptions<<inifiledescriptions[i];
            } else if (inifiledescriptions[i].toLower().contains("overview") && inifiledescriptions[i].toLower().contains("preview")) {
                files<<inifiles[i];
                files_types<<"overview_after";
                files_descriptions<<inifiledescriptions[i];
            } else if (inifiledescriptions[i].toLower().contains("z stack")) {
                files<<inifiles[i];
                files_types<<"zstack";
                files_descriptions<<inifiledescriptions[i];
            }
        }
    }

}
