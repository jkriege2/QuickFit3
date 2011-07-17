#ifndef DLGNEWPROPERTY_H
#define DLGNEWPROPERTY_H

#include <QDialog>


namespace Ui {
    class dlgNewProperty; // forward
}

class dlgNewProperty : public QDialog
{
        Q_OBJECT
    public:
        /** Default constructor */
        dlgNewProperty(QWidget* parent);
        /** Default destructor */
        virtual ~dlgNewProperty();

        QString propertyName();
        QString propertyType();
        int propertyTypeInt();
        QString propertyValue();

        void setPropertyName(QString name);
        void setPropertyValue(QString value);
        void setPropertyType(int i);
        void addPropertyNames(const QStringList& names);
    protected:
    private:
        Ui::dlgNewProperty* ui;
};

#endif // DLGNEWPROPERTY_H
