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
#include "qfrdrimagemaskedittools.h"

class QFCorrelationMaskTools : public QFRDRImageMaskEditTools
{
        Q_OBJECT
    public:
        explicit QFCorrelationMaskTools(QWidget *parentWidget = 0, const QString& settingsPrefix="");
        
        void registerCorrelationToolsToMenu(QMenu* menu) const;
        QAction* get_actFindErroneousPixels() const { return actFindErroneousPixels; }

        virtual void setRDR(QFRawDataRecord* rdr);

    public slots:
        /** \brief mask ACF runs according to statistics (find erroneous pixels) */
        void findErroneousPixels();
    protected:
        QAction* actFindErroneousPixels;
        QFRDRFCSDataInterface* fcs;

        
};

#endif // QFCORRELATIONMASKTOOLS_H
