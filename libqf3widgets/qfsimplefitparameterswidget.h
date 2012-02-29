#ifndef QFSIMPLEFITPARAMETERSWIDGET_H
#define QFSIMPLEFITPARAMETERSWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QList>
#include <QPointer>
#include "numberedit.h"
#include "libwid_imexport.h"
#include "qfsimplefitparameterinterface.h"
#include <QStringList>

class QFWIDLIB_EXPORT QFSimpleFitParametersWidget : public QWidget {
        Q_OBJECT
    public:
        explicit QFSimpleFitParametersWidget(QFSimpleFitParameterInterface* paraminterface, QWidget *parent = 0);

        void clearWidgets();
        void setParameterInterface(QFSimpleFitParameterInterface* paraminterface);
        void addParameter(const QString& id, const QString& label);
        void setParameters(const QStringList& ids, const QStringList& labels);

    signals:
        
    public slots:
        void updateWidgetValues();

    protected:
        bool updating;

        QFSimpleFitParameterInterface* paraminterface;

        struct subWidget {
            QString id;
            QLabel* label;
            NumberEdit* edit;
        };
        QList<subWidget> widgets;

        QGridLayout* mainLayout;

        QString getIDForWidget(QObject *edit);

    protected slots:
        void valueChanged(double value);
        
};

#endif // QFSIMPLEFITPARAMETERSWIDGET_H
