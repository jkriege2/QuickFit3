/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef QMODERNPROGRESSWIDGET_H
#define QMODERNPROGRESSWIDGET_H

#include <QWidget>
#include <QColor>
#include <QTimer>
#include <QImage>
#include "qfdialog.h"
#include <QLabel>
#include <QPushButton>
#include <QApplication>
#include "libwid_imexport.h"

/*! \brief a progress indicator widget
    \ingroup tools_qt

    like QProgressBar this can be used to display a progress value() between rangeMin() and rangeMax(), but there is also
    a mode where a colored circle/stroke wanders around the widget to indicate that something is going on, but not giving
    a real progress.
*/
class QFWIDLIB_EXPORT QModernProgressWidget : public QWidget {
        Q_OBJECT
    public:
         /** \brief modes of the progress widget */
         enum Mode {
             Strokes, /**< a ring of rounded strokes */
             Circles, /**< a ring of circles */
             GradientRing  /**< a ring filled with a color gradient */
         };

         /** \brief mode of percentage display */
         enum PercentageMode {
             Percent, /**< display the percentage (value() between minimum() and maximum() ) */
             Value  /**< display the value with the given suffix */
         };

         QModernProgressWidget(QWidget* parent=NULL);
         virtual ~QModernProgressWidget();

         /** \brief suffix for progress display */
         inline QString suffix() { return m_suffix; }
         /** \brief precision for progress display */
         inline int precision() { return m_precision; }
         /** \brief range minimum */
         inline double minimum() const { return m_rangeMin; }
         /** \brief range maximum */
         inline double maximum() const { return m_rangeMax; }
         /** \brief progress value */
         inline double value() const { return m_value; }
         /** \brief outer radius (0..1) of the displayed circle/stroke ring */
         inline double outerRadius() { return m_outerRadius; }
         /** \brief inner radius (0..1) of the displayed circle/stroke ring */
         inline double innerRadius() const { return m_innerRadius; }
         /** \brief color of an indicator, when inactive (usually a bit darker than the widget background color) */
         inline QColor backgroundColor() const { return m_backgroundColor; }
         /** \brief color of the indicator the furthest away from the main indicator (usually the same as backgroundColor() ) */
         inline QColor startColor() const { return m_startColor; }
         /** \brief color just before the current indicator (usually darker than startColor() ) */
         inline QColor stopColor() const { return m_stopColor; }
         /** \brief color of the current item */
         inline QColor indicatorColor() const { return  m_indicatorColor; }
         /** \brief how many of the indicators (fraction 0..1 of items() ) should follow current indicator with a different color than backgroundColor() ) */
         inline double nonBackgroundRange() const { return m_nonBackgroundRange; }
         /** \brief is an automatic spin going on? */
         inline bool spin() const { return m_spin; }
         /** \brief interval (milliseconds) of the movement if spin() is \c true */
         inline int spinInterval() const { return m_spinInterval; }
         /** \brief number of indicators making up the circle */
         inline int items() const { return m_items; }
         /** \brief type of widget: display a ring of strokes or circles, ...) */
         inline Mode mode() const { return m_mode; }
         /** \brief display the current percentage of progress as text */
         inline bool displayPercent() const { return m_displayPercent; }
         /** \brief color of the progress text */
         inline QColor textColor() const { return m_textColor; }
         /** \brief type of percentage mode */
         inline PercentageMode percentageMode() const { return m_percentageMode; }

         /** \brief suffix for progress display */
         inline void setSuffix(const QString& s) { m_suffix=s; updateWidget(); }
         /** \brief precision for progress display */
         inline void setPrecision(int p) { m_precision=p; updateWidget(); }
         /** \brief display the current percentage of progress as text */
         inline void setDisplayPercent(bool display) { m_displayPercent=display; updateWidget(); }
         /** \brief color of the progress text */
         inline void setTextColor(QColor col) { m_textColor=col; updateWidget();}
         /** \brief outer radius (0..1) of the displayed circle/stroke ring */
         inline void setOuterRadius(int val) { m_outerRadius =val; updateWidget(); }
         /** \brief inner radius (0..1) of the displayed circle/stroke ring */
         inline void setInnerRadius(int val) { m_innerRadius =val; updateWidget(); }
         /** \brief how many of the indicators (fraction 0..1 of items() ) should follow current indicator with a different color than backgroundColor() ) */
         inline void setNonBackgroundRange(double val) { m_nonBackgroundRange=val; updateWidget();}
         /** \brief color of the indicator the furthest away from the main indicator (usually the same as backgroundColor() ) */
         inline void setStartColor(QColor col) { m_startColor=col; updateWidget();}
         /** \brief color just before the current indicator (usually darker than startColor() ) */
         inline void setStopColor(QColor col) { m_stopColor=col; updateWidget();}
         /** \brief color of the current item */
         inline void setIndicatorColor(QColor col) { m_indicatorColor=col; updateWidget();}
         /** \brief color of an indicator, when inactive (usually a bit darker than the widget background color) */
         inline void setBackgroundColor(QColor col) { m_backgroundColor=col; updateWidget();}
         /** \brief is an automatic spin going on? */
         void setSpin(bool enabled);
         /** \brief interval (milliseconds) of the movement if spin() is \c true */
         inline void setSpinInterval(int msec) { m_spinInterval=msec; timer.setInterval(msec); }
         /** \brief number of indicators making up the circle */
         inline void setItems(int i) { m_items=i; updateWidget();}
         /** \brief type of widget: display a ring of strokes or circles, ...) */
         inline void setMode(Mode m) { m_mode=m; updateWidget(); }
         /** \brief type of percentage mode */
         inline void setPercentageMode(PercentageMode m) { m_percentageMode=m; updateWidget(); }
     public slots:
         /** \brief range minimum */
         inline void setMinimum(double val) { m_rangeMin =val; updateWidget(); }
         /** \brief range maximum */
         inline void setMaximum(double val) { m_rangeMax =val; updateWidget(); }
         /** \brief progress value */
         inline void setValue(double val) { m_value =val; updateWidget(); }
         /** \brief set range of progress */
         inline void setRange(double min, double max) { m_rangeMin=min; m_rangeMax=max; updateWidget(); }
     protected:
         virtual void paintEvent(QPaintEvent *event);
         virtual void resizeEvent(QResizeEvent* event);
     protected slots:
         void doSpin();
         void updateWidget();
     private:
         PercentageMode m_percentageMode;
         QString m_suffix;
         int m_precision;
         double m_rangeMin;
         double m_rangeMax;
         double m_value;
         double m_outerRadius;
         double m_innerRadius;
         int m_items;
         int m_spinItem;
         bool m_displayPercent;

         QColor m_startColor;
         QColor m_stopColor;
         QColor m_backgroundColor;
         QColor m_indicatorColor;
         QColor m_textColor;
         double m_nonBackgroundRange;
         bool m_spin;
         int m_spinInterval;

         bool m_doRepaint;
         bool m_darkCircleBorder;

         Mode m_mode;

         QTimer timer;

};


/*! \brief dialog, using QModernProgressWidget
    \ingroup tools_qt

    use open() and hide() to show/hide the dialog and openDelayed() if you wsnt the dialog to appear a given time after
    you called the function.

    The dialog is displayed at the screen center if setCentered() is set \c true.
*/
class QFWIDLIB_EXPORT QModernProgressDialog: public QFDialog {
        Q_OBJECT
    public:
        QModernProgressDialog(QWidget* parent=NULL, Qt::WindowFlags f = 0);
        QModernProgressDialog(const QString & labelText, const QString & cancelButtonText, QWidget* parent=NULL, Qt::WindowFlags f = Qt::Window|Qt::WindowTitleHint);
        virtual ~QModernProgressDialog();

        /** \brief text in the label of the dialog */
        inline QString labelText() { return label->text(); }
        /** \brief text on the "Cancel" button of the dialog */
        inline QString cancelButtonText() { return cancel->text(); }

        /** \brief text in the label of the dialog */
        inline void setLabelText(const QString& t) { label->setText(t); QApplication::processEvents(); }
        /** \brief text on the "Cancel" button of the dialog */
        inline void setCancelButtonText(const QString& t) { cancel->setText(t); QApplication::processEvents(); }
        /** \brief indicate whetehr the dialog has a Cancel button (default: \c true )*/
        inline void setHasCancel(bool has=true) { cancel->setVisible(has); QApplication::processEvents(); }
        /*! \brief display the progress dialog after a delay of \a minimumDuration milliseconds

            If the dialog has beend closed before the delay runs out, it is never displayed.
         */
        void openDelayed(int minimumDuration=2500);
        /** \brief returns \c true if the "Cancel" button has been clicked */
        inline bool wasCanceled() { return m_wasCancel; }
        inline double value() const { return progress->value(); }
        inline double minimum() const { return progress->minimum(); }
        inline double maximum() const { return progress->maximum(); }
        inline bool spin() const { return progress->spin(); }
        inline QModernProgressWidget* progressWidget() const { return progress; }
        inline QLabel* textLabel() const { return label; }
        inline QPushButton* cancelButton() const { return cancel; }
        /** \brief en-/disable spin and progress mode */
        void setMode(bool enabledSpin=true, bool enabledProgress=true);
    public slots:
        /** \brief set the value in progress mode (activate calling setRange() or setProgress() ) */
        void setValue(double value);
        /** \brief set the progress range */
        void setRange(double min, double max);
        /** \brief set the progress range */
        void setMinimum(double min);
        /** \brief set the progress range */
        void setMaximum(double max);
    signals:
         /** \brief emitted when the "Cancel" button is clicked */
         void canceled();
    private:
        QLabel* label;
        QModernProgressWidget* progress;
        QPushButton* cancel;
        int m_minimumDuration;
        bool m_wasCancel;
        bool m_closedBeforeDelay;
        QTimer timerDelay;
    protected:
        void createWidgets();
        virtual void showEvent(QShowEvent* event);
        virtual void closeEvent(QCloseEvent* e);
    protected slots:
        void cancelClicked();
};

#endif // QMODERNPROGRESSWIDGET_H
