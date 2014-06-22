/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    last modification: $LastChangedDate$  (revision $Rev$)

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



#ifndef QFRDRTABLEINTERFACE_H
#define QFRDRTABLEINTERFACE_H

#include <QtPlugin>

/*! \brief interface for data tables
    \ingroup qf3rdrdp_fcs

*/
class QFRDRTableInterface {
    public:
        virtual ~QFRDRTableInterface() {};


        /** \brief retuns the data stored in the table at a given position */
        virtual QVariant tableGetData(quint32 row, quint32 column) const=0;
        /** \brief sets the data in the table at a given position, if the cell does not exist, the table is expanded! */
        virtual void tableSetData(quint32 row, quint32 column, const QVariant& data)=0;
        /** \brief sets a whole column in a table  */
        virtual void tableSetColumnData(quint32 column, const QList<QVariant>& data)=0;
        /** \brief returns a whole column in a table  */
        virtual QList<QVariant>  tableGetColumnData(quint32 column)=0;
        /** \brief sets the column title */
        virtual void tableSetColumnTitle( quint32 column, const QString& data)=0;
        /** \brief returns a column's title */
        virtual QString tableGetColumnTitle(quint32 column) const=0;
        /** \brief retuns \c true if the table implementation supports math expressions in the cells */
        virtual bool tableSupportsExpressions() const=0;
        /** \brief sets the math expression in a specific cell */
        virtual void tableSetExpression(quint32 row, quint32 column, const QString& expression=QString(""))=0;
        /** \brief returns the math expression in a specific cell */
        virtual QString tableGetExpression(quint32 row, quint32 column) const=0;
        /** \brief reevaluates all math expressions */
        virtual void tableReevaluateExpressions()=0;
        /** \brief returns the number of columns in the table */
        virtual int tableGetColumnCount() const=0;
        /** \brief returns the number of rows in the table */
        virtual int tableGetRowCount() const=0;
        /** \brief returns \c true if the table is read-only */
        virtual bool tableIsReadonly() const=0;
        /** \brief sets a whole column in a table  */
        virtual void tableSetColumnDataAsDouble(quint32 column, const QVector<double>& data)=0;
        /** \brief returns a whole column in a table converted to  numbers  */
        virtual QVector<double> tableGetColumnDataAsDouble(int column)=0;
        /** \brief set the expression used to calculate the contents of the given column  */
        virtual void tableSetColumnExpression(quint32 column, const QString& expression=QString(""))=0;
        /** \brief set the comment of the given column  */
        virtual void tableSetColumnComment(quint32 column, const QString& comment=QString(""))=0;
        /** \brief set the comment for the given cell (row, column)  */
        virtual void tableSetComment(quint32 row, quint32 column, const QString& comment=QString(""))=0;
        /** \brief returns, whether setting a table propertie currently causes an immediate update of possibly displayed widgets (slow!!!) */
        virtual bool tablesGetDoEmitSignals() const=0;
        /** \brief enable/disable immediate repaints, when chaning graph properties.
         *
         * You can use this function to ensure that a block of tables...() methods are executet, with only an update of the widgets of a possibly open table editor (doEmit=true causes an immediate update!)
         * \begincode
          bool en=tab.tablesGetDoEmitSignals();
          tab.tablesSetDoEmitSignals(false);
           ... // your tables... code
          tab.tablesSetDoEmitSignals(en);
          \endcode
         */
        virtual void tablesSetDoEmitSignals(bool doEmit)=0;

};

Q_DECLARE_INTERFACE( QFRDRTableInterface,
                     "www.dkfz.de.b040.quickfit3.fcsplugin.QFRDRTableInterface/1.0")

#endif // QFRDRTABLEINTERFACE_H
