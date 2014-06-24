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

#ifndef QFIMAGETRANSFORM_H
#define QFIMAGETRANSFORM_H

#include <QObject>
#include <QVector>
#include <QWidget>
#include <QSettings>
#include <QFormLayout>
#include <QLabel>

class QFImageTransformWidget: public QWidget {
        Q_OBJECT
    public:
        explicit QFImageTransformWidget(const QString& title, QWidget* parent=NULL);
        virtual ~QFImageTransformWidget();
        virtual void readSettings(QSettings& settings, const QString& prefix);
        virtual void writeSettings(QSettings& settings, const QString& prefix) const;
        virtual bool transform(const QVector<double>& input, int width, int height, QVector<double>& output, int& width_out, int& height_out);

        static QList<QFImageTransformWidget*> loadSettings(const QString& filename, QWidget *parent=NULL);
        static void saveSettings(const QString& filename, const QList<QFImageTransformWidget*>& widgets);
    protected:
        QFormLayout* layForm;
        QLabel* labHeader;
};


class QFITWBlur: public QFImageTransformWidget {
        Q_OBJECT
    public:
        explicit QFITWBlur(QWidget* parent=NULL);
        virtual ~QFITWBlur();
        virtual void readSettings(QSettings& settings, const QString& prefix);
        virtual void writeSettings(QSettings& settings, const QString& prefix) const;
        virtual bool transform(const QVector<double>& input, int width, int height, QVector<double>& output, int& width_out, int& height_out);
};

#endif // QFIMAGETRANSFORM_H
