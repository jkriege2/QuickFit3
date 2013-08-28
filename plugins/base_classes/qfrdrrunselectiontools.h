#ifndef QFRDRRUNSELECTIONTOOLS_H
#define QFRDRRUNSELECTIONTOOLS_H

#include "qfrdrrunselection.h"

class QFRDRRunSelectionTools : public QFRDRRunSelectionsInterface
{
    public:
        explicit QFRDRRunSelectionTools();
        virtual ~QFRDRRunSelectionTools();
        


        virtual void leaveoutLoad(const QString& filename);
        virtual void leaveoutLoadFromString(const QString &leaveoutstring);
        virtual void leaveoutSave(const QString& filename) const ;
        virtual QString leaveoutToString() const;
        virtual QString leaveoutToIndexString(QChar separator=QChar(',')) const;
        virtual void leaveoutLoadFromIndexString(const QString& data, QChar separator=QChar(','));
        virtual void leaveoutSetAll();
        virtual void leaveoutInvert();
        virtual QList<int> leaveoutToIndexList();
        virtual void leaveoutChangedEvent();


    signals:
        
    public slots:
        
};

#endif // QFRDRRUNSELECTIONTOOLS_H
