#ifndef QFCORRELATIONMASKTOOLS_H
#define QFCORRELATIONMASKTOOLS_H

#include <QObject>
#include <QMenu>
#include <QAction>
#include <QSet>
#include <QList>
#include "qfrawdatarecord.h"
#include "qfrdrrunselection.h"
#include "qfrdrfcsdatainterface.h"
#include "qfrdrimagemask.h"
#include "qffinderroneouscorrelationdlg.h"

class QFCorrelationMaskTools : public QObject
{
        Q_OBJECT
    public:
        explicit QFCorrelationMaskTools(QWidget *parentWidget = 0);
        
        void setRDR(QFRawDataRecord* rdr);
        void registerCorrelationToolsToMenu(QMenu* menu) const;
        void registerMaskToolsToMenu(QMenu* menu) const;
        QAction* get_actFindErroneousPixels() const { return actFindErroneousPixels; }
        QAction* get_actCopyMask() const { return actCopyMask; }
        QAction* get_actPasteMask() const { return actPasteMask; }
        QAction* get_actLoadMask() const { return actLoadMask; }
        QAction* get_actSaveMask() const { return actSaveMask; }


    signals:
        void rawDataChanged();
    public slots:
        /** \brief mask ACF runs according to statistics (find erroneous pixels) */
        void findErroneousPixels();

        /** \brief load a mask file as a selection from hard disk */
        void loadMask();
        /** \brief load a mask file as a selection from the clipboard */
        void pasteMask();
        /** \brief save mask to harddisk */
        void saveMask();
        /** \brief copy mask to clipboard */
        void copyMask();
    protected:
        QWidget* parentWidget;
        QFRawDataRecord* rdr;
        QAction* actFindErroneousPixels;
        QAction* actCopyMask;
        QAction* actPasteMask;
        QAction* actLoadMask;
        QAction* actSaveMask;

        QFRDRImageMaskInterface* imagemask;
        QFRDRRunSelectionsInterface* runselection;
        QFRDRFCSDataInterface* fcs;

    protected slots:
        void setLeaveout(QList<int> leavout, bool clearOld=false);

        
};

#endif // QFCORRELATIONMASKTOOLS_H
