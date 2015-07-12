/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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

#ifndef JKAUTOOUTPUTTIMER_H
#define JKAUTOOUTPUTTIMER_H

#include <QElapsedTimer>
#include <QString>
#include "libwid_imexport.h"

class QFWIDQFWIDLIB_EXPORT JKAutoOutputTimer : public QElapsedTimer
{
    public:
        explicit JKAutoOutputTimer(const QString& message);
        ~JKAutoOutputTimer();
        void write(const QString& message) const;
    protected:
        QString message;
        QString indent;
        static int global_indent;
        
};

#endif // JKAUTOOUTPUTTIMER_H
