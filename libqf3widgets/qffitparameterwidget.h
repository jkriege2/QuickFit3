#ifndef QFFitParameterWidgetWrapper_H
#define QFFitParameterWidgetWrapper_H

#include "libwid_imexport.h"
#include <QWidget>
#include <QCheckBox>
#include <QSpinBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QComboBox>
#include <QGridLayout>
#include <QPointer>
#include <QShortcut>
#include <cfloat>
#include <limits.h>
#include <QMessageBox>
#include "qffitparameterbasicinterface.h"
#include "qfdoubleedit.h"
#include "qffitfunction.h"
#include "tools.h"

/*! \brief a class that displays a fit parameter, together with a label, possibly a checkbox to fix the value, possibly a label for the error and the fit range widgets.
    \ingroup qf3lib_widgets

    This class is NOT a widget. It is "only" a wrapper that displays widgets for a given parameter (description given to the constructor).
    The widgets created by this class are added to a given grid layout in the columns starting from 0 with the label.

    This class keeps track of the associated widgets. \c delete ing it also deletes the widgets. Before that they are removed
    from the layout.

    This widget will display a widget that is used to set the parameter. These widgets are possible:
      - a QFDoubleEdit
      - a QSpinBox
      - a QComboBox
      - a header for the parameters
    .

*/
class QFWIDLIB_EXPORT QFFitParameterWidgetWrapper : public QObject {
        Q_OBJECT
    public:
        /** \brief used to specify the widget to display */
        enum WidgetType { FloatEdit=0, IntSpinBox=1, IntDropDown=2, Header=255 };
        /*! \brief constructor

            \param datastore this is used to read/write the values
            \param layout the layout to which the widgets are added.
            \param row the row o add the widgets to in \ layout
            \param parameterID id of the parameter to change
            \param widget which widget to display
            \param editable is the value editable? (fix and range is only displayed, if this is \c true)
            \param displayFix display a fix checkbox?
            \param displayError display the error of the fit value (if !=0) ?
            \param editRangeAllowed display widgets to edit the value range allowed?
            \param parent parent widget
            \param label label for the parameter
        */
        QFFitParameterWidgetWrapper(QFFitParameterBasicInterface* datastore,  QGridLayout* layout, int row, QString parameterID, WidgetType widget, bool editable, bool displayFix, QFFitFunction::ErrorDisplayMode displayError, bool editRangeAllowed, QWidget* parent=NULL, QString label=QString(""), QStringList comboValues=QStringList());
        virtual ~QFFitParameterWidgetWrapper();

        /** \brief return whether the user may edit the values */
        bool editable() { return m_editable; };
        /** \brief return whether to display the value error */
        bool displayError() { return m_displayError; }
        /** \brief return whether to display a fix checkbox */
        bool displayFix() { return m_displayFix; }
        /** \brief return the parameter id */
        QString parameterID() { return m_parameterID; }

        /** \brief set minimum widths of the edit widgets */
        void setWidgetWidth(int width);
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
        /** \brief en/disable range editing widgets */
        void setRangeEnabled(bool enabled);
        /** \brief show/hide the value/error/fix widgets */
        void setEditValues(bool editValues);
        /** \brief show/hide the value/error/fix widgets, inverted version of setEditValues() */
        void unsetEditValues(bool editValues) { setEditValues(!editValues); };
        /** \brief set the absolute min/max for a given value */
        void setValueAbsoluteRange(double min, double max);
        /** \brief set tooltip for widgets (actually this should contain a description of the parameter, as things like "error of " or "value of " will be added autmatically) */
        void setToolTip(QString paramDescription);
        /** \brief show/hide the complete widget */
        void setVisible(bool visible);

        /** \brief switch between editing parameters (!=0) and parameter ranges (==0) (allows to connect to widgets with a currentIndexChanged(int) signal) */
        void setEditRange(int range);

        /** \brief switch between editing parameters (!=0) and parameter ranges (==0) (allows to connect to widgets with a currentIndexChanged(int) signal) */
        void unsetEditValues(int range);
    signals:
        /** \brief emited when a value changed */
        void valueChanged(QString id, double value);
        /** \brief emited when an error changed */
        void errorChanged(QString id, double error);
        /** \brief emited when a fix changed */
        void fixChanged(QString id, bool fix);
        /** \brief emited when a range changed */
        void rangeChanged(QString id, double min, double max);
        /** \brief emited when the suer hits the ENTER key in a value edit widget */
        void enterPressed(QString id);
    protected slots:
        void doubleValueChanged(double value);
        void doubleErrorChanged(double error);
        void intValueChanged(int value);
        void intValueChangedFromCombo(int value);
        void doubleMinChanged(double value);
        void intMinChanged(int value);
        void doubleMaxChanged(double value);
        void intMaxChanged(int value);
        void sfixChanged(bool fix);
        void pEnterPressed();
        void s_actCopyValueRuns();
        void s_actCopyFixRuns();
        void s_actCopyValueFixRuns();
        void s_actCopyValue();
        void s_actCopyFix();
        void s_actCopyValueFix();
        void s_actCopyValueInit();
        void s_actCopyFixInit();
        void s_actCopyValueFixInit();
        void s_actResetValue();
        void s_actResetFix();
        void s_actResetValueFix();
        void keyEventMatches(int key, Qt::KeyboardModifiers modifiers);
    protected:
        /** \brief parameter label */
        QString m_label;
        /** \brief parameter description */
        QString m_paramDescription;
        /** \brief this is used to read/write the values */
        QFFitParameterBasicInterface* m_datastore;
        /** \brief id of the parameter to change */
        QString m_parameterID;
        /** \brief  which widget to display */
        WidgetType m_widgetType;
        /** \brief display a fix checkbox? */
        bool m_displayFix;
        /** \brief display the error of the fit value (if !=0) ? */
        QFFitFunction::ErrorDisplayMode m_displayError;
        /** \brief display the error of the fit value (if !=0) ? */
        bool m_editable;
        /** \brief unit of the values */
        QString m_unit;
        /** \brief currently setting data */
        bool m_settingData;
        /** \brief increment for the widget */
        double m_increment;
        /** \brief indicates whether to display the range widgets  */
        bool m_editRange;
        /** \brief indicates whether to display the value/error/fix widgets */
        bool m_editValues;
        /** \brief indicates whether it is allowed to display the range of the value */
        bool m_editRangeAllowed;
        /** \brief width of the input widgets */
        int m_widgetWidth;
        bool m_visible;
        /** \brief parent widget */
        QPointer<QWidget> m_parent;

        QStringList m_comboValues;

        QPointer<QFDoubleEdit> neditValue;
        QPointer<QFDoubleEdit> neditError;
        QPointer<QSpinBox> spinIntValue;
        QPointer<QComboBox> cmbIntValue;
        QPointer<QFDoubleEdit> neditMin;
        QPointer<QSpinBox> spinIntMin;
        QPointer<QFDoubleEdit> neditMax;
        QPointer<QSpinBox> spinIntMax;
        QPointer<QCheckBox> chkFix;
        QPointer<QLabel> labError;
        QPointer<QLabel> hlabValue;
        QPointer<QLabel> hlabMin;
        QPointer<QLabel> hlabMax;
        QPointer<QLabel> hlabFix;
        QPointer<QLabel> labLabel;

        QShortcut* scSpace;
        QShortcut* scReturn;
        QShortcut* scEnter;
        QAction* actCopyValue;
        QAction* actCopyFix;
        QAction* actCopyValueFix;
        QAction* actCopyValueRuns;
        QAction* actCopyFixRuns;
        QAction* actCopyValueFixRuns;
        QAction* actCopyValueInit;
        QAction* actCopyFixInit;
        QAction* actCopyValueFixInit;
        QAction* actResetValue;
        QAction* actResetFix;
        QAction* actResetValueFix;
        QAction* sep1;
        QAction* sep2;
        QAction* sep3;
        QAction* sep4;

        QPointer<QGridLayout> m_layout;

        void fillCombo(QComboBox* cmb, int min, int max);

    private:
};

#endif // QFFitParameterWidgetWrapper_H
