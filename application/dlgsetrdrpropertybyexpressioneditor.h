#ifndef DLGSETRDRPROPERTYBYEXPRESSIONEDITOR_H
#define DLGSETRDRPROPERTYBYEXPRESSIONEDITOR_H

#include <QWidget>
#include "qconfigcomboboxwidget.h"
#include "qfproject.h"
#include "qfrawdatarecord.h"

namespace Ui {
    class DlgSetRDRPropertyByExpressionEditor;
}

class DlgSetRDRPropertyByExpression; // forward
class DlgSetRDRPropertyByExpressionEditor : public QFrame, public QConfigComboboxWidgetSaveable
{
        Q_OBJECT

    public:
        explicit DlgSetRDRPropertyByExpressionEditor(DlgSetRDRPropertyByExpression* par, QWidget *parent = 0);
        ~DlgSetRDRPropertyByExpressionEditor();

        QString getExpression() const;
        int getType() const;
        QString getPropertyName() const;

        void setExpression(const QString& exp);
        void setType(int t);
        void setPropertyName(const QString& pn);

        void setProject(QFProject *project);

        void setID(const QString &id);

        virtual bool widgetChanged(QSettings* settings, const QString& prefix);
        virtual bool saveWidget(QSettings* settings, const QString& prefix);
        virtual bool loadWidget(QSettings* settings, const QString& prefix);
    public slots:
        void updateTest();
    protected slots:
        void on_btnDelete_clicked();
        void emitWidgetContentsChanged();
    signals:
        void removeMe(DlgSetRDRPropertyByExpressionEditor* editor);
        void widgetContentsChanged();

    private:
        Ui::DlgSetRDRPropertyByExpressionEditor *ui;
        DlgSetRDRPropertyByExpression* par;
        QFProject *project;
};

#endif // DLGSETRDRPROPERTYBYEXPRESSIONEDITOR_H
