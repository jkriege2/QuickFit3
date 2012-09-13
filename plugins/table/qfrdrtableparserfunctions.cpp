#include "qfrdrtableparserfunctions.h"




void addQFRDRTableFunctions(jkMathParser* parser, QStringList* names) {
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





jkMathParser::jkmpResult fQFRDRTableEditor_data(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p) {
    jkMathParser::jkmpResult res;
    res.num=NAN;
    jkMathParserData* d=(jkMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            res.type=jkMathParser::jkmpDouble;
            if (n!=2) p->jkmpError("data(row, column) needs 2 argument");
            if ((params[0].type!=jkMathParser::jkmpDouble)||(params[1].type!=jkMathParser::jkmpDouble)) p->jkmpError("data(row, column) needs two integer arguments");
            int r=floor(params[0].num+1);
            int c=floor(params[1].num+1);
            if (r>=0 && c>=0 && r<d->model->rowCount() && c<d->model->columnCount()) {
                QVariant da= d->model->cell(r,c);
                switch(da.type()) {
                    case QVariant::LongLong :
                    case QVariant::ULongLong :
                    case QVariant::Int :
                    case QVariant::UInt :
                    case QVariant::Double: res.num=da.toDouble(); res.type=jkMathParser::jkmpDouble; break;
                    case QVariant::Date:
                    case QVariant::DateTime: res.num=da.toDateTime().toMSecsSinceEpoch(); res.type=jkMathParser::jkmpDouble; break;
                    case QVariant::Time: res.num=QDateTime(QDate::currentDate(), da.toTime()).toMSecsSinceEpoch(); res.type=jkMathParser::jkmpDouble; break;
                    case QVariant::Bool: res.boolean=da.toBool(); res.type=jkMathParser::jkmpBool; break;
                    case QVariant::String:
                        res.str=da.toString().toStdString(); res.type=jkMathParser::jkmpString; break;
                    default:
                        if (da.canConvert(QVariant::Double)) {
                            res.num=da.toDouble(); res.type=jkMathParser::jkmpDouble; break;
                        } else {
                            res.str=da.toString().toStdString(); res.type=jkMathParser::jkmpString; break;
                        }
                        break;
                }
            }
        }
    }
    return res;
}

jkMathParser::jkmpResult fQFRDRTableEditor_dataleft(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p) {
    jkMathParser::jkmpResult res;
    res.num=NAN;
    jkMathParserData* d=(jkMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            res.type=jkMathParser::jkmpDouble;
            if (n!=1) p->jkmpError("dataleft(delta_column) needs 1 argument");
            if ((params[0].type!=jkMathParser::jkmpDouble)) p->jkmpError("dataleft(delta_column) needs one integer arguments");
            int delta=floor(params[0].num);
            QVariant da= d->model->cell(d->row,d->column-delta);
            switch(da.type()) {
                case QVariant::LongLong :
                case QVariant::ULongLong :
                case QVariant::Int :
                case QVariant::UInt :
                case QVariant::Double: res.num=da.toDouble(); res.type=jkMathParser::jkmpDouble; break;
                case QVariant::Date:
                case QVariant::DateTime: res.num=da.toDateTime().toMSecsSinceEpoch(); res.type=jkMathParser::jkmpDouble; break;
                case QVariant::Time: res.num=QDateTime(QDate::currentDate(), da.toTime()).toMSecsSinceEpoch(); res.type=jkMathParser::jkmpDouble; break;
                case QVariant::Bool: res.boolean=da.toBool(); res.type=jkMathParser::jkmpBool; break;
                case QVariant::String:
                    res.str=da.toString().toStdString(); res.type=jkMathParser::jkmpString; break;
                default:
                    if (da.canConvert(QVariant::Double)) {
                        res.num=da.toDouble(); res.type=jkMathParser::jkmpDouble; break;
                    } else {
                        res.str=da.toString().toStdString(); res.type=jkMathParser::jkmpString; break;
                    }
                    break;
            }
        }
    }
    return res;
}


jkMathParser::jkmpResult fQFRDRTableEditor_colavg(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p) {
    jkMathParser::jkmpResult res;
    res.num=NAN;
    jkMathParserData* d=(jkMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            res.type=jkMathParser::jkmpDouble;
            QList<double> data;
            if (n==1) {
                if ((params[0].type!=jkMathParser::jkmpDouble)) p->jkmpError("colavg(column) needs one integer argument");
                for (int i=0; i<d->model->rowCount(); i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else if (n==3) {
                if ((params[0].type!=jkMathParser::jkmpDouble)||(params[1].type!=jkMathParser::jkmpDouble)||(params[2].type!=jkMathParser::jkmpDouble)) p->jkmpError("colavg(column, range_start, range_end) needs three integer argument");
                for (int i=params[1].num-1; i<=params[2].num-1; i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else {
                p->jkmpError("colavg(column [, range_start, range_end]) needs 1 or 3 arguments argument");
            }


            res.num=qfstatisticsAverage(data);
            res.type=jkMathParser::jkmpDouble;

        }
    }
    return res;
}


jkMathParser::jkmpResult fQFRDRTableEditor_colsum(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p) {
    jkMathParser::jkmpResult res;
    res.num=NAN;
    jkMathParserData* d=(jkMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            res.type=jkMathParser::jkmpDouble;
            QList<double> data;
            if (n==1) {
                if ((params[0].type!=jkMathParser::jkmpDouble)) p->jkmpError("colsum(column) needs one integer argument");
                for (int i=0; i<d->model->rowCount(); i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else if (n==3) {
                if ((params[0].type!=jkMathParser::jkmpDouble)||(params[1].type!=jkMathParser::jkmpDouble)||(params[2].type!=jkMathParser::jkmpDouble)) p->jkmpError("colsum(column, range_start, range_end) needs three integer argument");
                for (int i=params[1].num-1; i<=params[2].num-1; i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else {
                p->jkmpError("colsum(column [, range_start, range_end]) needs 1 or 3 arguments argument");
            }


            res.num=qfstatisticsSum(data);
            res.type=jkMathParser::jkmpDouble;

        }
    }
    return res;
}

jkMathParser::jkmpResult fQFRDRTableEditor_colsum2(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p) {
    jkMathParser::jkmpResult res;
    res.num=NAN;
    jkMathParserData* d=(jkMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            res.type=jkMathParser::jkmpDouble;
            QList<double> data;
            if (n==1) {
                if ((params[0].type!=jkMathParser::jkmpDouble)) p->jkmpError("colsum2(column) needs one integer argument");
                for (int i=0; i<d->model->rowCount(); i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else if (n==3) {
                if ((params[0].type!=jkMathParser::jkmpDouble)||(params[1].type!=jkMathParser::jkmpDouble)||(params[2].type!=jkMathParser::jkmpDouble)) p->jkmpError("colsum2(column, range_start, range_end) needs three integer argument");
                for (int i=params[1].num-1; i<=params[2].num-1; i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else {
                p->jkmpError("colsum2(column [, range_start, range_end]) needs 1 or 3 arguments argument");
            }


            res.num=qfstatisticsSum2(data);
            res.type=jkMathParser::jkmpDouble;

        }
    }
    return res;
}



jkMathParser::jkmpResult fQFRDRTableEditor_colvar(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p) {
    jkMathParser::jkmpResult res;
    res.num=NAN;
    jkMathParserData* d=(jkMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            res.type=jkMathParser::jkmpDouble;
            QList<double> data;
            if (n==1) {
                if ((params[0].type!=jkMathParser::jkmpDouble)) p->jkmpError("colvar(column) needs one integer argument");
                for (int i=0; i<d->model->rowCount(); i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else if (n==3) {
                if ((params[0].type!=jkMathParser::jkmpDouble)||(params[1].type!=jkMathParser::jkmpDouble)||(params[2].type!=jkMathParser::jkmpDouble)) p->jkmpError("colvar(column, range_start, range_end) needs three integer argument");
                for (int i=params[1].num-1; i<=params[2].num-1; i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else {
                p->jkmpError("colvar(column [, range_start, range_end]) needs 1 or 3 arguments argument");
            }


            res.num=qfstatisticsVariance(data);
            res.type=jkMathParser::jkmpDouble;

        }
    }
    return res;
}


jkMathParser::jkmpResult fQFRDRTableEditor_colstd(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p) {
    jkMathParser::jkmpResult res;
    res.num=NAN;
    jkMathParserData* d=(jkMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            res.type=jkMathParser::jkmpDouble;
            QList<double> data;
            if (n==1) {
                if ((params[0].type!=jkMathParser::jkmpDouble)) p->jkmpError("colstd(column) needs one integer argument");
                for (int i=0; i<d->model->rowCount(); i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else if (n==3) {
                if ((params[0].type!=jkMathParser::jkmpDouble)||(params[1].type!=jkMathParser::jkmpDouble)||(params[2].type!=jkMathParser::jkmpDouble)) p->jkmpError("colstd(column, range_start, range_end) needs three integer argument");
                for (int i=params[1].num-1; i<=params[2].num-1; i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else {
                p->jkmpError("colstd(column [, range_start, range_end]) needs 1 or 3 arguments argument");
            }


            res.num=sqrt(qfstatisticsVariance(data));
            res.type=jkMathParser::jkmpDouble;

        }
    }
    return res;
}


jkMathParser::jkmpResult fQFRDRTableEditor_colmin(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p) {
    jkMathParser::jkmpResult res;
    res.num=NAN;
    jkMathParserData* d=(jkMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            res.type=jkMathParser::jkmpDouble;
            QList<double> data;
            if (n==1) {
                if ((params[0].type!=jkMathParser::jkmpDouble)) p->jkmpError("colmin(column) needs one integer argument");
                for (int i=0; i<d->model->rowCount(); i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else if (n==3) {
                if ((params[0].type!=jkMathParser::jkmpDouble)||(params[1].type!=jkMathParser::jkmpDouble)||(params[2].type!=jkMathParser::jkmpDouble)) p->jkmpError("colmin(column, range_start, range_end) needs three integer argument");
                for (int i=params[1].num-1; i<=params[2].num-1; i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else {
                p->jkmpError("colmin(column [, range_start, range_end]) needs 1 or 3 arguments argument");
            }

            qSort(data);
            res.num=qfstatisticsSortedMin(data);
            res.type=jkMathParser::jkmpDouble;

        }
    }
    return res;
}


jkMathParser::jkmpResult fQFRDRTableEditor_colmax(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p) {
    jkMathParser::jkmpResult res;
    res.num=NAN;
    jkMathParserData* d=(jkMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            res.type=jkMathParser::jkmpDouble;
            QList<double> data;
            if (n==1) {
                if ((params[0].type!=jkMathParser::jkmpDouble)) p->jkmpError("colmax(column) needs one integer argument");
                for (int i=0; i<d->model->rowCount(); i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else if (n==3) {
                if ((params[0].type!=jkMathParser::jkmpDouble)||(params[1].type!=jkMathParser::jkmpDouble)||(params[2].type!=jkMathParser::jkmpDouble)) p->jkmpError("colmax(column, range_start, range_end) needs three integer argument");
                for (int i=params[1].num-1; i<=params[2].num-1; i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else {
                p->jkmpError("colmax(column [, range_start, range_end]) needs 1 or 3 arguments argument");
            }

            qSort(data);
            res.num=qfstatisticsSortedMax(data);
            res.type=jkMathParser::jkmpDouble;

        }
    }
    return res;
}


jkMathParser::jkmpResult fQFRDRTableEditor_colmedian(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p) {
    jkMathParser::jkmpResult res;
    res.num=NAN;
    jkMathParserData* d=(jkMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            res.type=jkMathParser::jkmpDouble;
            QList<double> data;
            if (n==1) {
                if ((params[0].type!=jkMathParser::jkmpDouble)) p->jkmpError("colmedian(column) needs one integer argument");
                for (int i=0; i<d->model->rowCount(); i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else if (n==3) {
                if ((params[0].type!=jkMathParser::jkmpDouble)||(params[1].type!=jkMathParser::jkmpDouble)||(params[2].type!=jkMathParser::jkmpDouble)) p->jkmpError("colmedian(column, range_start, range_end) needs three integer argument");
                for (int i=params[1].num-1; i<=params[2].num-1; i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
            } else {
                p->jkmpError("colmedian(column [, range_start, range_end]) needs 1 or 3 arguments argument");
            }

            qSort(data);
            res.num=qfstatisticsSortedMedian(data);
            res.type=jkMathParser::jkmpDouble;

        }
    }
    return res;
}



jkMathParser::jkmpResult fQFRDRTableEditor_colquantile(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p) {
    jkMathParser::jkmpResult res;
    res.num=NAN;
    jkMathParserData* d=(jkMathParserData*)p->get_data();
    if (d) {
        if (d->model) {
            res.type=jkMathParser::jkmpDouble;
            QList<double> data;
            double quantile=0;
            if (n==2) {
                if ((params[0].type!=jkMathParser::jkmpDouble)||(params[1].type!=jkMathParser::jkmpDouble)) p->jkmpError("colquantile(column, quantile) needs two number argument");
                for (int i=0; i<d->model->rowCount(); i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
                quantile=params[1].num;
            } else if (n==4) {
                if ((params[0].type!=jkMathParser::jkmpDouble)||(params[1].type!=jkMathParser::jkmpDouble)||(params[2].type!=jkMathParser::jkmpDouble)||(params[3].type!=jkMathParser::jkmpDouble)) p->jkmpError("colquantile(column, range_start, range_end, quantile) needs four number argument");
                for (int i=params[1].num-1; i<=params[2].num-1; i++) {
                    QVariant v=d->model->cell(i, params[0].num-1);
                    if (v.canConvert(QVariant::Double) && (v.type()!=QVariant::String)) {
                        data.append(v.toDouble());
                    }
                }
                quantile=params[3].num;
            } else {
                p->jkmpError("colquantile(column [, range_start, range_end], quantile) needs 2 or 4 arguments ");
            }

            qSort(data);
            res.num=qfstatisticsSortedQuantile(data, quantile);
            res.type=jkMathParser::jkmpDouble;

        }
    }
    return res;
}




