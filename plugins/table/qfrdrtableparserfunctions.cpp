#include "qfrdrtableparserfunctions.h"




void addQFRDRTableFunctions(QFMathParser* parser, QStringList* names) {
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

    if (names) {
        *names<<"data"<<"dataleft"<<"colavg"<<"colsum"<<"solsum2"<<"colvar"<<"colstad"<<"colmin"<<"colmax"<<"colmedian"<<"colquantile";
    }
}





QFMathParser::qfmpResult fQFRDRTableEditor_data(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p) {
    QFMathParser::qfmpResult res;
    res.num=NAN;
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            res.type=QFMathParser::qfmpDouble;
            if (n!=2) p->qfmpError("data(row, column) needs 2 argument");
            if ((params[0].type!=QFMathParser::qfmpDouble)||(params[1].type!=QFMathParser::qfmpDouble)) p->qfmpError("data(row, column) needs two integer arguments");
            int r=floor(params[0].num-1);
            int c=floor(params[1].num-1);
            if (r>=0 && c>=0 && r<d->model->rowCount() && c<d->model->columnCount()) {
                QVariant da= d->model->cell(r,c);
                switch(da.type()) {
                    case QVariant::LongLong :
                    case QVariant::ULongLong :
                    case QVariant::Int :
                    case QVariant::UInt :
                    case QVariant::Double: res.num=da.toDouble(); res.type=QFMathParser::qfmpDouble; break;
                    case QVariant::Date:
                    case QVariant::DateTime: res.num=da.toDateTime().toMSecsSinceEpoch(); res.type=QFMathParser::qfmpDouble; break;
                    case QVariant::Time: res.num=QDateTime(QDate::currentDate(), da.toTime()).toMSecsSinceEpoch(); res.type=QFMathParser::qfmpDouble; break;
                    case QVariant::Bool: res.boolean=da.toBool(); res.type=QFMathParser::qfmpBool; break;
                    case QVariant::String:
                        res.str=da.toString(); res.type=QFMathParser::qfmpString; break;
                    default:
                        if (da.canConvert(QVariant::Double)) {
                            res.num=da.toDouble(); res.type=QFMathParser::qfmpDouble; break;
                        } else {
                            res.str=da.toString(); res.type=QFMathParser::qfmpString; break;
                        }
                        break;
                }
            }
        }
    }
    return res;
}

QFMathParser::qfmpResult fQFRDRTableEditor_dataleft(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p) {
    QFMathParser::qfmpResult res;
    res.num=NAN;
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            res.type=QFMathParser::qfmpDouble;
            if (n!=1) p->qfmpError("dataleft(delta_column) needs 1 argument");
            if ((params[0].type!=QFMathParser::qfmpDouble)) p->qfmpError("dataleft(delta_column) needs one integer arguments");
            int delta=floor(params[0].num);
            QVariant da= d->model->cell(d->row,d->column-delta);
            switch(da.type()) {
                case QVariant::LongLong :
                case QVariant::ULongLong :
                case QVariant::Int :
                case QVariant::UInt :
                case QVariant::Double: res.num=da.toDouble(); res.type=QFMathParser::qfmpDouble; break;
                case QVariant::Date:
                case QVariant::DateTime: res.num=da.toDateTime().toMSecsSinceEpoch(); res.type=QFMathParser::qfmpDouble; break;
                case QVariant::Time: res.num=QDateTime(QDate::currentDate(), da.toTime()).toMSecsSinceEpoch(); res.type=QFMathParser::qfmpDouble; break;
                case QVariant::Bool: res.boolean=da.toBool(); res.type=QFMathParser::qfmpBool; break;
                case QVariant::String:
                    res.str=da.toString(); res.type=QFMathParser::qfmpString; break;
                default:
                    if (da.canConvert(QVariant::Double)) {
                        res.num=da.toDouble(); res.type=QFMathParser::qfmpDouble; break;
                    } else {
                        res.str=da.toString(); res.type=QFMathParser::qfmpString; break;
                    }
                    break;
            }
        }
    }
    return res;
}


QFMathParser::qfmpResult fQFRDRTableEditor_colavg(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p) {
    QFMathParser::qfmpResult res;
    res.num=NAN;
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            res.type=QFMathParser::qfmpDouble;
            QList<double> data;
            if (n==1) {
                if ((params[0].type!=QFMathParser::qfmpDouble)) p->qfmpError("colavg(column) needs one integer argument");
                for (int i=0; i<d->model->rowCount(); i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else if (n==3) {
                if ((params[0].type!=QFMathParser::qfmpDouble)||(params[1].type!=QFMathParser::qfmpDouble)||(params[2].type!=QFMathParser::qfmpDouble)) p->qfmpError("colavg(column, range_start, range_end) needs three integer argument");
                for (int i=params[1].num-1; i<=params[2].num-1; i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else {
                p->qfmpError("colavg(column [, range_start, range_end]) needs 1 or 3 arguments argument");
            }


            res.num=qfstatisticsAverage(data);
            res.type=QFMathParser::qfmpDouble;

        }
    }
    return res;
}


QFMathParser::qfmpResult fQFRDRTableEditor_colsum(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p) {
    QFMathParser::qfmpResult res;
    res.num=NAN;
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            res.type=QFMathParser::qfmpDouble;
            QList<double> data;
            if (n==1) {
                if ((params[0].type!=QFMathParser::qfmpDouble)) p->qfmpError("colsum(column) needs one integer argument");
                for (int i=0; i<d->model->rowCount(); i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else if (n==3) {
                if ((params[0].type!=QFMathParser::qfmpDouble)||(params[1].type!=QFMathParser::qfmpDouble)||(params[2].type!=QFMathParser::qfmpDouble)) p->qfmpError("colsum(column, range_start, range_end) needs three integer argument");
                for (int i=params[1].num-1; i<=params[2].num-1; i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else {
                p->qfmpError("colsum(column [, range_start, range_end]) needs 1 or 3 arguments argument");
            }


            res.num=qfstatisticsSum(data);
            res.type=QFMathParser::qfmpDouble;

        }
    }
    return res;
}

QFMathParser::qfmpResult fQFRDRTableEditor_colsum2(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p) {
    QFMathParser::qfmpResult res;
    res.num=NAN;
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            res.type=QFMathParser::qfmpDouble;
            QList<double> data;
            if (n==1) {
                if ((params[0].type!=QFMathParser::qfmpDouble)) p->qfmpError("colsum2(column) needs one integer argument");
                for (int i=0; i<d->model->rowCount(); i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else if (n==3) {
                if ((params[0].type!=QFMathParser::qfmpDouble)||(params[1].type!=QFMathParser::qfmpDouble)||(params[2].type!=QFMathParser::qfmpDouble)) p->qfmpError("colsum2(column, range_start, range_end) needs three integer argument");
                for (int i=params[1].num-1; i<=params[2].num-1; i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else {
                p->qfmpError("colsum2(column [, range_start, range_end]) needs 1 or 3 arguments argument");
            }


            res.num=qfstatisticsSum2(data);
            res.type=QFMathParser::qfmpDouble;

        }
    }
    return res;
}



QFMathParser::qfmpResult fQFRDRTableEditor_colvar(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p) {
    QFMathParser::qfmpResult res;
    res.num=NAN;
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            res.type=QFMathParser::qfmpDouble;
            QList<double> data;
            if (n==1) {
                if ((params[0].type!=QFMathParser::qfmpDouble)) p->qfmpError("colvar(column) needs one integer argument");
                for (int i=0; i<d->model->rowCount(); i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else if (n==3) {
                if ((params[0].type!=QFMathParser::qfmpDouble)||(params[1].type!=QFMathParser::qfmpDouble)||(params[2].type!=QFMathParser::qfmpDouble)) p->qfmpError("colvar(column, range_start, range_end) needs three integer argument");
                for (int i=params[1].num-1; i<=params[2].num-1; i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else {
                p->qfmpError("colvar(column [, range_start, range_end]) needs 1 or 3 arguments argument");
            }


            res.num=qfstatisticsVariance(data);
            res.type=QFMathParser::qfmpDouble;

        }
    }
    return res;
}


QFMathParser::qfmpResult fQFRDRTableEditor_colstd(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p) {
    QFMathParser::qfmpResult res;
    res.num=NAN;
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            res.type=QFMathParser::qfmpDouble;
            QList<double> data;
            if (n==1) {
                if ((params[0].type!=QFMathParser::qfmpDouble)) p->qfmpError("colstd(column) needs one integer argument");
                for (int i=0; i<d->model->rowCount(); i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else if (n==3) {
                if ((params[0].type!=QFMathParser::qfmpDouble)||(params[1].type!=QFMathParser::qfmpDouble)||(params[2].type!=QFMathParser::qfmpDouble)) p->qfmpError("colstd(column, range_start, range_end) needs three integer argument");
                for (int i=params[1].num-1; i<=params[2].num-1; i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else {
                p->qfmpError("colstd(column [, range_start, range_end]) needs 1 or 3 arguments argument");
            }


            res.num=sqrt(qfstatisticsVariance(data));
            res.type=QFMathParser::qfmpDouble;

        }
    }
    return res;
}


QFMathParser::qfmpResult fQFRDRTableEditor_colmin(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p) {
    QFMathParser::qfmpResult res;
    res.num=NAN;
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            res.type=QFMathParser::qfmpDouble;
            QList<double> data;
            if (n==1) {
                if ((params[0].type!=QFMathParser::qfmpDouble)) p->qfmpError("colmin(column) needs one integer argument");
                for (int i=0; i<d->model->rowCount(); i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else if (n==3) {
                if ((params[0].type!=QFMathParser::qfmpDouble)||(params[1].type!=QFMathParser::qfmpDouble)||(params[2].type!=QFMathParser::qfmpDouble)) p->qfmpError("colmin(column, range_start, range_end) needs three integer argument");
                for (int i=params[1].num-1; i<=params[2].num-1; i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else {
                p->qfmpError("colmin(column [, range_start, range_end]) needs 1 or 3 arguments argument");
            }

            qSort(data);
            res.num=qfstatisticsSortedMin(data);
            res.type=QFMathParser::qfmpDouble;

        }
    }
    return res;
}


QFMathParser::qfmpResult fQFRDRTableEditor_colmax(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p) {
    QFMathParser::qfmpResult res;
    res.num=NAN;
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            res.type=QFMathParser::qfmpDouble;
            QList<double> data;
            if (n==1) {
                if ((params[0].type!=QFMathParser::qfmpDouble)) p->qfmpError("colmax(column) needs one integer argument");
                for (int i=0; i<d->model->rowCount(); i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else if (n==3) {
                if ((params[0].type!=QFMathParser::qfmpDouble)||(params[1].type!=QFMathParser::qfmpDouble)||(params[2].type!=QFMathParser::qfmpDouble)) p->qfmpError("colmax(column, range_start, range_end) needs three integer argument");
                for (int i=params[1].num-1; i<=params[2].num-1; i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else {
                p->qfmpError("colmax(column [, range_start, range_end]) needs 1 or 3 arguments argument");
            }

            qSort(data);
            res.num=qfstatisticsSortedMax(data);
            res.type=QFMathParser::qfmpDouble;

        }
    }
    return res;
}


QFMathParser::qfmpResult fQFRDRTableEditor_colmedian(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p) {
    QFMathParser::qfmpResult res;
    res.num=NAN;
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            res.type=QFMathParser::qfmpDouble;
            QList<double> data;
            if (n==1) {
                if ((params[0].type!=QFMathParser::qfmpDouble)) p->qfmpError("colmedian(column) needs one integer argument");
                for (int i=0; i<d->model->rowCount(); i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else if (n==3) {
                if ((params[0].type!=QFMathParser::qfmpDouble)||(params[1].type!=QFMathParser::qfmpDouble)||(params[2].type!=QFMathParser::qfmpDouble)) p->qfmpError("colmedian(column, range_start, range_end) needs three integer argument");
                for (int i=params[1].num-1; i<=params[2].num-1; i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else {
                p->qfmpError("colmedian(column [, range_start, range_end]) needs 1 or 3 arguments argument");
            }

            qSort(data);
            res.num=qfstatisticsSortedMedian(data);
            res.type=QFMathParser::qfmpDouble;

        }
    }
    return res;
}



QFMathParser::qfmpResult fQFRDRTableEditor_colquantile(const QFMathParser::qfmpResult* params, unsigned int  n, QFMathParser* p) {
    QFMathParser::qfmpResult res;
    res.num=NAN;
    QFMathParserData* d=(QFMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            res.type=QFMathParser::qfmpDouble;
            QList<double> data;
            double quantile=0;
            if (n==2) {
                if ((params[0].type!=QFMathParser::qfmpDouble)||(params[1].type!=QFMathParser::qfmpDouble)) p->qfmpError("colquantile(column, quantile) needs two number argument");
                for (int i=0; i<d->model->rowCount(); i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
                quantile=params[1].num;
            } else if (n==4) {
                if ((params[0].type!=QFMathParser::qfmpDouble)||(params[1].type!=QFMathParser::qfmpDouble)||(params[2].type!=QFMathParser::qfmpDouble)||(params[3].type!=QFMathParser::qfmpDouble)) p->qfmpError("colquantile(column, range_start, range_end, quantile) needs four number argument");
                for (int i=params[1].num-1; i<=params[2].num-1; i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
                quantile=params[3].num;
            } else {
                p->qfmpError("colquantile(column [, range_start, range_end], quantile) needs 2 or 4 arguments ");
            }

            qSort(data);
            res.num=qfstatisticsSortedQuantile(data, quantile);
            res.type=QFMathParser::qfmpDouble;

        }
    }
    return res;
}




