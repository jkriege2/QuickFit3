/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2014-11-14 18:30:07 +0100 (Fr, 14 Nov 2014) $  (revision $Rev: 3610 $)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

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

#include "qfexporterimageseries.h"
#include <cstdlib>
#include <cmath>
#include<QDebug>
#include "qftools.h"
QFExporterImageSeries::QFExporterImageSeries() {
    width=height=0;
    channels=1;
}

void QFExporterImageSeries::setFrameSize(uint32_t width, uint32_t height, uint32_t channels)
{
    this->width=width;
    this->height=height;
    this->channels=channels;
    this->deltaX=1;
    this->deltaY=1;
    this->deltaZ=1;
    frame=0;
    this->unitname=QObject::tr("pixel");
}

bool QFExporterImageSeries::writeFrameFloat(const float *data)
{
    return intWriteFrameFloat(data);
}

bool QFExporterImageSeries::writeFrameDouble(const double *data)
{
    return intWriteFrameDouble(data);
}

bool QFExporterImageSeries::writeFrameUINT16(const uint16_t *data)
{
    return intWriteFrameUINT16(data);
}

bool QFExporterImageSeries::writeFrameUINT32(const uint32_t *data)
{
    return intWriteFrameUINT32(data);
}

bool QFExporterImageSeries::writeFrameUINT8(const uint8_t *data)
{
    return intWriteFrameUINT8(data);
}

bool QFExporterImageSeries::writeFramesFloat(const float *data, uint32_t frames)
{
    bool ok=true;
    for (uint32_t i=0; i<frames; i++) {
        ok=writeFrameFloat(&(data[i*width*height*channels]));
        if (!ok) return false;
        else frame++;
    }
    return ok;
}

bool QFExporterImageSeries::writeFramesDouble(const double *data, uint32_t frames)
{
    bool ok=true;
    for (uint32_t i=0; i<frames; i++) {
        ok=writeFrameDouble(&(data[i*width*height*channels]));
        if (!ok) return false;
        else frame++;
    }
    return ok;
}

bool QFExporterImageSeries::writeFramesUINT16(const uint16_t *data, uint32_t frames)
{
    bool ok=true;
    for (uint32_t i=0; i<frames; i++) {
        ok=writeFrameUINT16(&(data[i*width*height*channels]));
        if (!ok) return false;
        else frame++;
    }
    return ok;
}

bool QFExporterImageSeries::writeFramesUINT32(const uint32_t *data, uint32_t frames)
{
    bool ok=true;
    for (uint32_t i=0; i<frames; i++) {
        ok=writeFrameUINT32(&(data[i*width*height*channels]));
        if (!ok) return false;
        else frame++;
    }
    return ok;
}

bool QFExporterImageSeries::writeFramesUINT8(const uint8_t *data, uint32_t frames)
{
    bool ok=true;
    for (uint32_t i=0; i<frames; i++) {
        ok=writeFrameUINT8(&(data[i*width*height*channels]));
        if (!ok) return false;
        else frame++;
    }
    return ok;
}

void QFExporterImageSeries::setResolution(double deltaX, double deltaY, double deltaZ, const QString &unitname)
{
    this->deltaX=deltaX;
    this->deltaY=deltaY;
    this->deltaZ=deltaZ;
    this->unitname=unitname;
}

