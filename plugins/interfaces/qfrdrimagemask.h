#ifndef QFRDRIMAGEMASK_H
#define QFRDRIMAGEMASK_H



#include <QtPlugin>
#include <stdint.h>
#include <QString>
#include <QList>

/*! \brief represents a pixel mask (e.g. a mask marking broken pixels)
    \ingroup qf3rdrdp_imaging_fcs

*/
class QFRDRImageMaskInterface {
    public:
        virtual ~QFRDRImageMaskInterface() {}

        virtual void maskLoad(const QString& filename)=0;
        virtual void maskLoadFromString(const QString& maskstring)=0;
        virtual void maskSave(const QString& filename) const =0;
        virtual QString maskToString() const=0;
        virtual void maskClear()=0;
        virtual void maskSetAll()=0;
        virtual void maskUnset(uint16_t x, uint16_t y)=0;
        virtual void maskSet(uint16_t x, uint16_t y, bool value=true)=0;
        virtual void maskToggle(uint16_t x, uint16_t y)=0;
        virtual void maskInvert()=0;
        virtual bool maskGet(uint16_t x, uint16_t y) const =0;
        virtual bool* maskGet() const=0;
        virtual uint16_t maskGetWidth() const=0;
        virtual uint16_t maskGetHeight() const=0;
        virtual void maskMaskChangedEvent()=0;

};

Q_DECLARE_INTERFACE( QFRDRImageMaskInterface,
                     "www.dkfz.de.b040.quickfit3.fcsplugin.QFRDRImageMaskInterface/1.0")



#endif // QFRDRIMAGEMASK_H
