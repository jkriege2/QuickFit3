#ifndef NUMBEREDIT_H
#define NUMBEREDIT_H

#include <QLineEdit>
#include <QValidator>
#include <QDoubleValidator>
#include <QPushButton>
#include "tools.h"


///** \brief this QDoubleValidator descendent may exclude a given suffix from the string before validation
// *  \ingroup quickfit2_widgets
// */
//class QMyDoubleValidator: public QDoubleValidator {
//        Q_OBJECT
//    public:
//        /** \brief class constructor */
//        QMyDoubleValidator(QObject* parent=NULL): QDoubleValidator(parent) {
//            suffix="";
//        };
//
//        /** \brief sets the number of decimals */
//        inline void setSuffix(QString& s=QString("")) {
//            suffix=s;
//        };
//
//        /** \brief first remove the suffix, then validate with the parent class */
//        virtual QValidator::State validate ( QString & input, int & pos ) const {
//            QString in=input;
//            if (input.endsWith(suffix)) {
//                in=in.remove(input.size()-suffix.size(), suffix.size());
//                if (pos>=in.size()) pos=in.size();
//            }
//            //std::cout<<"validate '"<<in.toStdString()<<"'\n";
//            return QDoubleValidator::validate(in, pos);
//        }
//    protected:
//        /** \brief a suffix to display in the widget */
//        QString suffix;
//
//};


/** \brief this QLineEdit descendent implements a validating edit field that allows to enter doubles
 *         with a possibly defined range
 *  \ingroup quickfit2_widgets
 *
 */
class DoubleEdit : public QLineEdit {
        Q_OBJECT
    public:
        /** Default constructor */
        DoubleEdit(QWidget* parent=NULL);
        /** Default destructor */
        virtual ~DoubleEdit();

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
        QPushButton* buttonUp;
        /** \brief button "-" */
        QPushButton* buttonDown;
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
