#include "qfrdrtableparserfunctions.h"
#include "qfrdrtable.h"



void addQFRDRTableFunctions(QFMathParser* parser, QStringList* names, bool columnMode) {
    parser->addFunction("data", fQFRDRTableEditor_data);
    parser->addFunction("dataleft", fQFRDRTableEditor_dataleft);
    parser->addFunction("colavg", fQFRDRTableEditor_colavg);
    parser->addFunction("colsum", fQFRDRTableEditor_colsum);
    parser->addFunction("colsum2", fQFRDRTableEditor_colsum2);
    parser->addFunction("colvar", fQFRDRTableEditor_colvar);
    parser->addFunction("colstd", fQFRDRTableEditor_colstd);
    parser->addFunction("colmin", fQFRDRTableEditor_colmin);
    parser->addFunction("colmax", fQFRDRTableEditor_colmax);
    parser->addFunction("colmedian", fQFRDRTableEditor_colmedian);
    parser->addFunction("colquantile", fQFRDRTableEditor_colquantile);
    parser->addFunction("colimagewidth", fQFRDRTableEditor_colimagewidth);
    parser->addFunction("colimageheight", fQFRDRTableEditor_colimageheight);
    parser->addFunction("collength", fQFRDRTableEditor_collength);
    parser->addFunction("columntitles", fQFRDRTableEditor_coltitles);
    parser->addFunction("columntitle", fQFRDRTableEditor_coltitle);
    parser->addFunction("columnindexbytitle", fQFRDRTableEditor_colindexbytitle);
    //if (columnMode) {
        parser->addFunction("column", fQFRDRTableEditor_column);
        parser->addFunction("uniquecolumn", fQFRDRTableEditor_columnUnique);
        parser->addFunction("indexedcolavg", fQFRDRTableEditor_indexedColAvg);
        parser->addFunction("indexedcolsum", fQFRDRTableEditor_indexedColSum);
        parser->addFunction("indexedcolsum2", fQFRDRTableEditor_indexedColSum2);
        parser->addFunction("indexedcolvar", fQFRDRTableEditor_indexedColVar);
        parser->addFunction("indexedcolstd", fQFRDRTableEditor_indexedColStd);
        parser->addFunction("indexedcolmin", fQFRDRTableEditor_indexedColMin);
        parser->addFunction("indexedcolmax", fQFRDRTableEditor_indexedColMax);
        parser->addFunction("indexedcolmedian", fQFRDRTableEditor_indexedColMedian);
        parser->addFunction("indexedcolquantile", fQFRDRTableEditor_indexedColQuantile);
        parser->addFunction("columndata", fQFRDRTableEditor_column);
        parser->addFunction("columnstr", fQFRDRTableEditor_columnStr);
        parser->addFunction("columnbool", fQFRDRTableEditor_columnBool);
        parser->addFunction("rowdata", fQFRDRTableEditor_row);
        parser->addFunction("rowstr", fQFRDRTableEditor_rowStr);
        parser->addFunction("rowbool", fQFRDRTableEditor_rowBool);

        parser->addFunction("colsavg", fQFRDRTableEditor_colsavg);
        parser->addFunction("colssum", fQFRDRTableEditor_colssum);
        parser->addFunction("colssum2", fQFRDRTableEditor_colssum2);
        parser->addFunction("colsvar", fQFRDRTableEditor_colsvar);
        parser->addFunction("colsstd", fQFRDRTableEditor_colsstd);
        parser->addFunction("colsmin", fQFRDRTableEditor_colsmin);
        parser->addFunction("colsmax", fQFRDRTableEditor_colsmax);
        parser->addFunction("colsmedian", fQFRDRTableEditor_colsmedian);
        parser->addFunction("colsquantile", fQFRDRTableEditor_colsquantile);

    //}

    if (names) {
        *names  <<"data"
                <<"dataleft"
                <<"colavg"
                <<"colsum"
                <<"colsum2"
                <<"colvar"
                <<"colstd"
                <<"colmin"
                <<"colmax"
                <<"colmedian"
                <<"colquantile"
                <<"colimagewidth"
                <<"colimageheight"
                <<"collength"
                <<"columntitles"
                <<"columntitle"
                <<"columnindexbytitle"
                <<"column"
                <<"uniquecolumn"
                <<"indexedcolavg"
                <<"indexedcolsum"
                <<"indexedcolsum2"
                <<"indexedcolvar"
                <<"indexedcolstd"
                <<"indexedcolmin"
                <<"indexedcolmax"
                <<"indexedcolmedian"
                <<"indexedcolquantile"
                <<"columndata"
                <<"columnstr"
                <<"columnbool"
                <<"rowdata"
                <<"rowstr"
                <<"rowbool"
                <<"colsavg"
                <<"colssum"
                <<"colssum2"
                <<"colsvar"
                <<"colsstd"
                <<"colsmin"
                <<"colsmax"
                <<"colsmedian"
                <<"colsquantile";
        //}
    }
}

void fQFRDRTableEditor_coltitles(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
    res.setInvalid();
    res.num=NAN;
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            if (n==0) {
                QStringList colt;
                for (int i=0; i<d->model->columnCount(); i++) colt.append(d->model->columnTitle(i));
                res.setStringVec(colt);
            } else {
                p->qfmpError("columntitles() has no arguments");
            }
        }
    }

}

void fQFRDRTableEditor_coltitle(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
    res.setInvalid();
    res.num=NAN;
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            if (n==1 && (params[0].type==qfmpDouble) ) {
                QStringList colt;
                for (int i=0; i<d->model->columnCount(); i++) colt.append(d->model->columnTitle(i));
                res.setString(colt.value(params[0].toInteger()-1, ""));
            } else {
                p->qfmpError("columntitle(col) needs one integer arguments");
            }
        }
    }

}

void fQFRDRTableEditor_colindexbytitle(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
    res.setInvalid();
    res.num=NAN;
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            if (n==1 && (params[0].type==qfmpString) ) {
                QStringList colt;
                for (int i=0; i<d->model->columnCount(); i++) colt.append(d->model->columnTitle(i));
                res.setDouble(colt.indexOf(params[0].str)+1);
            } else {
                p->qfmpError("columnindexbytitle(col) needs one string arguments");
            }
        }
    }

}

void fQFRDRTableEditor_columnStr(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
    res.setInvalid();
    res.num=NAN;
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            if (n==1) {
                if ((params[0].type!=qfmpDouble))  {
                    p->qfmpError("columnstr(column) needs one integer arguments");
                    return;
                }
                int c=floor(params[0].num-1);
                if (c>=0 && c<d->model->columnCount()) {
                    QVariantList vl=d->model->getColumnData(c, Qt::EditRole);
                    QStringList sl;
                    for (int i=0; i<vl.size(); i++) {
                        sl.append(vl[i].toString());
                    }
                    res.setStringVec(sl);
                }
            } else if (n==2) {
                if ((params[0].type!=qfmpDouble)||(params[1].type!=qfmpDouble&&params[1].type!=qfmpDoubleVector)) {
                    p->qfmpError("columnstr(column, rows) needs one integer and one integer vector arguments");
                    return;
                }
                int c=floor(params[0].num-1);
                QVector<int> r=params[1].asIntVector();
                for (int i=0; i<r.size(); i++) r[i]=r[i]-1;
                if (c>=0 && c<d->model->columnCount()) {
                    QVariantList dat=d->model->getColumnData(c, Qt::EditRole);
                    QStringList dat1;
                    for (int i=0; i<r.size(); i++) {
                        if (r[i]>=0 && r[i]<dat.size()) {
                            dat1<<dat[r[i]].toString();
                        }
                    }
                    res.setStringVec(dat1);
                }
            } else {
                p->qfmpError("columnstr(column)/columnstr(column, rows) needs 1 or 2 arguments");
            }
        }
    }

}


void fQFRDRTableEditor_columnBool(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
    res.setInvalid();
    res.num=NAN;
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            if (n==1) {
                if ((params[0].type!=qfmpDouble)) {
                    p->qfmpError("columnbool(column) needs one integer arguments");
                    return;
                }
                int c=floor(params[0].num-1);
                if (c>=0 && c<d->model->columnCount()) {
                    QVariantList vl=d->model->getColumnData(c, Qt::EditRole);
                    QVector<bool> sl;
                    for (int i=0; i<vl.size(); i++) {
                        sl.append(vl[i].toBool());
                    }
                    res.setBoolVec(sl);
                }
            } else if (n==2) {
                if ((params[0].type!=qfmpDouble)||(params[1].type!=qfmpDouble&&params[1].type!=qfmpDoubleVector)) {
                    p->qfmpError("columnbool(column, rows) needs one integer and one integer vector arguments");
                    return;
                }
                int c=floor(params[0].num-1);
                QVector<int> r=params[1].asIntVector();
                for (int i=0; i<r.size(); i++) r[i]=r[i]-1;
                if (c>=0 && c<d->model->columnCount()) {
                    QVariantList dat=d->model->getColumnData(c, Qt::EditRole);
                    QVector<bool> dat1;
                    for (int i=0; i<r.size(); i++) {
                        if (r[i]>=0 && r[i]<dat.size()) {
                            dat1<<dat[r[i]].toBool();
                        }
                    }
                    res.setBoolVec(dat1);
                }
            } else {
                p->qfmpError("columnbool(column)/columnbool(column, rows) needs 1 or 2 arguments");
            }
        }
    }

}

void fQFRDRTableEditor_column(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
    res.setInvalid();
    res.num=NAN;
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            if (n==1) {
                if ((params[0].type!=qfmpDouble)) {
                    p->qfmpError("column(column) needs one integer arguments");
                    return;
                }
                int c=floor(params[0].num-1);
                if (c>=0 && c<d->model->columnCount()) {

                    res.setDoubleVec(d->model->getColumnDataAsNumbers(c, Qt::EditRole));
                }
            } else if (n==2) {
                if ((params[0].type!=qfmpDouble)||(params[1].type!=qfmpDouble&&params[1].type!=qfmpDoubleVector)) {
                    p->qfmpError("column(column, rows) needs one integer and one integer vector arguments");
                    return;
                }
                int c=floor(params[0].num-1);
                QVector<int> r=params[1].asIntVector();
                for (int i=0; i<r.size(); i++) r[i]=r[i]-1;
                if (c>=0 && c<d->model->columnCount()) {
                    QVector<double> dat=d->model->getColumnDataAsNumbers(c, Qt::EditRole);
                    QVector<double> dat1;
                    for (int i=0; i<r.size(); i++) {
                        if (r[i]>=0 && r[i]<dat.size()) {
                            dat1<<dat[r[i]];
                        }
                    }
                    res.setDoubleVec(dat1);
                }
            } else {
                p->qfmpError("column(column)/column(column, rows) needs 1 or 2 arguments");
            }
        }
    }

}











void fQFRDRTableEditor_rowStr(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
    res.setInvalid();
    res.num=NAN;
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            if (n==1) {
                if ((params[0].type!=qfmpDouble)) {
                    p->qfmpError("rownstr(row) needs one integer arguments");
                    return;
                }
                int r=floor(params[0].num-1);
                if (r>=0 && r<d->model->rowCount()) {
                    QVariantList vl;
                    for (int i=0; i<d->model->columnCount(); i++) {
                        vl<<d->model->data(d->model->index(r, i), Qt::EditRole);
                    }
                    QStringList sl;
                    for (int i=0; i<vl.size(); i++) {
                        sl.append(vl[i].toString());
                    }
                    res.setStringVec(sl);
                }
            } else if (n==2) {
                if ((params[0].type!=qfmpDouble)||(params[1].type!=qfmpDouble&&params[1].type!=qfmpDoubleVector)) {
                    p->qfmpError("rowstr(row, columns) needs one integer and one integer vector arguments");
                    return;
                }
                int r=floor(params[0].num-1);
                QVector<int> cols=params[1].asIntVector();
                for (int i=0; i<cols.size(); i++) cols[i]=cols[i]-1;
                if (r>=0 && r<d->model->rowCount()) {
                    QStringList dat1;
                    for (int i=0; i<cols.size(); i++) {
                        if (cols[i]>=0 && cols[i]<d->model->columnCount()) {
                            dat1<<d->model->data(d->model->index(r, cols[i]), Qt::EditRole).toString();
                        }
                    }
                    res.setStringVec(dat1);
                }
            } else {
                p->qfmpError("rowstr(column)/rowstr(row, columns) needs 1 or 2 arguments");
            }
        }
    }

}


void fQFRDRTableEditor_rowBool(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
    res.setInvalid();
    res.num=NAN;
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            if (n==1) {
                if ((params[0].type!=qfmpDouble)){
                    p->qfmpError("rowbool(row) needs one integer arguments");
                    return;
                }
                int r=floor(params[0].num-1);
                if (r>=0 && r<d->model->rowCount()) {
                    QVariantList vl;
                    for (int i=0; i<d->model->columnCount(); i++) {
                        vl<<d->model->data(d->model->index(r, i), Qt::EditRole);
                    }
                    QVector<bool> sl;
                    for (int i=0; i<vl.size(); i++) {
                        sl.append(vl[i].toBool());
                    }
                    res.setBoolVec(sl);
                }
            } else if (n==2) {
                if ((params[0].type!=qfmpDouble)||(params[1].type!=qfmpDouble&&params[1].type!=qfmpDoubleVector)) {
                    p->qfmpError("rowbool(row, columns) needs one integer and one integer vector arguments");
                    return;
                }
                int r=floor(params[0].num-1);
                QVector<int> cols=params[1].asIntVector();
                for (int i=0; i<cols.size(); i++) cols[i]=cols[i]-1;
                if (r>=0 && r<d->model->rowCount()) {
                    QVector<bool> dat1;
                    for (int i=0; i<cols.size(); i++) {
                        if (cols[i]>=0 && cols[i]<d->model->columnCount()) {
                            dat1<<d->model->data(d->model->index(r, cols[i]), Qt::EditRole).toBool();
                        }
                    }
                    res.setBoolVec(dat1);
                }
            } else {
                p->qfmpError("rowbool(row)/rowbool(row, columns) needs 1 or 2 arguments");
            }
        }
    }

}

void fQFRDRTableEditor_row(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
    res.setInvalid();
    res.num=NAN;
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            if (n==1) {
                if ((params[0].type!=qfmpDouble)) {
                    p->qfmpError("rowdata(row) needs one integer arguments");
                    return ;
                }
                int r=floor(params[0].num-1);
                if (r>=0 && r<d->model->rowCount()) {
                    QVariantList vl;
                    for (int i=0; i<d->model->columnCount(); i++) {
                        vl<<d->model->data(d->model->index(r, i), Qt::EditRole);
                    }
                    QVector<double> sl;
                    for (int i=0; i<vl.size(); i++) {
                        //sl.append(vl[i].toBool());
                        const QVariant& dv=vl[i];
                        bool ok=false;
                        double dd=dv.toDouble(&ok);
                        if (dv.isValid() && (dv.type()==QVariant::Double || dv.type()==QVariant::Int || dv.type()==QVariant::UInt || dv.type()==QVariant::Bool || dv.type()==QVariant::LongLong || dv.type()==QVariant::ULongLong) && ok) {
                            sl.append(dd);
                        } else if (vl.size()>0) {
                            sl.append(NAN);
                        }
                    }
                    res.setDoubleVec(sl);
                }
            } else if (n==2) {
                if ((params[0].type!=qfmpDouble)||(params[1].type!=qfmpDouble&&params[1].type!=qfmpDoubleVector)) {
                    p->qfmpError("rowdata(row, columns) needs one integer and one integer vector arguments");
                    return ;
                }


                int r=floor(params[0].num-1);
                QVector<int> cols=params[1].asIntVector();
                for (int i=0; i<cols.size(); i++) cols[i]=cols[i]-1;
                if (r>=0 && r<d->model->rowCount()) {
                    QVector<double> dat1;
                    for (int i=0; i<cols.size(); i++) {
                        if (cols[i]>=0 && cols[i]<d->model->columnCount()) {
                            const QVariant& dv=d->model->data(d->model->index(r, cols[i]), Qt::EditRole);
                            bool ok=false;
                            double dd=dv.toDouble(&ok);
                            if (dv.isValid() && (dv.type()==QVariant::Double || dv.type()==QVariant::Int || dv.type()==QVariant::UInt || dv.type()==QVariant::Bool || dv.type()==QVariant::LongLong || dv.type()==QVariant::ULongLong) && ok) {
                                dat1.append(dd);
                            } else {
                                dat1.append(NAN);
                            }
                        }
                    }
                    res.setDoubleVec(dat1);
                }

            } else {
                p->qfmpError("rowdata(row)/rowdata(row, columns) needs 1 or 2 arguments");
            }
        }
    }

}















void fQFRDRTableEditor_data(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
    res.setInvalid();
    res.num=NAN;
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            res.type=qfmpDouble;
            if (n!=2) {
                p->qfmpError("data(row, column) needs 2 argument");
                return ;
            }
            if ((!(params[0].type==qfmpDouble)&&(params[1].type==qfmpDouble)) && (!((params[0].type==qfmpDouble)&&params[1].convertsToIntVector())) && (!((params[1].type==qfmpDouble)&&params[0].convertsToIntVector()))) {
                p->qfmpError("data(row, column) needs two integer/integer vector arguments");
                return ;
            }
            if ((params[0].type==qfmpDouble)&&(params[1].type==qfmpDouble)) {
                int r=floor(params[0].num-1);
                int c=floor(params[1].num-1);
                if (r>=0 && c>=0 && r<d->model->rowCount() && c<d->model->columnCount()) {
                    QVariant da= d->model->cell(r,c);
                    switch(da.type()) {
                        case QVariant::LongLong :
                        case QVariant::ULongLong :
                        case QVariant::Int :
                        case QVariant::UInt :
                        case QVariant::Double: res.num=da.toDouble(); res.type=qfmpDouble; break;
                        case QVariant::Date:
                        case QVariant::DateTime: res.num=da.toDateTime().toMSecsSinceEpoch(); res.type=qfmpDouble; break;
                        case QVariant::Time: res.num=QDateTime(QDate::currentDate(), da.toTime()).toMSecsSinceEpoch(); res.type=qfmpDouble; break;
                        case QVariant::Bool: res.boolean=da.toBool(); res.type=qfmpBool; break;
                        case QVariant::String:
                            res.str=da.toString(); res.type=qfmpString; break;
                        default:
                            if (da.canConvert(QVariant::Double)) {
                                res.num=da.toDouble(); res.type=qfmpDouble; break;
                            } else {
                                res.str=da.toString(); res.type=qfmpString; break;
                            }
                            break;
                    }
                }
            } else if ((params[0].type==qfmpDouble)&&params[1].convertsToIntVector()) {
                int r=params[0].toInteger()-1;
                QVector<int> cols=params[1].asIntVector();
                QVector<double> resv;
                if (r>=0 && r<d->model->rowCount()) {
                    for (int ci=0; ci<cols.size(); ci++) {
                        int c=cols[ci]-1;
                        if (c>=0 && c<d->model->columnCount()) {
                            QVariant da= d->model->cell(r,c);
                            switch(da.type()) {
                                case QVariant::LongLong :
                                case QVariant::ULongLong :
                                case QVariant::Int :
                                case QVariant::UInt :
                                case QVariant::Double: resv.append(da.toDouble()); break;
                                case QVariant::Date:
                                case QVariant::DateTime: resv.append(da.toDateTime().toMSecsSinceEpoch()); break;
                                case QVariant::Time: resv.append(QDateTime(QDate::currentDate(), da.toTime()).toMSecsSinceEpoch()); break;
                                default:
                                    if (da.canConvert(QVariant::Double)) {
                                        resv.append(da.toDouble()); break;
                                    } else {
                                    }
                                    break;
                            }

                        }
                    }
                }
                res.setDoubleVec(resv);
            } else if ((params[1].type==qfmpDouble)&&params[0].convertsToIntVector()) {
                int c=params[1].toInteger()-1;
                QVector<int> rows=params[0].asIntVector();
                QVector<double> resv;
                if (c>=0 && c<d->model->columnCount()) {
                    for (int i=0; i<rows.size(); i++) {
                        int r=rows[i]-1;
                        if (r>=0 && r<d->model->rowCount()) {
                            QVariant da= d->model->cell(r,c);
                            switch(da.type()) {
                                case QVariant::LongLong :
                                case QVariant::ULongLong :
                                case QVariant::Int :
                                case QVariant::UInt :
                                case QVariant::Double: resv.append(da.toDouble()); break;
                                case QVariant::Date:
                                case QVariant::DateTime: resv.append(da.toDateTime().toMSecsSinceEpoch()); break;
                                case QVariant::Time: resv.append(QDateTime(QDate::currentDate(), da.toTime()).toMSecsSinceEpoch()); break;
                                default:
                                    if (da.canConvert(QVariant::Double)) {
                                        resv.append(da.toDouble()); break;
                                    } else {
                                    }
                                    break;
                            }

                        }
                    }
                }
                res.setDoubleVec(resv);
            }
        }
    }

}

void fQFRDRTableEditor_dataleft(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
    res.setInvalid();
    res.num=NAN;
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            res.type=qfmpDouble;
            if (n!=1) {
                p->qfmpError("dataleft(delta_column) needs 1 argument");
                return ;
            }
            if ((params[0].type!=qfmpDouble)) {
                p->qfmpError("dataleft(delta_column) needs one integer arguments");
                return ;
            }
            int delta=floor(params[0].num);
            QVariant da= d->model->cell(d->row,d->column-delta);
            switch(da.type()) {
                case QVariant::LongLong :
                case QVariant::ULongLong :
                case QVariant::Int :
                case QVariant::UInt :
                case QVariant::Double: res.num=da.toDouble(); res.type=qfmpDouble; break;
                case QVariant::Date:
                case QVariant::DateTime: res.num=da.toDateTime().toMSecsSinceEpoch(); res.type=qfmpDouble; break;
                case QVariant::Time: res.num=QDateTime(QDate::currentDate(), da.toTime()).toMSecsSinceEpoch(); res.type=qfmpDouble; break;
                case QVariant::Bool: res.boolean=da.toBool(); res.type=qfmpBool; break;
                case QVariant::String:
                    res.str=da.toString(); res.type=qfmpString; break;
                default:
                    if (da.canConvert(QVariant::Double)) {
                        res.num=da.toDouble(); res.type=qfmpDouble; break;
                    } else {
                        res.str=da.toString(); res.type=qfmpString; break;
                    }
                    break;
            }
        }
    }

}


void fQFRDRTableEditor_colavg(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
    res.setInvalid();
    res.num=NAN;
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            res.type=qfmpDouble;
            QList<double> data;
            if (n==1) {
                if ((params[0].type!=qfmpDouble)) {
                    p->qfmpError("colavg(column) needs one integer argument");
                    return ;
                }
                for (int i=0; i<d->model->rowCount(); i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else if (n==3) {
                if ((params[0].type!=qfmpDouble)||(params[1].type!=qfmpDouble)||(params[2].type!=qfmpDouble)) {
                    p->qfmpError("colavg(column, range_start, range_end) needs three integer argument");
                    return ;
                }
                for (int i=params[1].num-1; i<=params[2].num-1; i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else {
                p->qfmpError("colavg(column [, range_start, range_end]) needs 1 or 3 arguments argument");
                return ;
            }


            res.num=qfstatisticsAverage(data);
            res.type=qfmpDouble;

        }
    }

}


void fQFRDRTableEditor_colsum(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
    res.setInvalid();
    res.num=NAN;
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            res.type=qfmpDouble;
            QList<double> data;
            if (n==1) {
                if ((params[0].type!=qfmpDouble)) {
                    p->qfmpError("colsum(column) needs one integer argument");
                    return ;
                }
                for (int i=0; i<d->model->rowCount(); i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else if (n==3) {
                if ((params[0].type!=qfmpDouble)||(params[1].type!=qfmpDouble)||(params[2].type!=qfmpDouble)) {
                    p->qfmpError("colsum(column, range_start, range_end) needs three integer argument");
                    return ;
                }
                for (int i=params[1].num-1; i<=params[2].num-1; i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else {
                p->qfmpError("colsum(column [, range_start, range_end]) needs 1 or 3 arguments argument");
                return ;
            }


            res.num=qfstatisticsSum(data);
            res.type=qfmpDouble;

        }
    }
    return ;
}

void fQFRDRTableEditor_colsum2(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
    res.setInvalid();
    res.num=NAN;
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            res.type=qfmpDouble;
            QList<double> data;
            if (n==1) {
                if ((params[0].type!=qfmpDouble)) {
                    p->qfmpError("colsum2(column) needs one integer argument");
                    return ;
                }
                for (int i=0; i<d->model->rowCount(); i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else if (n==3) {
                if ((params[0].type!=qfmpDouble)||(params[1].type!=qfmpDouble)||(params[2].type!=qfmpDouble)) {
                    p->qfmpError("colsum2(column, range_start, range_end) needs three integer argument");
                    return ;
                }
                for (int i=params[1].num-1; i<=params[2].num-1; i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else {
                p->qfmpError("colsum2(column [, range_start, range_end]) needs 1 or 3 arguments argument");
                return ;
            }


            res.num=qfstatisticsSum2(data);
            res.type=qfmpDouble;

        }
    }
    return ;
}



void fQFRDRTableEditor_colvar(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
    res.setInvalid();
    res.num=NAN;
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            res.type=qfmpDouble;
            QList<double> data;
            if (n==1) {
                if ((params[0].type!=qfmpDouble)) {
                    p->qfmpError("colvar(column) needs one integer argument");
                    return ;
                }
                for (int i=0; i<d->model->rowCount(); i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else if (n==3) {
                if ((params[0].type!=qfmpDouble)||(params[1].type!=qfmpDouble)||(params[2].type!=qfmpDouble)){
                    p->qfmpError("colvar(column, range_start, range_end) needs three integer argument");
                    return ;
                }
                for (int i=params[1].num-1; i<=params[2].num-1; i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else {
                p->qfmpError("colvar(column [, range_start, range_end]) needs 1 or 3 arguments argument");
                return ;
            }

            res.num=qfstatisticsVariance(data);
            res.type=qfmpDouble;

        }
    }
    return ;
}


void fQFRDRTableEditor_colstd(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
    res.setInvalid();
    res.num=NAN;
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            res.type=qfmpDouble;
            QList<double> data;
            if (n==1) {
                if ((params[0].type!=qfmpDouble)) {
                    p->qfmpError("colstd(column) needs one integer argument");
                    return ;
                }
                for (int i=0; i<d->model->rowCount(); i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else if (n==3) {
                if ((params[0].type!=qfmpDouble)||(params[1].type!=qfmpDouble)||(params[2].type!=qfmpDouble)) {
                    p->qfmpError("colstd(column, range_start, range_end) needs three integer argument");
                    return ;
                }
                for (int i=params[1].num-1; i<=params[2].num-1; i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else {
                p->qfmpError("colstd(column [, range_start, range_end]) needs 1 or 3 arguments argument");
                return ;
            }

            res.num=sqrt(qfstatisticsVariance(data));
            res.type=qfmpDouble;

        }
    }
    return ;
}


void fQFRDRTableEditor_colmin(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
    res.setInvalid();
    res.num=NAN;
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            res.type=qfmpDouble;
            QList<double> data;
            if (n==1) {
                if ((params[0].type!=qfmpDouble)) {
                    p->qfmpError("colmin(column) needs one integer argument");
                    return ;
                }
                for (int i=0; i<d->model->rowCount(); i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else if (n==3) {
                if ((params[0].type!=qfmpDouble)||(params[1].type!=qfmpDouble)||(params[2].type!=qfmpDouble)) {
                    p->qfmpError("colmin(column, range_start, range_end) needs three integer argument");
                    return ;
                }
                for (int i=params[1].num-1; i<=params[2].num-1; i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else {
                p->qfmpError("colmin(column [, range_start, range_end]) needs 1 or 3 arguments argument");
                return ;
            }
            qSort(data);
            res.num=qfstatisticsSortedMin(data);
            res.type=qfmpDouble;

        }
    }
    return ;
}


void fQFRDRTableEditor_colmax(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
    res.setInvalid();
    res.num=NAN;
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            res.type=qfmpDouble;
            QList<double> data;
            if (n==1) {
                if ((params[0].type!=qfmpDouble)) {
                    p->qfmpError("colmax(column) needs one integer argument");
                    return ;
                }
                for (int i=0; i<d->model->rowCount(); i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else if (n==3) {
                if ((params[0].type!=qfmpDouble)||(params[1].type!=qfmpDouble)||(params[2].type!=qfmpDouble)) {
                    p->qfmpError("colmax(column, range_start, range_end) needs three integer argument");
                    return ;
                }
                for (int i=params[1].num-1; i<=params[2].num-1; i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else {
                p->qfmpError("colmax(column [, range_start, range_end]) needs 1 or 3 arguments argument");
                return ;
            }
            qSort(data);
            res.num=qfstatisticsSortedMax(data);
            res.type=qfmpDouble;

        }
    }
    return ;
}


void fQFRDRTableEditor_colmedian(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
    res.setInvalid();
    res.num=NAN;
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            res.type=qfmpDouble;
            QList<double> data;
            if (n==1) {
                if ((params[0].type!=qfmpDouble)) {
                    p->qfmpError("colmedian(column) needs one integer argument");
                    return ;
                }
                for (int i=0; i<d->model->rowCount(); i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else if (n==3) {
                if ((params[0].type!=qfmpDouble)||(params[1].type!=qfmpDouble)||(params[2].type!=qfmpDouble)) {
                    p->qfmpError("colmedian(column, range_start, range_end) needs three integer argument");
                    return ;
                }
                for (int i=params[1].num-1; i<=params[2].num-1; i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else {
                p->qfmpError("colmedian(column [, range_start, range_end]) needs 1 or 3 arguments argument");
                return ;
            }
            qSort(data);
            res.num=qfstatisticsSortedMedian(data);
            res.type=qfmpDouble;

        }
    }
    return ;
}



void fQFRDRTableEditor_colquantile(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
    res.setInvalid();
    res.num=NAN;
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            res.type=qfmpDouble;
            QList<double> data;
            double quantile=0;
            if (n==2) {
                if ((params[0].type!=qfmpDouble)||(params[1].type!=qfmpDouble))  {
                    p->qfmpError("colquantile(column, quantile) needs two number argument");
                    return ;
                }
                for (int i=0; i<d->model->rowCount(); i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
                quantile=params[1].num;
            } else if (n==4) {
                if ((params[0].type!=qfmpDouble)||(params[1].type!=qfmpDouble)||(params[2].type!=qfmpDouble)||(params[3].type!=qfmpDouble)) {
                    p->qfmpError("colquantile(column, range_start, range_end, quantile) needs four number argument");
                    return ;
                }
                for (int i=params[1].num-1; i<=params[2].num-1; i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
                quantile=params[3].num;
            } else {
                p->qfmpError("colquantile(column [, range_start, range_end], quantile) needs 2 or 4 arguments ");
                return ;
            }
            qSort(data);
            res.num=qfstatisticsSortedQuantile(data, quantile);
            res.type=qfmpDouble;

        }
    }
    return ;
}
















void fQFRDRTableEditor_columnUnique(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
    res.setInvalid();
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            if (n!=1) {
                p->qfmpError("uniquecolumn(column) needs 1 argument");
                return ;
            }
            if ((params[0].type!=qfmpDouble)) {
                p->qfmpError("uniquecolumn(column) needs one integer arguments");
                return ;
            }
            int c=floor(params[0].num-1);
            if (c>=0 && c<d->model->columnCount()) {
                QVariantList vl=d->model->getColumnData(c, Qt::EditRole);
                QVector<double> dv;
                QStringList sl;
                bool isNumber=true;
                for (int i=0; i<vl.size(); i++) {
                    bool ok=false;
                    if (vl[i].canConvert(QVariant::Double)) {
                        double d=vl[i].toDouble(&ok);
                        dv<<d;
                        if (!ok) isNumber=false;
                    } else {
                        isNumber=false;
                    }
                    sl<<vl[i].toString();
                }
                if (isNumber) {
                    QVector<double> dvu;
                    for (int i=0; i<dv.size(); i++) {
                        if (!dvu.contains(dv[i])) dvu<<dv[i];
                    }
                    res.setDoubleVec(dvu);
                } else {
                    QStringList slu;
                    for (int i=0; i<sl.size(); i++) {
                        if (!slu.contains(sl[i])) slu<<sl[i];
                    }
                    res.setStringVec(slu);
                }
            }
        }
    }
    return ;
}





void fQFRDRTableEditor_indexedColAvg(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
    res.setInvalid();
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            if (n!=2) {
                p->qfmpError("indexedcolavg(column, index) needs 2 argument");
                return ;
            }
            if ((params[0].type!=qfmpDouble)||(params[1].type!=qfmpDouble)) {
                p->qfmpError("indexedcolavg(column, index) needs two integer arguments");
                return ;
            }
            int c=floor(params[0].num-1);
            int idx=floor(params[1].num-1);
            if (c>=0 && c<d->model->columnCount() && idx>=0 && idx<d->model->columnCount()) {
                QVector<double> data=d->model->getColumnDataAsNumbers(c, Qt::EditRole);
                QVector<double> resvec;
                QVariantList idxvl=d->model->getColumnData(idx, Qt::EditRole);
                QVector<double> idxdv;
                QStringList idxsl;
                bool isNumber=true;
                for (int i=0; i<idxvl.size(); i++) {
                    bool ok=false;
                    if (idxvl[i].canConvert(QVariant::Double)) {
                        double d=idxvl[i].toDouble(&ok);
                        idxdv<<d;
                        if (!ok) isNumber=false;
                    } else {
                        isNumber=false;
                    }
                    idxsl<<idxvl[i].toString();
                }
                if (isNumber) {
                    QMap<double, QList<double> > datasets;
                    QVector<double> dvu;
                    for (int i=0; i<qMin(data.size(), idxdv.size()); i++) {
                        if (!dvu.contains(idxdv[i])) dvu<<idxdv[i];
                        datasets[idxdv[i]].append(data.value(i, NAN));
                    }
                    for (int i=0; i<dvu.size(); i++) {
                        //qDebug()<<"number col:"<<qfstatisticsAverage(datasets[dvu[i]])<<datasets[dvu[i]];
                        resvec<<qfstatisticsAverage(datasets[dvu[i]]);
                    }
                } else {
                    QMap<QString, QList<double> > datasets;
                    QStringList slu;
                    for (int i=0; i<qMin(data.size(), idxsl.size()); i++) {
                        if (!slu.contains(idxsl[i])) slu<<idxsl[i];
                        datasets[idxsl[i]].append(data.value(i, NAN));
                    }
                    for (int i=0; i<slu.size(); i++) {
                        resvec<<qfstatisticsAverage(datasets[slu[i]]);
                    }
                }
                res.setDoubleVec(resvec);
            }
        }
    }
    return ;
}



void fQFRDRTableEditor_indexedColSum(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
    res.setInvalid();
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            if (n!=2) {
                p->qfmpError("indexedcolsum(column, index) needs 2 argument");
                return ;
            }
            if ((params[0].type!=qfmpDouble)||(params[1].type!=qfmpDouble)) {
                p->qfmpError("indexedcolsum(column, index) needs two integer arguments");
                return ;
            }
            int c=floor(params[0].num-1);
            int idx=floor(params[1].num-1);
            if (c>=0 && c<d->model->columnCount() && idx>=0 && idx<d->model->columnCount()) {
                QVector<double> data=d->model->getColumnDataAsNumbers(c, Qt::EditRole);
                QVector<double> resvec;
                QVariantList idxvl=d->model->getColumnData(idx, Qt::EditRole);
                QVector<double> idxdv;
                QStringList idxsl;
                bool isNumber=true;
                for (int i=0; i<idxvl.size(); i++) {
                    bool ok=false;
                    if (idxvl[i].canConvert(QVariant::Double)) {
                        double d=idxvl[i].toDouble(&ok);
                        idxdv<<d;
                        if (!ok) isNumber=false;
                    } else {
                        isNumber=false;
                    }
                    idxsl<<idxvl[i].toString();
                }
                if (isNumber) {
                    QMap<double, QList<double> > datasets;
                    QVector<double> dvu;
                    for (int i=0; i<qMin(data.size(), idxdv.size()); i++) {
                        if (!dvu.contains(idxdv[i])) dvu<<idxdv[i];
                        datasets[idxdv[i]].append(data.value(i, NAN));
                    }
                    for (int i=0; i<dvu.size(); i++) {
                        //qDebug()<<"number col:"<<qfstatisticsSum(datasets[dvu[i]])<<datasets[dvu[i]];
                        resvec<<qfstatisticsSum(datasets[dvu[i]]);
                    }
                } else {
                    QMap<QString, QList<double> > datasets;
                    QStringList slu;
                    for (int i=0; i<qMin(data.size(), idxsl.size()); i++) {
                        if (!slu.contains(idxsl[i])) slu<<idxsl[i];
                        datasets[idxsl[i]].append(data.value(i, NAN));
                    }
                    for (int i=0; i<slu.size(); i++) {
                        resvec<<qfstatisticsSum(datasets[slu[i]]);
                    }
                }
                res.setDoubleVec(resvec);
            }
        }
    }
    return ;
}



void fQFRDRTableEditor_indexedColSum2(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
    res.setInvalid();
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            if (n!=2) {
                p->qfmpError("indexedcolsum2(column, index) needs 2 argument");
                return ;
            }
            if ((params[0].type!=qfmpDouble)||(params[1].type!=qfmpDouble)) {
                p->qfmpError("indexedcolsum2(column, index) needs two integer arguments");
                return ;
            }
            int c=floor(params[0].num-1);
            int idx=floor(params[1].num-1);
            if (c>=0 && c<d->model->columnCount() && idx>=0 && idx<d->model->columnCount()) {
                QVector<double> data=d->model->getColumnDataAsNumbers(c, Qt::EditRole);
                QVector<double> resvec;
                QVariantList idxvl=d->model->getColumnData(idx, Qt::EditRole);
                QVector<double> idxdv;
                QStringList idxsl;
                bool isNumber=true;
                for (int i=0; i<idxvl.size(); i++) {
                    bool ok=false;
                    if (idxvl[i].canConvert(QVariant::Double)) {
                        double d=idxvl[i].toDouble(&ok);
                        idxdv<<d;
                        if (!ok) isNumber=false;
                    } else {
                        isNumber=false;
                    }
                    idxsl<<idxvl[i].toString();
                }
                if (isNumber) {
                    QMap<double, QList<double> > datasets;
                    QVector<double> dvu;
                    for (int i=0; i<qMin(data.size(), idxdv.size()); i++) {
                        if (!dvu.contains(idxdv[i])) dvu<<idxdv[i];
                        datasets[idxdv[i]].append(data.value(i, NAN));
                    }
                    for (int i=0; i<dvu.size(); i++) {
                        //qDebug()<<"number col:"<<qfstatisticsSum(datasets[dvu[i]])<<datasets[dvu[i]];
                        resvec<<qfstatisticsSum2(datasets[dvu[i]]);
                    }
                } else {
                    QMap<QString, QList<double> > datasets;
                    QStringList slu;
                    for (int i=0; i<qMin(data.size(), idxsl.size()); i++) {
                        if (!slu.contains(idxsl[i])) slu<<idxsl[i];
                        datasets[idxsl[i]].append(data.value(i, NAN));
                    }
                    for (int i=0; i<slu.size(); i++) {
                        resvec<<qfstatisticsSum2(datasets[slu[i]]);
                    }
                }
                res.setDoubleVec(resvec);
            }
        }
    }
    return ;
}





void fQFRDRTableEditor_indexedColStd(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
    res.setInvalid();
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            if (n!=2) {
                p->qfmpError("indexedcolstd(column, index) needs 2 argument");
                return ;
            }
            if ((params[0].type!=qfmpDouble)||(params[1].type!=qfmpDouble)) {
                p->qfmpError("indexedcolstd(column, index) needs two integer arguments");
                return ;
            }
            int c=floor(params[0].num-1);
            int idx=floor(params[1].num-1);
            if (c>=0 && c<d->model->columnCount() && idx>=0 && idx<d->model->columnCount()) {
                QVector<double> data=d->model->getColumnDataAsNumbers(c, Qt::EditRole);
                QVector<double> resvec;
                QVariantList idxvl=d->model->getColumnData(idx, Qt::EditRole);
                QVector<double> idxdv;
                QStringList idxsl;
                bool isNumber=true;
                for (int i=0; i<qMin(idxvl.size(), data.size()); i++) {
                    bool ok=false;
                    if (idxvl[i].canConvert(QVariant::Double)) {
                        double d=idxvl[i].toDouble(&ok);
                        idxdv<<d;
                        if (!ok) isNumber=false;
                    } else {
                        isNumber=false;
                    }
                    idxsl<<idxvl[i].toString();
                }
                if (isNumber) {
                    QMap<double, QList<double> > datasets;
                    QVector<double> dvu;
                    for (int i=0; i<qMin(data.size(), idxdv.size()); i++) {
                        if (!dvu.contains(idxdv[i])) dvu<<idxdv[i];
                        if (i<data.size()) datasets[idxdv[i]].append(data.value(i, NAN));
                    }
                    for (int i=0; i<dvu.size(); i++) {
                        resvec<<qfstatisticsStd(datasets[dvu[i]]);
                    }
                } else {
                    QMap<QString, QList<double> > datasets;
                    QStringList slu;
                    for (int i=0; i<qMin(data.size(), idxsl.size()); i++) {
                        if (!slu.contains(idxsl[i])) slu<<idxsl[i];
                        if (i<data.size()) datasets[idxsl[i]].append(data.value(i, NAN));
                    }
                    for (int i=0; i<slu.size(); i++) {
                        resvec<<qfstatisticsStd(datasets[slu[i]]);
                    }
                }
                res.setDoubleVec(resvec);
            }
        }
    }
    return ;
}





void fQFRDRTableEditor_indexedColVar(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
    res.setInvalid();
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            if (n!=2)  {
                p->qfmpError("indexedcolvar(column, index) needs 2 argument");
                return ;
            }
            if ((params[0].type!=qfmpDouble)||(params[1].type!=qfmpDouble)) {
                p->qfmpError("indexedcolvar(column, index) needs two integer arguments");
                return ;
            }
            int c=floor(params[0].num-1);
            int idx=floor(params[1].num-1);
            if (c>=0 && c<d->model->columnCount() && idx>=0 && idx<d->model->columnCount()) {
                QVector<double> data=d->model->getColumnDataAsNumbers(c, Qt::EditRole);
                QVector<double> resvec;
                QVariantList idxvl=d->model->getColumnData(idx, Qt::EditRole);
                QVector<double> idxdv;
                QStringList idxsl;
                bool isNumber=true;
                for (int i=0; i<idxvl.size(); i++) {
                    bool ok=false;
                    if (idxvl[i].canConvert(QVariant::Double)) {
                        double d=idxvl[i].toDouble(&ok);
                        idxdv<<d;
                        if (!ok) isNumber=false;
                    } else {
                        isNumber=false;
                    }
                    idxsl<<idxvl[i].toString();
                }
                if (isNumber) {
                    QMap<double, QList<double> > datasets;
                    QVector<double> dvu;
                    for (int i=0; i<qMin(data.size(), idxdv.size()); i++) {
                        if (!dvu.contains(idxdv[i])) dvu<<idxdv[i];
                        datasets[idxdv[i]].append(data.value(i, NAN));
                    }
                    for (int i=0; i<dvu.size(); i++) {
                        resvec<<qfstatisticsVariance(datasets[dvu[i]]);
                    }
                } else {
                    QMap<QString, QList<double> > datasets;
                    QStringList slu;
                    for (int i=0; i<qMin(data.size(), idxsl.size()); i++) {
                        if (!slu.contains(idxsl[i])) slu<<idxsl[i];
                        datasets[idxsl[i]].append(data.value(i, NAN));
                    }
                    for (int i=0; i<slu.size(); i++) {
                        resvec<<qfstatisticsVariance(datasets[slu[i]]);
                    }
                }
                res.setDoubleVec(resvec);
            }
        }
    }
    return ;
}




void fQFRDRTableEditor_indexedColMin(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
    res.setInvalid();
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            if (n!=2) {
                p->qfmpError("indexedcolmin(column, index) needs 2 argument");
                return ;
            }
            if ((params[0].type!=qfmpDouble)||(params[1].type!=qfmpDouble)) {
                p->qfmpError("indexedcolmin(column, index) needs two integer arguments");
                return ;
            }
            int c=floor(params[0].num-1);
            int idx=floor(params[1].num-1);
            if (c>=0 && c<d->model->columnCount() && idx>=0 && idx<d->model->columnCount()) {
                QVector<double> data=d->model->getColumnDataAsNumbers(c, Qt::EditRole);
                QVector<double> resvec;
                QVariantList idxvl=d->model->getColumnData(idx, Qt::EditRole);
                QVector<double> idxdv;
                QStringList idxsl;
                bool isNumber=true;
                for (int i=0; i<idxvl.size(); i++) {
                    bool ok=false;
                    if (idxvl[i].canConvert(QVariant::Double)) {
                        double d=idxvl[i].toDouble(&ok);
                        idxdv<<d;
                        if (!ok) isNumber=false;
                    } else {
                        isNumber=false;
                    }
                    idxsl<<idxvl[i].toString();
                }
                if (isNumber) {
                    QMap<double, QList<double> > datasets;
                    QVector<double> dvu;
                    for (int i=0; i<qMin(data.size(), idxdv.size()); i++) {
                        if (!dvu.contains(idxdv[i])) dvu<<idxdv[i];
                        datasets[idxdv[i]].append(data.value(i, NAN));
                    }
                    for (int i=0; i<dvu.size(); i++) {
                        resvec<<qfstatisticsMin(datasets[dvu[i]]);
                    }
                } else {
                    QMap<QString, QList<double> > datasets;
                    QStringList slu;
                    for (int i=0; i<qMin(data.size(), idxsl.size()); i++) {
                        if (!slu.contains(idxsl[i])) slu<<idxsl[i];
                        datasets[idxsl[i]].append(data.value(i, NAN));
                    }
                    for (int i=0; i<slu.size(); i++) {
                        resvec<<qfstatisticsMin(datasets[slu[i]]);
                    }
                }
                res.setDoubleVec(resvec);
            }
        }
    }
    return ;
}



void fQFRDRTableEditor_indexedColMax(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
    res.setInvalid();
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            if (n!=2) {
                p->qfmpError("indexedcolmax(column, index) needs 2 argument");
                return ;
            }
            if ((params[0].type!=qfmpDouble)||(params[1].type!=qfmpDouble)) {
                p->qfmpError("indexedcolmax(column, index) needs two integer arguments");
                return ;
            }
            int c=floor(params[0].num-1);
            int idx=floor(params[1].num-1);
            if (c>=0 && c<d->model->columnCount() && idx>=0 && idx<d->model->columnCount()) {
                QVector<double> data=d->model->getColumnDataAsNumbers(c, Qt::EditRole);
                QVector<double> resvec;
                QVariantList idxvl=d->model->getColumnData(idx, Qt::EditRole);
                QVector<double> idxdv;
                QStringList idxsl;
                bool isNumber=true;
                for (int i=0; i<idxvl.size(); i++) {
                    bool ok=false;
                    if (idxvl[i].canConvert(QVariant::Double)) {
                        double d=idxvl[i].toDouble(&ok);
                        idxdv<<d;
                        if (!ok) isNumber=false;
                    } else {
                        isNumber=false;
                    }
                    idxsl<<idxvl[i].toString();
                }
                if (isNumber) {
                    QMap<double, QList<double> > datasets;
                    QVector<double> dvu;
                    for (int i=0; i<qMin(data.size(), idxdv.size()); i++) {
                        if (!dvu.contains(idxdv[i])) dvu<<idxdv[i];
                        datasets[idxdv[i]].append(data.value(i, NAN));
                    }
                    for (int i=0; i<dvu.size(); i++) {
                        resvec<<qfstatisticsMax(datasets[dvu[i]]);
                    }
                } else {
                    QMap<QString, QList<double> > datasets;
                    QStringList slu;
                    for (int i=0; i<qMin(data.size(), idxsl.size()); i++) {
                        if (!slu.contains(idxsl[i])) slu<<idxsl[i];
                        datasets[idxsl[i]].append(data.value(i, NAN));
                    }
                    for (int i=0; i<slu.size(); i++) {
                        resvec<<qfstatisticsMax(datasets[slu[i]]);
                    }
                }
                res.setDoubleVec(resvec);
            }
        }
    }
    return ;
}



void fQFRDRTableEditor_indexedColMedian(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
    res.setInvalid();
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            if (n!=2) {
                p->qfmpError("indexedcolmedian(column, index) needs 2 argument");
                return ;
            }
            if ((params[0].type!=qfmpDouble)||(params[1].type!=qfmpDouble)) {
                p->qfmpError("indexedcolmedian(column, index) needs two integer arguments");
                return ;
            }
            int c=floor(params[0].num-1);
            int idx=floor(params[1].num-1);
            if (c>=0 && c<d->model->columnCount() && idx>=0 && idx<d->model->columnCount()) {
                QVector<double> data=d->model->getColumnDataAsNumbers(c, Qt::EditRole);
                QVector<double> resvec;
                QVariantList idxvl=d->model->getColumnData(idx, Qt::EditRole);
                QVector<double> idxdv;
                QStringList idxsl;
                bool isNumber=true;
                for (int i=0; i<idxvl.size(); i++) {
                    bool ok=false;
                    if (idxvl[i].canConvert(QVariant::Double)) {
                        double d=idxvl[i].toDouble(&ok);
                        idxdv<<d;
                        if (!ok) isNumber=false;
                    } else {
                        isNumber=false;
                    }
                    idxsl<<idxvl[i].toString();
                }
                if (isNumber) {
                    QMap<double, QList<double> > datasets;
                    QVector<double> dvu;
                    for (int i=0; i<qMin(data.size(), idxdv.size()); i++) {
                        if (!dvu.contains(idxdv[i])) dvu<<idxdv[i];
                        datasets[idxdv[i]].append(data.value(i, NAN));
                    }
                    for (int i=0; i<dvu.size(); i++) {
                        resvec<<qfstatisticsMedian(datasets[dvu[i]]);
                    }
                } else {
                    QMap<QString, QList<double> > datasets;
                    QStringList slu;
                    for (int i=0; i<qMin(data.size(), idxsl.size()); i++) {
                        if (!slu.contains(idxsl[i])) slu<<idxsl[i];
                        datasets[idxsl[i]].append(data.value(i, NAN));
                    }
                    for (int i=0; i<slu.size(); i++) {
                        resvec<<qfstatisticsMedian(datasets[slu[i]]);
                    }
                }
                res.setDoubleVec(resvec);
            }
        }
    }
    return ;
}




void fQFRDRTableEditor_indexedColQuantile(qfmpResult& res, const qfmpResult* params, unsigned int  n, QFMathParser* p) {
    res.setInvalid();
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            if (n!=3) {
                p->qfmpError("indexedcolmedian(column, index, quantile) needs 3 argument");
                return ;
            }
            if ((params[0].type!=qfmpDouble)||(params[1].type!=qfmpDouble)||(params[2].type!=qfmpDouble)) {
                p->qfmpError("indexedcolmedian(column, index, quantile) needs three integer arguments");
                return ;
            }
            int c=floor(params[0].num-1);
            int idx=floor(params[1].num-1);
            double q=params[2].num;
            if (c>=0 && c<d->model->columnCount() && idx>=0 && idx<d->model->columnCount()) {
                QVector<double> data=d->model->getColumnDataAsNumbers(c, Qt::EditRole);
                QVector<double> resvec;
                QVariantList idxvl=d->model->getColumnData(idx, Qt::EditRole);
                QVector<double> idxdv;
                QStringList idxsl;
                bool isNumber=true;
                for (int i=0; i<idxvl.size(); i++) {
                    bool ok=false;
                    if (idxvl[i].canConvert(QVariant::Double)) {
                        double d=idxvl[i].toDouble(&ok);
                        idxdv<<d;
                        if (!ok) isNumber=false;
                    } else {
                        isNumber=false;
                    }
                    idxsl<<idxvl[i].toString();
                }
                if (isNumber) {
                    QMap<double, QList<double> > datasets;
                    QVector<double> dvu;
                    for (int i=0; i<qMin(data.size(), idxdv.size()); i++) {
                        if (!dvu.contains(idxdv[i])) dvu<<idxdv[i];
                        datasets[idxdv[i]].append(data.value(i, NAN));
                    }
                    for (int i=0; i<dvu.size(); i++) {
                        resvec<<qfstatisticsQuantile(datasets[dvu[i]], q);
                    }
                } else {
                    QMap<QString, QList<double> > datasets;
                    QStringList slu;
                    for (int i=0; i<qMin(data.size(), idxsl.size()); i++) {
                        if (!slu.contains(idxsl[i])) slu<<idxsl[i];
                        datasets[idxsl[i]].append(data.value(i, NAN));
                    }
                    for (int i=0; i<slu.size(); i++) {
                        resvec<<qfstatisticsQuantile(datasets[slu[i]], q);
                    }
                }
                res.setDoubleVec(resvec);
            }
        }
    }
    return ;
}

/*
 *m->model()->setColumnHeaderData(c, QFRDRTable::ColumnCommentRole, edt->getComment());
                     m->model()->setColumnHeaderData(c, QFRDRTable::ColumnImageWidth, edt->getImageWidth());
 **/

void fQFRDRTableEditor_colimagewidth(qfmpResult& res, const qfmpResult *params, unsigned int n, QFMathParser *p)
{
    res.setInvalid();
    res.num=NAN;
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            if (n==1) {
                if ((params[0].type!=qfmpDouble)) {
                    p->qfmpError("colimagewidth(column) needs one integer arguments");
                    return ;
                }
                int c=floor(params[0].num-1);
                if (c>=0 && c<d->model->columnCount()) {
                    QVariant v=d->model->getColumnHeaderData(c, QFRDRTable::ColumnImageWidth);
                    double w=1;
                    if (v.canConvert(QVariant::Int)) {
                        if (v.toInt()>0) w=v.toInt();
                    }
                    res.setDouble(w);
                }
            } else {
                p->qfmpError("colimagewidth(column) needs 1 argument");
            }
        }
    }
    return ;
}


void fQFRDRTableEditor_colimageheight(qfmpResult& res, const qfmpResult *params, unsigned int n, QFMathParser *p)
{
    res.setInvalid();
    res.num=NAN;
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            if (n==1) {
                if ((params[0].type!=qfmpDouble))  {
                    p->qfmpError("colimagewidth(column) needs one integer arguments");
                    return ;
                }
                int c=floor(params[0].num-1);
                if (c>=0 && c<d->model->columnCount()) {
                    QVariant v=d->model->getColumnHeaderData(c, QFRDRTable::ColumnImageWidth);
                    int w=1;
                    if (v.canConvert(QVariant::Int)) {
                        if (v.toInt()>0) w=v.toInt();
                    }
                    res.setDouble(d->model->getColumnDataAsNumbers(c, Qt::EditRole).size()/w);
                }
            } else {
                p->qfmpError("colimagewidth(column) needs 1 argument");
            }
        }
    }
    return ;
}

void fQFRDRTableEditor_collength(qfmpResult& res, const qfmpResult *params, unsigned int n, QFMathParser *p)
{
    res.setInvalid();
    res.num=NAN;
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            if (n==1) {
                if ((params[0].type!=qfmpDouble)) {
                    p->qfmpError("collength(column) needs one integer arguments");
                    return ;
                }
                int c=floor(params[0].num-1);
                if (c>=0 && c<d->model->columnCount()) {

                    res.setDouble(d->model->getColumnDataAsNumbers(c, Qt::EditRole).size());
                }
            } else {
                p->qfmpError("collength(column) needs 1 arguments");
            }
        }
    }
    return ;
}


void fQFRDRTableEditor_colsavg(qfmpResult& res, const qfmpResult *params, unsigned int n, QFMathParser *p)
{
    res.setInvalid();
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            if (n==1) {
                if (!(params[0].convertsToIntVector())) {
                    p->qfmpError("colsavg(cols) needs one integer vector arguments");
                    return ;
                }
                QVector<int> cols=params[0].asIntVector();
                int rowCount=d->model->rowCount();
                res.setDoubleVec(rowCount, 0);
                int noDataCount=0;
                for (int r=0; r<rowCount; r++) {
                    QVector<double> data;
                    data.reserve(cols.size());
                    bool nodata=true;
                    for (int c=0; c<cols.size(); c++) {
                        double dbl=d->model->cellDouble(r, cols[c]-1);
                        data.append(dbl);
                        nodata=nodata&&(!QFFloatIsOK(dbl));
                    }
                    if (nodata)  {
                        res.numVec[r]=NAN;
                        noDataCount++;
                    } else {
                        noDataCount=0;
                        res.numVec[r]=qfstatisticsAverage(data);
                    }
                }
                while (noDataCount>0) {
                    res.numVec.pop_back();
                    noDataCount--;
                }
            } else {
                p->qfmpError("colsavg(column) needs 1 argument");
            }
        }
    }
}


void fQFRDRTableEditor_colscount(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p)
{
    res.setInvalid();
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            if (n==1) {
                if (!(params[0].convertsToIntVector())) {
                    p->qfmpError("colscount(cols) needs one integer vector arguments");
                    return ;
                }
                QVector<int> cols=params[0].asIntVector();
                int rowCount=d->model->rowCount();
                res.setDoubleVec(rowCount, 0);
                int noDataCount=0;
                for (int r=0; r<rowCount; r++) {
                    QVector<double> data;
                    data.reserve(cols.size());
                    bool nodata=true;
                    for (int c=0; c<cols.size(); c++) {
                        double dbl=d->model->cellDouble(r, cols[c]-1);
                        data.append(dbl);
                        nodata=nodata&&(!QFFloatIsOK(dbl));
                    }
                    if (nodata)  {
                        res.numVec[r]=0;
                        noDataCount++;
                    } else {
                        noDataCount=0;
                        res.numVec[r]=qfstatisticsCount(data);
                    }
                }
                while (noDataCount>0) {
                    res.numVec.pop_back();
                    noDataCount--;
                }
            } else {
                p->qfmpError("colscount(column) needs 1 argument");
            }
        }
    }
}


void fQFRDRTableEditor_colssum(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p)
{
    res.setInvalid();
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            if (n==1) {
                if (!(params[0].convertsToIntVector())) {
                    p->qfmpError("colssum(cols) needs one integer vector arguments");
                    return ;
                }
                QVector<int> cols=params[0].asIntVector();
                int rowCount=d->model->rowCount();
                res.setDoubleVec(rowCount, 0);
                int noDataCount=0;
                for (int r=0; r<rowCount; r++) {
                    QVector<double> data;
                    data.reserve(cols.size());
                    bool nodata=true;
                    for (int c=0; c<cols.size(); c++) {
                        double dbl=d->model->cellDouble(r, cols[c]-1);
                        data.append(dbl);
                        nodata=nodata&&(!QFFloatIsOK(dbl));
                    }
                    if (nodata)  {
                        res.numVec[r]=NAN;
                        noDataCount++;
                    } else {
                        noDataCount=0;
                        res.numVec[r]=qfstatisticsSum(data);
                    }
                }
                while (noDataCount>0) {
                    res.numVec.pop_back();
                    noDataCount--;
                }
            } else {
                p->qfmpError("colssum(column) needs 1 argument");
            }
        }
    }

}


void fQFRDRTableEditor_colssum2(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p)
{
    res.setInvalid();
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            if (n==1) {
                if (!(params[0].convertsToIntVector())) {
                    p->qfmpError("colssum2(cols) needs one integer vector arguments");
                    return ;
                }
                QVector<int> cols=params[0].asIntVector();
                int rowCount=d->model->rowCount();
                res.setDoubleVec(rowCount, 0);
                int noDataCount=0;
                for (int r=0; r<rowCount; r++) {
                    QVector<double> data;
                    data.reserve(cols.size());
                    bool nodata=true;
                    for (int c=0; c<cols.size(); c++) {
                        double dbl=d->model->cellDouble(r, cols[c]-1);
                        data.append(dbl);
                        nodata=nodata&&(!QFFloatIsOK(dbl));
                    }
                    if (nodata)  {
                        res.numVec[r]=NAN;
                        noDataCount++;
                    } else {
                        noDataCount=0;
                        res.numVec[r]=qfstatisticsSum2(data);
                    }
                }
                while (noDataCount>0) {
                    res.numVec.pop_back();
                    noDataCount--;
                }
            } else {
                p->qfmpError("colssum2(column) needs 1 argument");
            }
        }
    }

}


void fQFRDRTableEditor_colsvar(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p)
{
    res.setInvalid();
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            if (n==1) {
                if (!(params[0].convertsToIntVector())) {
                    p->qfmpError("colsvar(cols) needs one integer vector arguments");
                    return ;
                }
                QVector<int> cols=params[0].asIntVector();
                int rowCount=d->model->rowCount();
                res.setDoubleVec(rowCount, 0);
                int noDataCount=0;
                for (int r=0; r<rowCount; r++) {
                    QVector<double> data;
                    data.reserve(cols.size());
                    bool nodata=true;
                    for (int c=0; c<cols.size(); c++) {
                        double dbl=d->model->cellDouble(r, cols[c]-1);
                        data.append(dbl);
                        nodata=nodata&&(!QFFloatIsOK(dbl));
                    }
                    if (nodata)  {
                        res.numVec[r]=NAN;
                        noDataCount++;
                    } else {
                        noDataCount=0;
                        res.numVec[r]=qfstatisticsVariance(data);
                    }
                }
                while (noDataCount>0) {
                    res.numVec.pop_back();
                    noDataCount--;
                }
            } else {
                p->qfmpError("colsvar(column) needs 1 argument");
            }
        }
    }

}


void fQFRDRTableEditor_colsstd(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p)
{
    res.setInvalid();
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            if (n==1) {
                if (!(params[0].convertsToIntVector())) {
                    p->qfmpError("colsstd(cols) needs one integer vector arguments");
                    return ;
                }
                QVector<int> cols=params[0].asIntVector();
                int rowCount=d->model->rowCount();
                res.setDoubleVec(rowCount, 0);
                int noDataCount=0;
                for (int r=0; r<rowCount; r++) {
                    QVector<double> data;
                    data.reserve(cols.size());
                    bool nodata=true;
                    for (int c=0; c<cols.size(); c++) {
                        double dbl=d->model->cellDouble(r, cols[c]-1);
                        data.append(dbl);
                        nodata=nodata&&(!QFFloatIsOK(dbl));
                    }
                    if (nodata)  {
                        res.numVec[r]=NAN;
                        noDataCount++;
                    } else {
                        noDataCount=0;
                        res.numVec[r]=qfstatisticsStd(data);
                    }
                }
                while (noDataCount>0) {
                    res.numVec.pop_back();
                    noDataCount--;
                }
            } else {
                p->qfmpError("colsstd(column) needs 1 argument");
            }
        }
    }

}


void fQFRDRTableEditor_colsmedian(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p)
{
    res.setInvalid();
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            if (n==1) {
                if (!(params[0].convertsToIntVector())) {
                    p->qfmpError("colsmedian(cols) needs one integer vector arguments");
                    return ;
                }
                QVector<int> cols=params[0].asIntVector();
                int rowCount=d->model->rowCount();
                res.setDoubleVec(rowCount, 0);
                int noDataCount=0;
                for (int r=0; r<rowCount; r++) {
                    QVector<double> data;
                    data.reserve(cols.size());
                    bool nodata=true;
                    for (int c=0; c<cols.size(); c++) {
                        double dbl=d->model->cellDouble(r, cols[c]-1);
                        data.append(dbl);
                        nodata=nodata&&(!QFFloatIsOK(dbl));
                    }
                    if (nodata)  {
                        res.numVec[r]=NAN;
                        noDataCount++;
                    } else {
                        noDataCount=0;
                        res.numVec[r]=qfstatisticsMedian(data);
                    }
                }
                while (noDataCount>0) {
                    res.numVec.pop_back();
                    noDataCount--;
                }
            } else {
                p->qfmpError("colsmedian(column) needs 1 argument");
            }
        }
    }

}




void fQFRDRTableEditor_colsmin(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p)
{
    res.setInvalid();
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            if (n==1) {
                if (!(params[0].convertsToIntVector())) {
                    p->qfmpError("colsmin(cols) needs one integer vector arguments");
                    return ;
                }
                QVector<int> cols=params[0].asIntVector();
                int rowCount=d->model->rowCount();
                res.setDoubleVec(rowCount, 0);
                int noDataCount=0;
                for (int r=0; r<rowCount; r++) {
                    QVector<double> data;
                    data.reserve(cols.size());
                    bool nodata=true;
                    for (int c=0; c<cols.size(); c++) {
                        double dbl=d->model->cellDouble(r, cols[c]-1);
                        data.append(dbl);
                        nodata=nodata&&(!QFFloatIsOK(dbl));
                    }
                    if (nodata)  {
                        res.numVec[r]=NAN;
                        noDataCount++;
                    } else {
                        noDataCount=0;
                        res.numVec[r]=qfstatisticsMin(data);
                    }
                }
                while (noDataCount>0) {
                    res.numVec.pop_back();
                    noDataCount--;
                }
            } else {
                p->qfmpError("colsmin(column) needs 1 argument");
            }
        }
    }

}


void fQFRDRTableEditor_colsmax(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p)
{
    res.setInvalid();
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            if (n==1) {
                if (!(params[0].convertsToIntVector())) {
                    p->qfmpError("colsmax(cols) needs one integer vector arguments");
                    return ;
                }
                QVector<int> cols=params[0].asIntVector();
                int rowCount=d->model->rowCount();
                res.setDoubleVec(rowCount, 0);
                int noDataCount=0;
                for (int r=0; r<rowCount; r++) {
                    QVector<double> data;
                    data.reserve(cols.size());
                    bool nodata=true;
                    for (int c=0; c<cols.size(); c++) {
                        double dbl=d->model->cellDouble(r, cols[c]-1);
                        data.append(dbl);
                        nodata=nodata&&(!QFFloatIsOK(dbl));
                    }
                    if (nodata)  {
                        res.numVec[r]=NAN;
                        noDataCount++;
                    } else {
                        noDataCount=0;
                        res.numVec[r]=qfstatisticsMax(data);
                    }
                }
                while (noDataCount>0) {
                    res.numVec.pop_back();
                    noDataCount--;
                }
            } else {
                p->qfmpError("colsmax(column) needs 1 argument");
            }
        }
    }

}

void fQFRDRTableEditor_colsquantile(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *p)
{
    res.setInvalid();
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            if (n==2) {
                if (!(params[0].convertsToIntVector())) {
                    p->qfmpError("colsquantile(cols, quantile) needs an integer vector as first argument");
                    return ;
                }
                if (!(params[1].type==qfmpDouble)) {
                    p->qfmpError("colsquantile(cols, quantile) needs a number as second argument");
                    return ;
                }
                QVector<int> cols=params[0].asIntVector();
                double q=params[1].num;
                int rowCount=d->model->rowCount();
                res.setDoubleVec(rowCount, 0);
                int noDataCount=0;
                for (int r=0; r<rowCount; r++) {
                    QVector<double> data;
                    data.reserve(cols.size());
                    bool nodata=true;
                    for (int c=0; c<cols.size(); c++) {
                        double dbl=d->model->cellDouble(r, cols[c]-1);
                        data.append(dbl);
                        nodata=nodata&&(!QFFloatIsOK(dbl));
                    }
                    if (nodata)  {
                        res.numVec[r]=NAN;
                        noDataCount++;
                    } else {
                        noDataCount=0;
                        res.numVec[r]=qfstatisticsQuantile(data, q);
                    }
                }
                while (noDataCount>0) {
                    res.numVec.pop_back();
                    noDataCount--;
                }
            } else {
                p->qfmpError("colsquantile(column, quantile) needs 2 argument");
            }
        }
    }

}
