#ifndef DLGNEWPROPERTY_H
#define DLGNEWPROPERTY_H

#include <QDialog>
#include "ui_dlg_newproperty.h"

class dlgNewProperty : public QDialog, public Ui::dlgNewProperty
{
    public:
        /** Default constructor */
        dlgNewProperty(QWidget* parent);
        /** Default destructor */
        virtual ~dlgNewProperty();
    protected:
    private:
};

#endif // DLGNEWPROPERTY_H
