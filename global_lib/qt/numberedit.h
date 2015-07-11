#ifndef NUMBEREDIT_H
#define NUMBEREDIT_H

#include <QLineEdit>
#include <QValidator>
#include <QDoubleValidator>
#include <QPushButton>
#include <QToolButton>
#include "tools.h"
#include "../lib_imexport.h"



/** \brief this QLineEdit descendent implements a validating edit field that allows to enter doubles
 *         with a possibly defined range
 *  \ingroup tools_qt
 *
 */
class LIB_EXPORT NumberEdit : public QLineEdit {
        Q_OBJECT
    public:
        /** Default constructor */
        NumberEdit(QWidget* parent=NULL);
        /** Default destructor */
        virtual ~NumberEdit();

        /** \brief defines a range */
        inline void setRange(double min, double max) {
            this->min=min;
            this->max=max;
            this->checkMin=true;
            this->checkMax=true;
            //validator->setRange(min, max);
        };

        /** \brief define only the lower bound */
        inline void setMin(double min) {
            this->min=min;
            this->checkMin=true;
        };

        /** \brief define only the upper bound */
        inline void setMax(double max) {
            this->max=max;
            this->checkMax=true;
        };

        /** \brief define which bounds to check */
        inline void setCheckBounds(bool min, bool max) {
            this->checkMin=min;
            this->checkMax=max;
        };

        /** \brief sets the number of decimals */
        inline void setDecimals(int d) {
            decimals=d;
            //validator->setDecimals(d);
        };
        /** \brief sets the number of decimals */
        void setSuffix(QString s=QString(""));

        /** \brief sets the single step size */
        inline void setSingleStep(double val) {
            singleStep=val;
        };
    signals:
        /** \brief emited whenever a entered value changed */
        void valueChanged(double value);
    public slots:
        /** \brief sets the value of the component */
        void setValue(double value);

        /** \brief returns the currently set value */
        double value();

        /** \brief increases the value by singleStep */
        void stepUp();
        /** \brief dereases the value by singleStep */
        void stepDown();
    private slots:
        /** \brief connected to the line edit and called whenever the text changed */
        void texttChanged ( const QString & text );

        /** \brief called when the cursor position changed */
        void cursorChanged ( int old, int newp );

        /** \brief change the appearence of the widget to the error state */
        void setViewError();

        /** \brief change the appearence of the widget to the normal state */
        void setViewOK();
    protected:
        /** \brief min value of the range */
        double min;
        /** \brief min value of the range */
        double max;
        /** \brief if \c true the upper bound (max) is checked */
        bool checkMax;
        /** \brief if \c true the lower bound (min) is checked */
        bool checkMin;
        /** \brief single step when using the buttons */
        double singleStep;
        /** \brief button "+" */
        QToolButton* buttonUp;
        /** \brief button "-" */
        QToolButton* buttonDown;
        /** \brief number of decimals */
        int decimals;
        /** \brief a suffix to display in the widget */
        QString suffix;

        /** \brief validator for the line edit */
        //QMyDoubleValidator* validator;

        /** \brief reads the longest possible double number from the beginning of \a input */
        double extractVal(QString input);

        /** \brief moves and resizes the +/- buttons */
        void resizeEvent(QResizeEvent *event);

        /** \brief react on key presses */
        void keyPressEvent ( QKeyEvent * event );

        /** \brief palette backup */
        QPalette pal;
        /** \brief palette error */
        QPalette palError;
    private:
};



#endif // NUMBEREDIT_H
