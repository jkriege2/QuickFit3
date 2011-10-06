#include <QtGui>
#include "tools.h"
#include <iostream>
#include "libwid_imexport.h"

#ifndef DATACUTSLIDER_H
#define DATACUTSLIDER_H

/** \brief two sliders in a box, aligned left and right
 *  \ingroup qf3lib_widgets
 *
 * The widget looks like this:
 *  \image html datacutsliders.png
 * You can supply a lower and an upper bound which is valid for both integer edit fields (QSpinBoxes).
 * The left edit allows to set the lower bound and the right one the upper bound. So the left box starts at
 * value=min and the right one at value=max. It is always asserted that leftValue<rightValue.
 *
 */
class QFWIDLIB_EXPORT DataCutSliders : public QWidget
{
    Q_OBJECT

    public:
        /** \brief class constructor */
        DataCutSliders(QWidget* parent=0);

        GetSetMacroI(int, min, update())
        GetSetMacroI(int, max, update())
        GetSetMacroI(int, userMin, update())
        GetSetMacroI(int, userMax, update())
        GetSetMacro(bool, allowCopyToAll)
        GetSetMacro(QString, runsName)
        GetSetMacro(QString, filesName)
        GetSetMacro(bool, copyToRunsEnabled)
        GetSetMacro(bool, copyToFilesEnabled)
        GetMacro(bool, sliderSignals)

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
        void sliderLowValueChanged(int value);
        /** \brief this slot is bound to the sliderHigh and is called whenever its value changes. It updates the internal variables and label */
        void sliderHighValueChanged(int value);

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
        void slidersChanged(int userMin, int userMax, int min, int max);

        /** \brief this signal is emited when the user clicks the "copy to all" context menu of the min widget item. */
        void copyUserMinToAll(int userMin);

        /** \brief this signal is emited when the user clicks the "copy to all" context menu of the max widget item. */
        void copyUserMaxToAll(int userMax);

        /** \brief this signal is emited when the user clicks the "copy to all runs" context menu of the min widget item. */
        void copyUserMinToAllRuns(int userMin);

        /** \brief this signal is emited when the user clicks the "copy to all runs" context menu of the max widget item. */
        void copyUserMaxToAllRuns(int userMax);

        /** \brief this signal is emited when the user clicks the "copy to all" context menu of the min/max widget item. */
        void copyUserMinMaxToAllRuns(int userMin, int userMax);

        /** \brief this signal is emited when the user clicks the "copy to all runs" context menu of the max widget item. */
        void copyUserMinMaxToAll(int userMin, int userMax);

    public slots:
        /** \brief sets all the data and constraints of the widget at once */
        void setValues(int userMin, int userMax, int min, int max);
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
        QSpinBox* editLow;
        /** \brief QLineEdit which displays and edits the current upper cut-off */
        QSpinBox* editHigh;


        /** \brief if set \c true this widget may emit a copy...() signals */
        bool allowCopyToAll;

        /** \brief context menu for the widget */
        QMenu* contextMenu;

        /** \brief overall minimum value */
        int min;
        /** \brief overall maximum value */
        int max;
        /** \brief user set minimum value */
        int userMin;
        /** \brief user set maximum value */
        int userMax;

        /** \brief name used for "runs" */
        QString runsName;
        /** \brief name used for "files" */
        QString filesName;
        /** \brief enable copy to runs */
        bool copyToRunsEnabled;
        /** \brief enable copy to all files */
        bool copyToFilesEnabled;
};




#endif // DATACUTSLIDER_H

