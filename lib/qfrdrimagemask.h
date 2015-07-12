/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/



#ifndef QFRDRIMAGEMASK_H
#define QFRDRIMAGEMASK_H



#include "qfplugin.h"
#include <stdint.h>
#include <QString>
#include <QList>
#include <QVector>

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
        virtual void maskUnset(uint32_t x, uint32_t y)=0;
        virtual void maskSet(uint32_t x, uint32_t y, bool value=true)=0;
        virtual void maskToggle(uint32_t x, uint32_t y)=0;
        virtual void maskInvert()=0;
        virtual bool maskGet(uint32_t x, uint32_t y) const =0;
        virtual bool* maskGet() const=0;
        virtual void maskSet(const bool* mask) const=0;
        virtual uint32_t maskGetWidth() const=0;
        virtual uint32_t maskGetHeight() const=0;
        virtual void maskMaskChangedEvent()=0;

};



inline QVector<bool> QFRDRImageMaskInterface_getMaskAsBoolVec(QFRDRImageMaskInterface* rs, bool inverted=true) {
    QVector<bool> b;
    if (rs) {
        uint32_t count=rs->maskGetWidth()*rs->maskGetHeight();
        const bool* mask=rs->maskGet();
        if (inverted) {
            for (uint32_t i=0; i<count; i++) {
                b.append(!mask[i]);
            }
        } else {
            for (uint32_t i=0; i<count; i++) {
                b.append(mask[i]);
            }
        }
    }
    return b;
}

Q_DECLARE_INTERFACE( QFRDRImageMaskInterface,
                     "www.dkfz.de.b040.quickfit3.fcsplugin.QFRDRImageMaskInterface/1.0")



#endif // QFRDRIMAGEMASK_H
