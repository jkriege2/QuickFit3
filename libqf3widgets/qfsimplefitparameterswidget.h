#ifndef QFSIMPLEFITPARAMETERSWIDGET_H
#define QFSIMPLEFITPARAMETERSWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QList>
#include <QPointer>
#include "qfdoubleedit.h"
#include "libwid_imexport.h"
#include "qfsimplefitparameterinterface.h"
#include <QStringList>
#include <QSpacerItem>

class QFWIDLIB_EXPORT QFSimpleFitParametersWidget : public QWidget {
        Q_OBJECT
    public:
        explicit QFSimpleFitParametersWidget(QFSimpleFitParameterInterface* paraminterface, QWidget *parent = 0);

        void clearWidgets();
        void setParameterInterface(QFSimpleFitParameterInterface* paraminterface);
        void addParameter(const QString& id, const QString& label);
        void setParameters(const QStringList& ids, const QStringList& labels);

    signals:
        void parameterChanged(QString id, double value);
        
    public slots:
        void updateWidgetValues();

    protected:
        bool updating;

        QFSimpleFitParameterInterface* paraminterface;

        struct subWidget {
            QString id;
            QPointer<QLabel> label;
            QPointer<QFDoubleEdit> edit;
        };
        QList<subWidget> widgets;

        QGridLayout* mainLayout;
        QSpacerItem* spacer;

        QString getIDForWidget(QObject *edit);


    protected slots:
        void valueChanged(double value);
        
};

#endif // QFSIMPLEFITPARAMETERSWIDGET_H
