/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    last modification: $LastChangedDate$  (revision $Rev$)

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

#ifndef QFPREPROCESSORMAGIC_H
#define QFPREPROCESSORMAGIC_H

#include "qfpreprocessormagic_private.h"


#define QFPPM_DEC(x) QFPPM_DEC_I(x)
#define QFPPM_DEC_I(x) QFPPM_DEC_ ## x

#define QFPPM_INC(x) QFPPM_INC_I(x)
#define QFPPM_INC_I(x) QFPPM_INC_ ## x

#define QFPPM_COUNT_START 0







#endif // QFPREPROCESSORMAGIC_H
