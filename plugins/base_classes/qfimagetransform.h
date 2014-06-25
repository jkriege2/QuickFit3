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
#include <QGroupBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QScrollArea>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QComboBox>

class QFImageTransformItemWidget; // forward

class QFImageTransformWidget: public QWidget {
        Q_OBJECT
    public:
        explicit QFImageTransformWidget(QWidget* parent=NULL);
        virtual ~QFImageTransformWidget();

        bool transform(const QVector<double>& input, int width, int height, QVector<double>& output, int& width_out, int& height_out);
    public slots:
        void clear();
        void load(const QString& filename=QString());
        void save(const QString& filename=QString());
    signals:
        void propertiesChanged();
    protected slots:
        void emitPropsChanged(QFImageTransformItemWidget* widget);
        void deleteWidget(QFImageTransformItemWidget* widget);
        void moveWidgetsUp(QFImageTransformItemWidget* widget);
        void moveWidgetsDown(QFImageTransformItemWidget* widget);
        void addWidget();
    protected:
        void connectWidget(QFImageTransformItemWidget* widget, bool conn=true);
        QList<QFImageTransformItemWidget*> items;
        QAction* actAdd;
        QAction* actClear;
        QAction* actLoad;
        QAction* actSave;
        QScrollArea* area;
        QVBoxLayout* layItems;
        QWidget* widMain;
};


class QFImageTransformItemWidget: public QGroupBox {
        Q_OBJECT
    public:
        explicit QFImageTransformItemWidget(const QString& title, QWidget* parent=NULL);
        virtual ~QFImageTransformItemWidget();
        virtual void readSettings(QSettings& settings, const QString& prefix);
        virtual void writeSettings(QSettings& settings, const QString& prefix) const;
        virtual bool transform(const QVector<double>& input, int width, int height, QVector<double>& output, int& width_out, int& height_out);

        static QList<QFImageTransformItemWidget*> loadSettings(const QString& filename, QWidget *parent=NULL);
        static void saveSettings(const QString& filename, const QList<QFImageTransformItemWidget*>& widgets);
        static QStringList getItemIDs();
        static QFImageTransformItemWidget* createItem(const QString& id,  QWidget *parent);
    protected:
        QFormLayout* layForm;
        //QLabel* labHeader;
        QAction* actMoveUp;
        QAction* actMoveDown;
        QAction* actDelete;
        bool doEmit;
    protected slots:
        void upClicked();
        void downClicked();
        void deleteClicked();
        void emitPropChange();
    signals:
        void deleteWidget(QFImageTransformItemWidget* widget);
        void moveWidgetsUp(QFImageTransformItemWidget* widget);
        void moveWidgetsDown(QFImageTransformItemWidget* widget);
        void propertiesChanged(QFImageTransformItemWidget* widget);
};


class QFITWBlur: public QFImageTransformItemWidget {
        Q_OBJECT
    public:
        explicit QFITWBlur(QWidget* parent=NULL);
        virtual ~QFITWBlur();
        virtual void readSettings(QSettings& settings, const QString& prefix);
        virtual void writeSettings(QSettings& settings, const QString& prefix) const;
        virtual bool transform(const QVector<double>& input, int width, int height, QVector<double>& output, int& width_out, int& height_out);
    protected:
        QDoubleSpinBox* spinSigma;
};



class QFITWMedianFilter: public QFImageTransformItemWidget {
        Q_OBJECT
    public:
        explicit QFITWMedianFilter(QWidget* parent=NULL);
        virtual void readSettings(QSettings& settings, const QString& prefix);
        virtual void writeSettings(QSettings& settings, const QString& prefix) const;
        virtual bool transform(const QVector<double>& input, int width, int height, QVector<double>& output, int& width_out, int& height_out);
    protected:
        QSpinBox* spinSigma;
};


class QFITWResize: public QFImageTransformItemWidget {
        Q_OBJECT
    public:
        explicit QFITWResize(QWidget* parent=NULL);
        virtual void readSettings(QSettings& settings, const QString& prefix);
        virtual void writeSettings(QSettings& settings, const QString& prefix) const;
        virtual bool transform(const QVector<double>& input, int width, int height, QVector<double>& output, int& width_out, int& height_out);
    protected:
        QSpinBox* spinWidth;
        QSpinBox* spinHeight;
        QComboBox* cmbInterpolation;
};

class QFITWAverageDir: public QFImageTransformItemWidget {
        Q_OBJECT
    public:
        explicit QFITWAverageDir(int dir, bool samesize, QWidget* parent=NULL);
        virtual void readSettings(QSettings& settings, const QString& prefix);
        virtual void writeSettings(QSettings& settings, const QString& prefix) const;
        virtual bool transform(const QVector<double>& input, int width, int height, QVector<double>& output, int& width_out, int& height_out);
    protected:
        int dir;
        bool samesize;
};

class QFITWExpression: public QFImageTransformItemWidget {
        Q_OBJECT
    public:
        explicit QFITWExpression(QWidget* parent=NULL);
        virtual void readSettings(QSettings& settings, const QString& prefix);
        virtual void writeSettings(QSettings& settings, const QString& prefix) const;
        virtual bool transform(const QVector<double>& input, int width, int height, QVector<double>& output, int& width_out, int& height_out);
    protected:
        QLineEdit* edtExpression;
};





#endif // QFIMAGETRANSFORM_H
