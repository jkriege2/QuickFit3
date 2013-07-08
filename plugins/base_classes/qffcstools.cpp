#include "qffcstools.h"
#include "qfrdrsimplecountrateinterface.h"
#include "qfrdrcountratesinterface.h"

bool qfFCSHasSpecial(const QFRawDataRecord *r, int index, const QString &paramid, double &value, double &error)  {
    //qDebug()<<"qfFCSHasSpecial: "<<paramid;
    if (paramid=="count_rate") {
        QFRDRCountRatesInterface* crintf=qobject_cast<QFRDRCountRatesInterface*>(r);
        value=0;
        error=0;
        if (crintf) {
            error=crintf->getRateStdDev(index)*1000.0;
            value=crintf->getRateMean(index)*1000.0;
        }
        QFRDRSimpleCountRatesInterface* scrintf=qobject_cast<QFRDRSimpleCountRatesInterface*>(r);
        if (scrintf && value==0) {
            value=scrintf->getSimpleCountrateAverage(index, true)*1000.0;
            error=scrintf->getSimpleCountrateStdDev(index, true)*1000.0;
        }
        if (crintf||scrintf) return true;
    } else if (paramid=="count_rate1") {
        QFRDRCountRatesInterface* crintf=qobject_cast<QFRDRCountRatesInterface*>(r);
        value=0;
        error=0;
        //qDebug()<<paramid<<crintf;
        if (crintf && crintf->getRateChannels()>0) {
            int c=0;
            if (crintf->getRateChannelsSwapped()) {
                if (c==0) c=1;
                else if (c==1) c=0;
            }
            error=crintf->getRateStdDev(index, c)*1000.0;
            value=crintf->getRateMean(index, c)*1000.0;
            //qDebug()<<"getRateMean(run="<<index<<", ch=0) = "<<value<<" +/- "<<error;
            return true;
        }
        QFRDRSimpleCountRatesInterface* scrintf=qobject_cast<QFRDRSimpleCountRatesInterface*>(r);
        //qDebug()<<paramid<<scrintf;
        if (scrintf && value==0) {
            value=scrintf->getSimpleCountrateAverage(index,0, false)*1000.0;
            error=scrintf->getSimpleCountrateStdDev(index,0, false)*1000.0;
            return true;
        }
    } else if (paramid=="count_rate2") {
        QFRDRCountRatesInterface* crintf=qobject_cast<QFRDRCountRatesInterface*>(r);
        value=0;
        error=0;
        if (crintf && crintf->getRateChannels()>1) {
            int c=1;
            if (crintf->getRateChannelsSwapped()) {
                if (c==0) c=1;
                else if (c==1) c=0;
            }
            error=crintf->getRateStdDev(index, c)*1000.0;
            value=crintf->getRateMean(index, c)*1000.0;
            //qDebug()<<"getRateMean(run="<<index<<", ch=1) = "<<value<<" +/- "<<error;
            return true;
        }
        QFRDRSimpleCountRatesInterface* scrintf=qobject_cast<QFRDRSimpleCountRatesInterface*>(r);
        if (scrintf && value==0 && scrintf->getSimpleCountrateChannels()>1) {
            value=scrintf->getSimpleCountrateAverage(index,1, false)*1000.0;
            error=scrintf->getSimpleCountrateStdDev(index,1, false)*1000.0;
            return true;
        }
    } else if (paramid=="pixel_width") {
        if (!r) return false;
        double bin=r->getProperty("BINNING", 1.0).toDouble();
        double width=r->getProperty("PIXEL_WIDTH", -1).toDouble();
        if (width<=0) return false;
        value=bin*width;
        error=0;
        return true;
    } else if (paramid=="pixel_height") {
        if (!r) return false;
        double bin=r->getProperty("BINNING", 1.0).toDouble();
        double height=r->getProperty("PIXEL_HEIGHT", -1).toDouble();
        if (height<=0) return false;
        value=bin*height;
        error=0;
        return true;
    } else if (paramid=="focus_distance") {
        if (!r) return false;
        double deltax=r->getProperty("DCCF_DELTAX", 0.0).toDouble();
        double deltay=r->getProperty("DCCF_DELTAY", 0.0).toDouble();
        double bin=r->getProperty("BINNING", 1.0).toDouble();
        double width=r->getProperty("PIXEL_WIDTH", -1).toDouble();
        double height=r->getProperty("PIXEL_HEIGHT", -1).toDouble();
        if (r->getRole().toLower()=="acf") {
            value=0;
            error=0;
            return true;
        } else {
            if (width<=0 || height<=0 || !(r->propertyExists("DCCF_DELTAX") && r->propertyExists("DCCF_DELTAY"))) return false;
            value=sqrt(qfSqr(bin*width*deltax)+qfSqr(bin*height*deltay));
            error=0;
            return true;
        }
    } else if (paramid=="focus_distancex"||paramid=="focus_distance_x") {
        if (!r) return false;
        double deltax=r->getProperty("DCCF_DELTAX", 0.0).toDouble();
        double bin=r->getProperty("BINNING", 1.0).toDouble();
        double width=r->getProperty("PIXEL_WIDTH", -1).toDouble();
        if (r->getRole().toLower()=="acf") {
            value=0;
            error=0;
            return true;
        } else {
            if (width<=0 || !(r->propertyExists("DCCF_DELTAX"))) return false;
            value=bin*width*deltax;
            //qDebug()<<deltax<<bin<<width;
            error=0;
            return true;
        }
    } else if (paramid=="focus_distancey"||paramid=="focus_distance_y") {
        if (!r) return false;
        double deltay=r->getProperty("DCCF_DELTAY", 0.0).toDouble();
        double bin=r->getProperty("BINNING", 1.0).toDouble();
        double height=r->getProperty("PIXEL_HEIGHT", -1).toDouble();
        if (r->getRole().toLower()=="acf") {
            value=0;
            error=0;
            return true;
        } else {
            if (height<=0 || !(r->propertyExists("DCCF_DELTAY"))) return false;
            //qDebug()<<deltay<<bin<<height;
            value=bin*height*deltay;
            error=0;
            return true;
        }
    } else if (paramid=="dls_angle") {
        if (!r) return false;
        if (!(r->propertyExists("ANGLE [°]") || r->propertyExists("ANGLE"))) return false;
        double angle_deg=r->getProperty("ANGLE [°]", r->getProperty("ANGLE", 90.0).toDouble()).toDouble();
        value=angle_deg;
        error=0;
        return true;
    } else if (paramid=="wavelength") {
        if (!r) return false;
        if (!(r->propertyExists("WAVELENGTH [nm]") || r->propertyExists("WAVELENGTH"))) return false;
        double angle_deg=r->getProperty("WAVELENGTH [nm]", r->getProperty("WAVELENGTH", 532.0).toDouble()).toDouble();
        value=angle_deg;
        error=0;
        return true;
    } else if (paramid=="refractive_index") {
        if (!r) return false;
        if (!(r->propertyExists("REFRACTIVE_INDEX"))) return false;
        double angle_deg=r->getProperty("REFRACTIVE_INDEX", 1.33).toDouble();
        value=angle_deg;
        error=0;
        return true;
    } else if (paramid=="viscosity") {
        if (!r) return false;
        if (!(r->propertyExists("VISCOSITY [cp]") || r->propertyExists("VISCOSITY"))) return false;
        double angle_deg=r->getProperty("VISCOSITY [cp]", r->getProperty("VISCOSITY", 0.86).toDouble()).toDouble();
        value=angle_deg;
        error=0;
        return true;
    } else if (paramid=="temperature") {
        if (!r) return false;
        if (!(r->propertyExists("TEMPERATURE [K]") || r->propertyExists("TEMPERATURE"))) return false;
        double angle_deg=r->getProperty("TEMPERATURE [K]", r->getProperty("TEMPERATURE", 396).toDouble()).toDouble();
        value=angle_deg;
        error=0;
        return true;
    }
    QString param=paramid;
    if (param.contains("hieght")) {
        return qfFCSHasSpecial(r, index, param.replace("hieght", "height", Qt::CaseInsensitive), value, error);
    }

    return false;
}










bool qfFCSOverrideFitFunctionPreset(const QFEvaluationItem* eval, const QFRawDataRecord* r, QString paramid, double &value, bool checkPositive)  {
    QString param=paramid.toUpper();
    QString preset=QString("PRESET_")+param;
    if (eval->propertyExists(preset)) {
        double fw=eval->getProperty(preset, 0).toDouble();
        if (checkPositive && fw<0) return false;
        value=fw;
        return true;
    }
    if (param=="DIFF_COEFF1") {
        if (eval->propertyExists("PRESET_D1")) {
            double D=eval->getProperty("PRESET_D1", 0).toDouble();
            if (D<=0) return false;
            value=D;
            return true;
        }
    }
    if (param=="DIFF_COEFF2") {
        if (eval->propertyExists("PRESET_D2")) {
            double D=eval->getProperty("PRESET_D2", 0).toDouble();
            if (D<=0) return false;
            value=D;
            return true;
        }
    }
    if (param=="DIFF_COEFF3") {
        if (eval->propertyExists("PRESET_D3")) {
            double D=eval->getProperty("PRESET_D3", 0).toDouble();
            if (D<=0) return false;
            value=D;
            return true;
        }
    }
    QString p=param;
    if (p.contains("HIEGHT")) {
        return qfFCSOverrideFitFunctionPreset(eval, r, p.replace("HIEGHT", "HEIGHT", Qt::CaseInsensitive), value, checkPositive);
    }
    return false;
}

bool qfFCSOverrideFitFunctionPresetError(const QFEvaluationItem* eval, const QFRawDataRecord* r, QString paramid, double &value, bool checkPositive)  {
    QString param=paramid.toUpper();
    QString preset=QString("PRESET_")+param+QString("_ERROR");
    if (eval->propertyExists(preset)) {
        double fw=eval->getProperty(preset, 0).toDouble();
        if (checkPositive && fw<0) return false;
        value=fw;
        return true;
    }
    if (param=="DIFF_COEFF1") {
        if (eval->propertyExists("PRESET_D1_ERROR")) {
            double D=eval->getProperty("PRESET_D1_ERROR", 0).toDouble();
            if (D<=0) return false;
            value=D;
            return true;
        }
    }
    if (param=="DIFF_COEFF2") {
        if (eval->propertyExists("PRESET_D2_ERROR")) {
            double D=eval->getProperty("PRESET_D2_ERROR", 0).toDouble();
            if (D<=0) return false;
            value=D;
            return true;
        }
    }
    if (param=="DIFF_COEFF3") {
        if (eval->propertyExists("PRESET_D3_ERROR")) {
            double D=eval->getProperty("PRESET_D3_ERROR", 0).toDouble();
            if (D<=0) return false;
            value=D;
            return true;
        }
    }
    QString p=param;
    if (p.contains("HIEGHT")) {
        return qfFCSOverrideFitFunctionPresetError(eval, r, p.replace("HIEGHT", "HEIGHT", Qt::CaseInsensitive), value, checkPositive);
    }

    return false;

}

bool qfFCSOverrideFitFunctionPresetFix(const QFEvaluationItem* eval, const QFRawDataRecord* r, QString paramid, bool &value)  {
    QString param=paramid.toUpper();
    QString preset=QString("PRESET_")+param+QString("_FIX");
    if (param=="DIFF_COEFF1") {
        if (eval->propertyExists("PRESET_D1_FIX")) {
            value=eval->getProperty("PRESET_D1_FIX", false).toBool();
            return true;
        }
    }
    if (param=="DIFF_COEFF2") {
        if (eval->propertyExists("PRESET_D2_FIX")) {
            value=eval->getProperty("PRESET_D2_FIX", false).toBool();
            return true;
        }
    }
    if (param=="DIFF_COEFF3") {
        if (eval->propertyExists("PRESET_D3_FIX")) {
            value=eval->getProperty("PRESET_D3_FIX", false).toBool();
            return true;
        }
    }
    if (eval->propertyExists(preset)) {
        value=eval->getProperty(preset, false).toBool();
        return true;
    }
    QString p=param;
    if (p.contains("HIEGHT")) {
        return qfFCSOverrideFitFunctionPresetFix(eval, r, p.replace("HIEGHT", "HEIGHT", Qt::CaseInsensitive), value);
    }

    return false;
}



