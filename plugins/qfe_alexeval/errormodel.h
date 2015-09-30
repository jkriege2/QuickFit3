/*
  Name: errormodel.h
  Copyright: (c) 2008
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/

*/


/** \file errormodel.h
  * \ingroup sdff_editor
  *
  * This file implements a Qt model that can store errormessages that are produced by
  * the various parser classes in the sequencer library. It is based on a simple table
  * model where each column represents one property of an error message (errorcode, line,
  * errormessage ...).
  */

#ifndef ERRORMODEL_H
#define ERRORMODEL_H

#include <QString>
#include <QAbstractTableModel>
#include <vector>

#include "sdfflib/sequencererror.h"
#include "sdfflib/baseimporter.h"

/** \brief This is a Qt model class (descendent of QAbstractTableModel) which
 *         implements a model that may hold error messages produced  by the
 *         sequencer library.
 *
 *  \ingroup sdff_editor
 *
 * This is based on a simple table model where each column of the table represents one
 * error information and each row represent one error. By using this model we can use the
 * powerful Qt model/view framework to display the errormessages in a table.
 *
 * For each errormessage the model stores these parameters:
 *  - errorType (Error, Warning, Information)
 *  - errorcode
 *  - filename (the file where the error occured)
 *  - line (where the error occured)
 *  - time (when did the error occur, for runtime errors only!)
 *  - errormessage
 * .
 *
 * The sequencer::SDFFParser class produces (like all sequencer::baseImporter descendents) a list if sequencer::bi_event
 * objects that represent one error/message/warning event each. This class stores a list
 * of such sequencer::bi_event events in the private property list and allows for access to them via
 * the standarddized QAbstractTableModel interface. This model is read-only as we do not
 * need an editing of the error messages by the user, we just want to display them.
 *
 */
class ErrorModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    /** \brief class constructor that may load a list of events from the supplied \a importer object */
    ErrorModel(sequencer::baseImporter* importer = NULL, QObject *parent = 0);

    /** \brief load errors from sequencer::baseImporter class into this Qt model class */
    void loadErrors(sequencer::baseImporter* importer);

    /** \brief return the n-th event */
    sequencer::bi_event getEvent(unsigned int n);

    /** \brief add an addition event */
    void addEvent(sequencer::bi_event event);

    /** \brief Returns the number of rows for the children of the given parent. */
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    /** \brief Returns the number of columns for the children of the given parent. */
    int columnCount(const QModelIndex & parent = QModelIndex() ) const;
    /** \brief Returns the data stored under the given role for the item referred to by the index. */
    QVariant data(const QModelIndex &index, int role) const;
    /** \brief Returns the data for the given role and section in the header with the specified orientation. */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

private:
    /** \brief a vector containing all events accessible by the model interface */
    std::vector<sequencer::bi_event> events;
};

#endif // ERRORMODEL_H


