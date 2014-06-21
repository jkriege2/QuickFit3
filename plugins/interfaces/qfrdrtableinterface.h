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
