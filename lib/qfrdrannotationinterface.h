/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2015-04-02 13:55:22 +0200 (Do, 02 Apr 2015) $  (revision $Rev: 3902 $)

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

#ifndef QFRDRANNOTATIONINTERFACE_H
#define QFRDRANNOTATIONINTERFACE_H

#include <QString>
#include <QVector>

/*! \brief allows to annotate a vector (or with QFRDRImageToRunInterface an image) of data with annotations opf different types
    \ingroup qf3rdrdp_imaging_fcs

*/
class QFRDRAnnotationInterface {
    public:
        virtual ~QFRDRAnnotationInterface() {}

        enum QFRDRAnnotationTypes {
            annotNone,
            annotKategoryIndex,
            annotLabel
        };

        inline static QString QFRDRAnnotationTypesToString(QFRDRAnnotationTypes type) {
            switch(type) {
                default:
                case annotLabel: return QString("label");
                case annotKategoryIndex: return QString("kat");
            }
            return QString("none");
        }

        inline static QFRDRAnnotationTypes StringToQFRDRAnnotationTypes(const QString& type) {
            if (type.toLower()=="kat") return annotKategoryIndex;
            if (type.toLower()=="label") return annotLabel;
            return annotNone;
        }

        virtual int annotAdd(QFRDRAnnotationTypes type, const QString& label, int index)=0;
        virtual int annotAdd(QFRDRAnnotationTypes type, const QString& label, double x, double y=0, double z=0)=0;
        virtual int annotAdd(QFRDRAnnotationTypes type, const QString& label)=0;
        virtual void annotAddValue(int annotation, int index, double data)=0;
        virtual void annotSetLabel(int annotation, const QString& label)=0;
        virtual void annotSetComment(int annotation, const QString& comment)=0;
        virtual QString annotGetComment(int annotation) const=0;


        virtual int annotGetCount() const=0;
        virtual QString annotGetLabel(int annotation) const=0;
        virtual int annotGetIndex(int annotation) const=0;
        virtual QFRDRAnnotationTypes annotGetType(int annotation) const=0;
        virtual QVector<double> annotGetValues(int annotation) const=0;
        virtual double annotGetValue(int annotation, int index, double defaultVal=0) const=0;
        virtual QVector<bool> annotGetValuesSet(int annotation) const=0;
        virtual bool annotIsValueSet(int annotation, int index) const=0;
        virtual QVector<double> annotGetPositionsX(int annotation) const=0;
        virtual QVector<double> annotGetPositionsY(int annotation) const=0;
        virtual QVector<double> annotGetPositionsZ(int annotation) const=0;

        inline  double annotGetPositionX(int annotation) const { return annotGetPositionsX(annotation).value(0, 0); }
        inline  double annotGePositionY(int annotation) const { return annotGetPositionsY(annotation).value(0, 0); }
        inline  double annotGetPositionZ(int annotation) const { return annotGetPositionsZ(annotation).value(0, 0); }

};

Q_DECLARE_INTERFACE( QFRDRAnnotationInterface,
                     "www.dkfz.de.b040.quickfit3.fcsplugin.QFRDRAnnotationInterface/1.0")

#endif // QFRDRANNOTATIONINTERFACE_H

