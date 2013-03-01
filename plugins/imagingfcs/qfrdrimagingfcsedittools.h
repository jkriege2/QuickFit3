#ifndef QFRDRIMAGINGFCSEDITTOOLS_H
#define QFRDRIMAGINGFCSEDITTOOLS_H

#include <QObject>
#include <QAction>
#include <QMenu>
#include <QToolBar>
#include "qfrawdatarecord.h"

class QFRDRImagingFCSData; // forward

class QFRDRImagingFCSEditTools : public QObject
{
        Q_OBJECT
    public:
        explicit QFRDRImagingFCSEditTools(QObject *parent = 0);

        void registerToMenu(QMenu* menu);
        void registerToToolbar(QToolBar* menu);

        void setRDR(QFRawDataRecord* record);

    signals:
        
    public slots:

    protected slots:
        void excludeSegments();

    protected:
        QFRDRImagingFCSData* imFCS;

        QAction* actExcludeSegments;
        
};

#endif // QFRDRIMAGINGFCSEDITTOOLS_H
