#ifndef QFFITPARAMETERWIDGET_H
#define QFFITPARAMETERWIDGET_H

#include <QWidget>
#include <QCheckBox>
#include <QSpinBox>
#include <QLabel>
#include <QHBoxLayout>
#include "qffitparameterbasicinterface.h"
#include "numberedit.h"

/*! \brief a widget that displays a fit parameter, together with a label, possibly a checkbox to fix the value, possible a label for the error.
    \ingroup qf3lib_widgets

    This widget will display a widget that is used to set the parameter. These widgets are possible:
      - a NumberEdit
      - a QSpinBox
    .
*/
class QFFitParameterWidget : public QWidget {
        Q_OBJECT
    public:
        /** \brief used to specify the widget to display */
        enum WidgetType { FloatEdit=0, IntSpinBox=1, Header=255 };
        /*! \brief constructor

            \param datastore this is used to read/write the values
            \param parameterID id of the parameter to change
            \param widget which widget to display
            \param editable is the value editable? (fix and range is only displayed, if this is \c true)
            \param displayFix display a fix checkbox?
            \param displayError display the error of the fit value (if !=0) ?
            \param editRangeAllowed display widgets to edit the value range allowed?
            \param parent parent widget
        */
        QFFitParameterWidget(QFFitParameterBasicInterface* datastore, QString parameterID, WidgetType widget, bool editable, bool displayFix, bool displayError, bool editRangeAllowed, QWidget* parent=NULL);
        virtual ~QFFitParameterWidget();

        /** \brief return whether the user may edit the values */
        bool editable() { return m_editable; };
        /** \brief return whether to display the value error */
        bool displayError() { return m_displayError; }
        /** \brief return whether to display a fix checkbox */
        bool displayFix() { return m_displayFix; }
        /** \brief return the parameter id */
        QString parameterID() { return m_parameterID; }

        /** \brief set widths of the edit widgets */
        void setWidgetWidth(int width, int fixWidth);
        /** \brief set widths of checkbox widthes */
        void setCheckWidth(int width);
        /** \brief set unit string */
        void setUnit(QString unit);
        /** \brief set widget increment */
        void setIncrement(double increment);

        void disableDatastore() { m_settingData=true; }
        void enableDatastore() { m_settingData=false; }

    public slots:
        /** \brief reload values from datastore */
        void reloadValues();
        /*! \brief set value of the editor
            \param value the value to set the widget to
            \param error the error of the value (if any)
            \param writeback if \c true this function will write back the value (not the error) to m_datastore, using QFFitParameterBasicInterface::setFitValue()

            This function also calls reloadValues() but superseeds it.
         */
        void setValue(double value, double error, bool writeback=false);
        /** \brief switch the min/max widget on/off */
        void setEditRange(bool editRange);
    signals:
        /** \brief emited when a value changed */
        void valueChanged(QString id, double value);
        /** \brief emited when a fix changed */
        void fixChanged(QString id, bool fix);
        /** \brief emited when a range changed */
        void rangeChanged(QString id, double min, double max);
    protected slots:
        void doubleValueChanged(double value);
        void intValueChanged(int value);
        void doubleMinChanged(double value);
        void intMinChanged(int value);
        void doubleMaxChanged(double value);
        void intMaxChanged(int value);
        void sfixChanged(bool fix);
    protected:
        /** \brief this is used to read/write the values */
        QFFitParameterBasicInterface* m_datastore;
        /** \brief id of the parameter to change */
        QString m_parameterID;
        /** \brief  which widget to display */
        WidgetType m_widgetType;
        /** \brief display a fix checkbox? */
        bool m_displayFix;
        /** \brief display the error of the fit value (if !=0) ? */
        bool m_displayError;
        /** \brief display the error of the fit value (if !=0) ? */
        bool m_editable;
        /** \brief unit of the values */
        QString m_unit;
        /** \brief currently setting data */
        bool m_settingData;
        /** \brief increment for the widget */
        double m_increment;
        /** \brief indicates whether to display the range of the value */
        bool m_editRange;
        /** \brief indicates whether it is allowed to display the range of the value */
        bool m_editRangeAllowed;
        /** \brief width of the input widgets */
        int m_widgetWidth;
        /** \brief with of checkboxes */
        int m_checkWidth;

        NumberEdit* neditValue;
        QSpinBox* spinIntValue;
        NumberEdit* neditMin;
        QSpinBox* spinIntMin;
        NumberEdit* neditMax;
        QSpinBox* spinIntMax;
        QCheckBox* chkFix;
        QLabel* labError;
        QHBoxLayout* layMain;
        QLabel* hlabValue;
        QLabel* hlabMin;
        QLabel* hlabMax;
        QLabel* hlabFix;
        QWidget* spCheck;

    private:
};

#endif // QFFITPARAMETERWIDGET_H
