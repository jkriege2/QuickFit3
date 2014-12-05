/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/



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

Q_DECLARE_INTERFACE( QFRDRImageMaskInterface,
                     "www.dkfz.de.b040.quickfit3.fcsplugin.QFRDRImageMaskInterface/1.0")



#endif // QFRDRIMAGEMASK_H
