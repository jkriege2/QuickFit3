#ifndef DOUBLEDATACUTSLIDERS_H
#define DOUBLEDATACUTSLIDERS_H

#include "qfdoubleedit.h"
#include <QDoubleSpinBox>
#include <QWidget>
#include <QLabel>
#include "tools.h"
#include <iostream>
#include "libwid_imexport.h"

/** \brief two sliders in a box, aligned left and right, like DataCutSliders, but allows to select a range of double-numbers and supports logarithmic scales
 *  \ingroup qf3lib_widgets
 *
 * The widget looks like this:
 *  \image html datacutsliders.png
 * You can supply a lower and an upper bound which is valid for both integer edit fields (QSpinBoxes).
 * The left edit allows to set the lower bound and the right one the upper bound. So the left box starts at
 * value=min and the right one at value=max. It is always asserted that leftValue<rightValue.
 *
 */
class QFWIDLIB_EXPORT DoubleDataCutSliders : public QWidget
{
        Q_OBJECT
    public:
        explicit DoubleDataCutSliders(QWidget *parent = 0);

        GetSetMacroI(double, min, update())
        GetSetMacroI(double, max, update())
        GetSetMacroI(double, userMin, update())
        GetSetMacroI(double, userMax, update())
        GetSetMacro(bool, allowCopyToAll)
        GetSetMacro(QString, runsName)
        GetSetMacro(QString, filesName)
        GetSetMacro(bool, copyToRunsEnabled)
        GetSetMacro(bool, copyToFilesEnabled)
        GetMacro(bool, sliderSignals)

        void setLogScale(bool enabled, double increments=20);

        /** \brief switches off the slidersChanged() signal. */
        void disableSliderSignals() {
            sliderSignals=false;
        }
        /** \brief switches on the slidersChanged() signal. */
        void enableSliderSignals() {
            sliderSignals=true;
        }
    protected:

        /** \brief displays a context menu, if contextMenu is not \c NULL */
        void contextMenuEvent(QContextMenuEvent *event);

    private slots:
        /** \brief this slot is bound to the sliderLow and is called whenever its value changes. It updates the internal variables and label */
        void sliderLowValueChanged(double value);
        /** \brief this slot is bound to the sliderHigh and is called whenever its value changes. It updates the internal variables and label */
        void sliderHighValueChanged(double value);

        /** \brief this slot is bound to the sliderLow and is called whenever it finished editing. */
        void sliderLowEditingFinished();
        /** \brief this slot is bound to the sliderHigh and is called whenever it finishes editing */
        void sliderHighEditingFinished();

        /** \brief this emity copyUserMinToAll() */
        void copyUserMinClicked() { emit copyUserMinToAll(userMin); }

        /** \brief this emity copyUserMaxToAll() */
        void copyUserMaxClicked() { emit copyUserMaxToAll(userMax); }

        /** \brief this emity copyUserMaxToAll() */
        void copyUserMinMaxClicked() { emit copyUserMinMaxToAll(userMin, userMax); }

        /** \brief this emity copyUserMinToAll() */
        void copyUserMinRunsClicked() { emit copyUserMinToAllRuns(userMin); }

        /** \brief this emity copyUserMaxToAll() */
        void copyUserMaxRunsClicked() { emit copyUserMaxToAllRuns(userMax); }

        /** \brief this emity copyUserMaxToAll() */
        void copyUserMinMaxRunsClicked() { emit copyUserMinMaxToAllRuns(userMin, userMax); }

    signals:
        /** \brief this signal is emitted by the DataCutSlider widget, whenever the user moved the sliders or set new values by
        *          the set_userMin(), set_userMax() and get_min() as well as get_max() methods. You can switch off this signal by
        *          using disableSliderSignals() (and enableSliderSignals() to reacivate it).
        */
        void slidersChanged(double userMin, double userMax, double min, double max);

        /** \brief this signal is emited when the user clicks the "copy to all" context menu of the min widget item. */
        void copyUserMinToAll(double userMin);

        /** \brief this signal is emited when the user clicks the "copy to all" context menu of the max widget item. */
        void copyUserMaxToAll(double userMax);

        /** \brief this signal is emited when the user clicks the "copy to all runs" context menu of the min widget item. */
        void copyUserMinToAllRuns(double userMin);

        /** \brief this signal is emited when the user clicks the "copy to all runs" context menu of the max widget item. */
        void copyUserMaxToAllRuns(double userMax);

        /** \brief this signal is emited when the user clicks the "copy to all" context menu of the min/max widget item. */
        void copyUserMinMaxToAllRuns(double userMin, double userMax);

        /** \brief this signal is emited when the user clicks the "copy to all runs" context menu of the max widget item. */
        void copyUserMinMaxToAll(double userMin, double userMax);

    public slots:
        /** \brief sets all the data and constraints of the widget at once */
        void setValues(double userMin, double userMax, double min, double max);

        void setCentralText(const QString& label);
    private:
        /** \brief update the widgets with the values in the private data members */
        void update();

        /** \brief indicates whether to emit the signalChanged() signal, wen the values change
         *
         * This property is set with enableSliderSignals() and disableSliderSignals() and accessed by
         * get_sliderSignals()
         */
        bool sliderSignals;

        /** \brief QLineEdit which displays and edits the current lower cut-off */
        QFDoubleEdit* editLow;
        /** \brief QLineEdit which displays and edits the current upper cut-off */
        QFDoubleEdit* editHigh;


        /** \brief if set \c true this widget may emit a copy...() signals */
        bool allowCopyToAll;

        /** \brief context menu for the widget */
        QMenu* contextMenu;

        /** \brief overall minimum value */
        double min;
        /** \brief overall maximum value */
        double max;
        /** \brief user set minimum value */
        double userMin;
        /** \brief user set maximum value */
        double userMax;

        /** \brief name used for "runs" */
        QString runsName;
        /** \brief name used for "files" */
        QString filesName;
        /** \brief enable copy to runs */
        bool copyToRunsEnabled;
        /** \brief enable copy to all files */
        bool copyToFilesEnabled;

        QLabel* labCentral;
};

#endif // DOUBLEDATACUTSLIDERS_H
