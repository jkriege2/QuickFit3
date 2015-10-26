/*
Copyright (c) 2012-2015 by Sebastian Isbaner

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

#ifndef FCSCURVE_H
#define FCSCURVE_H

#include <QVector>
#include <QString>
#include "parameters.h"

class FCScurve : public QVector<double>
{
    QString name;
    int numberOfPhotonsUsed;
    const FCSparams_t* FCSparams;
public:
    FCScurve();
    FCScurve(QString name, FCSparams_t* params=NULL);

    GetSetMacro(QString, name);
    GetSetMacro(int, numberOfPhotonsUsed);
    const FCSparams_t* getFCSparams() const {return FCSparams;}
    void set_FCSparams(const FCSparams_t& params) {FCSparams=&params;}
    void set_FCSparams(const FCSparams_t* params) {FCSparams=params;}

};

#endif // FCSCURVE_H
