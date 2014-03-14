#ifndef QFRDRIMAGEMASKTOOLS_H
#define QFRDRIMAGEMASKTOOLS_H

#include "qfrdrimagemask.h"

class QFRDRImageMaskTools : public QFRDRImageMaskInterface {
    public:
        QFRDRImageMaskTools();
        virtual ~QFRDRImageMaskTools();

        virtual void maskLoad(const QString& filename);
        virtual void maskLoadFromString(const QString &maskstring);
        virtual void maskSave(const QString& filename) const ;
        virtual QString maskToString() const;
        virtual QString maskToIndexString(QChar separator=QChar(',')) const;
        virtual void maskLoadFromIndexString(const QString& data, QChar separator=QChar(','));
        virtual void maskClear();
        virtual void maskSetAll();
        virtual void maskUnset(uint32_t x, uint32_t y);
        virtual void maskSet(uint32_t x, uint32_t y, bool value=true);
        virtual void maskToggle(uint32_t x, uint32_t y);
        virtual void maskInvert();
        virtual bool maskGet(uint32_t x, uint32_t y) const ;
        virtual bool* maskGet() const;
        virtual uint32_t maskGetWidth() const;
        virtual uint32_t maskGetHeight() const;
        virtual long maskGetCount() const;
        virtual void maskSetIdx(uint32_t idx, bool value=true);
        virtual bool maskGetIdx(uint32_t idx) const;
        virtual QList<int> maskToIndexList();
        virtual void maskMaskChangedEvent();
    protected:
        virtual void maskInit(uint32_t w, uint32_t h);
        virtual void maskDelete();

        virtual QString maskToListString(const QString& coordinate_separator=QString(", "), const QString& pixel_separator=QString("; ")) const;
        virtual void maskLoadFromListString(const QString& maskstring, QChar coordinate_separator=QChar(','), QChar pixel_separator=QChar(';'));


    private:
        bool* mask;
        uint32_t mask_w;
        uint32_t mask_h;

};

#endif // QFRDRIMAGEMASKTOOLS_H
