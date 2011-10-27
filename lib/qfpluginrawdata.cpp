#include "qfpluginrawdata.h"
#include <QDebug>

QFPluginRawDataRecordBase::QFPluginRawDataRecordBase() {
    //qDebug()<<"QFPluginRawDataRecordBase::QFPluginRawDataRecordBase()";
    services=NULL;
    project=NULL;
    settings=NULL;
    parentWidget=NULL;

}

QFPluginRawDataRecordBase::~QFPluginRawDataRecordBase() {

}

void QFPluginRawDataRecordBase::getQFLibVersion(int& major, int& minor) const { \
        major=QF3LIB_APIVERSION_MAJOR; \
        minor=QF3LIB_APIVERSION_MINOR;\
    }


void QFPluginRawDataRecordBase::setProject(QFProject* project) {
    this->project=project;
}

void QFPluginRawDataRecordBase::setServices(QFPluginServices* services) {
    this->services=services;
}

void QFPluginRawDataRecordBase::setSettings(ProgramOptions* settings) {
    this->settings=settings;
}

void QFPluginRawDataRecordBase::setParentWidget(QWidget* parentWidget) {
    this->parentWidget=parentWidget;
}
