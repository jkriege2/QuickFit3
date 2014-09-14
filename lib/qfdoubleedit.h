/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QFDOUBLEEDIT_H
#define QFDOUBLEEDIT_H

#include "lib_imexport.h"
#include <QLineEdit>
#include <QRegExpValidator>
#include <QRegExp>
#include <QToolButton>
#include <cmath>
#include <QList>
#include <QAction>
#include <QPair>
#include "programoptions.h"
/*! \brief this QLineEdit descendent implements a validating edit field that allows to enter floating point numbers
           with a possibly defined range
    \ingroup qf3lib_widgets

    In addition this widget colors the background of the widget if the number you entered exceeds the range. In that case
    the edit corrects the value when it looses its focus. The error background color may be set by setErrorColor().

    The maximal number of decimals may be set using setDecimals(). Note that trailing 0s are deleted from the string.

    This widget accepts comma and point as decimal separator and transforms them to the systems decimal separator,
    set in the current locale.

    It is also possible to extend the default context menu with new QAction's by calling addContextmenuAction(),
    deleteContextmenuAction() and clearContextmenuActions().

    This widget may also be used to catch key events. You will have to register each event using addKeyEvent().
    If a key event matches, the keyEvent() signal will be emitted.

    Using setLogScale() the sidget can be switched to a logarithmic step scheme. In that case, the increments are the
    number of increments per decade.

 */
class QFLIB_EXPORT QFDoubleEdit : public QLineEdit {
        Q_OBJECT
    public:
        /** \brief class constructor */
        QFDoubleEdit(QWidget* parent);
        /** \brief class destructor */
        virtual ~QFDoubleEdit();
        /** \brief return the value entered in this widget */
        double value() const;
        /** \brief add a new QAction to the default context menu (append after a seprator) */
        void addContextmenuAction(QAction* menuAction);
        /** \brief remove a given QAction from the default context menu (added with addContextmenuAction() ) */
        void deleteContextmenuAction(QAction* menuAction);
        /** \brief remove all additional context menu QAction s */
        void clearContextmenuActions();
        /** \brief add a new Key Event */
        void addKeyEvent(int key, Qt::KeyboardModifiers modifiers);

        void setErrorColor(const QColor& color) { m_errorColor=color; updateWidget(text()); }
        void setBackgroundColor(const QColor& color) { m_background=color; updateWidget(text()); }
        double maximum() const { return m_maximum; };
        double minimum() const { return m_minimum; };
        double checkMaximum() const { return m_checkMaximum; };
        double checkMinimum() const { return m_checkMinimum; };
        double decimals() const { return m_decimals; };
        QColor errorColor() const { return m_errorColor; }
        QColor backgroundColor() const { return m_background; }
        double increment() const { return m_increment; }
        bool showUpDown() const { return m_showUpDown; }
        bool isIntegerWidget() const { return m_Integer; }
        bool logScale() const;
    public slots:
        void setCheckMaximum(bool check) { m_checkMaximum=check; }
        void setCheckMinimum(bool check) { m_checkMinimum=check; }
        void setCheckBounds(bool min, bool max) { setCheckMinimum(min); setCheckMaximum(max); }
        void setIntegerWidget(bool intWidget) { m_Integer=intWidget; setValue(this->value()); }
        /** \brief set the value entered in this widget */
        void setValue(double value);
        void setMaximum(double value) { setCheckMaximum(true); m_maximum=value; setValue(this->value()); }
        void setMinimum(double value) { setCheckMinimum(true); m_minimum=value; setValue(this->value()); }
        void setRange(double min, double max);
        void setDecimals(int decimals) { m_decimals=decimals; setValue(value()); }
        void stepUp();
        void stepDown();
        void stepUp(int steps);
        void stepDown(int steps);
        void setIncrement(double inc) { m_increment=inc; }
        void setSingleStep(double inc) { setIncrement(inc); }
        void setShowUpDown(bool showUpDown);
        void setUpDownKeyEnabled(bool enabled);
        void setLogScale(bool enabled, double increments=10);
    signals:
        /** \brief emited whenever a entered value changed */
        void valueChanged(double value);
        /** \brief emitted when the widget looses focus */
        void focusOut(double value);
        /** \brief emitted when a key event matches */
        void keyEventMatches(int key, Qt::KeyboardModifiers modifiers);
        void enterPressed();
    protected:
        /** \brief used to store the key events */
        QList<QPair<int, Qt::KeyboardModifiers> > m_keyEvents;
        /** \brief list of additional actions for context menu */
        QList<QAction*> m_contextmenuActions;
        /** \brief regular expression used to match floating point numbers */
        QRegExp m_regexp;
        /** \brief RegExp validator for this widget */
        QRegExpValidator* m_validator;
        /** \brief hidden setValidator() method */
        void setValidator(const QValidator* validator) {
            QLineEdit::setValidator(validator);
        }
        /** \brief error color */
        QColor m_errorColor;
        QColor m_background;
        QColor m_noInt;
        double m_maximum;
        double m_minimum;
        double m_increment;
        bool m_checkMaximum;
        bool m_checkMinimum;
        int m_decimals;
        bool m_logscale;
        double m_logbase;
        QString QFDoubleEdit_BASIC_REGEXP;
        bool m_Integer;
        bool m_showUpDown;
        QToolButton* m_btnUp;
        QToolButton* m_btnDown;
        bool m_updownKeyEnabled;
        double m_current;
        virtual void contextMenuEvent(QContextMenuEvent* event);
    protected slots:
        void updateWidget(const QString & text);
        virtual void focusOutEvent ( QFocusEvent * event ) ;
        virtual void keyPressEvent ( QKeyEvent * event ) ;
        virtual void resizeEvent ( QResizeEvent * event ) ;
        virtual void wheelEvent ( QWheelEvent * event );
};


CREATE_WIDGET_PROGRAMOPTIONS_QFLOADSTORE(QFDoubleEdit,setValue,value,double)

#endif // QFDOUBLEEDIT_H
