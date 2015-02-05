/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2014-11-14 18:30:07 +0100 (Fr, 14 Nov 2014) $  (revision $Rev: 3610 $)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

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

bool QFExporterImageSeries::writeFrameUINT8(const uint8_t *data)
{
    return intWriteFrameUINT8(data);
}

