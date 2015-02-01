/*
Copyright (c) 2014
	
	last modification: $LastChangedDate: 2015-01-18 16:06:14 +0100 (So, 18 Jan 2015) $  (revision $Rev: 3726 $)

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


#include "qfevalbeadscanpsf_item.h"
#include "qfevalbeadscanpsf_editor.h"
#include "qfrdrimagestackinterface.h"
#include "qfrdrimagemask.h"
#include "qftools.h"
#include "statistics_tools.h"
#include "CImg.h"
#include "qffitfunction.h"
#include "qffitfunctionnd.h"
#include "qffitalgorithm.h"
#include "fitfuncpsfgaussian.h"
#include "qffitfunctionpsf3dgaussian.h"
#include "fitfuncpsfgaussianbeamwidth.h"

QFEvalBeadScanPSFItem::QFEvalBeadScanPSFItem(QFProject* parent):
    QFEvaluationItem(parent, true, false)
{

}

QFEvalBeadScanPSFItem::~QFEvalBeadScanPSFItem() {

}


void QFEvalBeadScanPSFItem::intWriteData(QXmlStreamWriter& w) {
    /* store global evaluation parameters / initial values or anything else to the project tree here */
}

void QFEvalBeadScanPSFItem::intReadData(QDomElement* e) {
    /* read back anything you've written in intWriteData() in this function */
}


QFEvaluationEditor* QFEvalBeadScanPSFItem::createEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent) {
    return new QFEvalBeadScanPSFEditor(services, propEditor, parent);
};

bool QFEvalBeadScanPSFItem::isApplicable(QFRawDataRecord* record) const {
     return qobject_cast<QFRDRImageStackInterface*>(record);
}

bool QFEvalBeadScanPSFItem::hasEvaluation(QFRawDataRecord* r1) {
    // checks, whether any results exist for the given eval ID
    QFRawDataRecord* r=r1;
    QString rsid=getEvaluationResultID();
    return r->resultsExistsFromEvaluation(rsid);
}

QString QFEvalBeadScanPSFItem::getEvaluationResultID() {
    // in the simplest case, the evaluation ID only contains the type-name and the record ID, this way, different
	// evaluation objects may write results into the same RDR and keep them separate. You may want to extend this ID,
	// e.g. by the used fit function ...
	return QString("%1_%2").arg(getType()).arg(getID());
}


void QFEvalBeadScanPSFItem::doEvaluation(QFRawDataRecord* record, double deltaXY, double deltaZ, int ROIxy, int ROIz, int pixels_per_frame, double est_psf_width, double est_psf_height, QProgressDialog* dlgEvaluationProgress) {
    QApplication::processEvents();
    if (dlgEvaluationProgress&& dlgEvaluationProgress->wasCanceled()) return; // canceled by user ?

    QString evalID=getEvaluationResultID();
    QFRDRImageStackInterface* data=qobject_cast<QFRDRImageStackInterface*>(record);
    QFRDRImageMaskInterface* maskI=qobject_cast<QFRDRImageMaskInterface*>(record);
    int stack=0;
    int channels=qMin(2,data->getImageStackChannels(stack));
    int size_z=data->getImageStackFrames(stack);
    int width=data->getImageStackWidth(stack);
    int height=data->getImageStackHeight(stack);
    int zsteps=qMax(4, ROIz/4);
    double min_distance=double(ROIxy)/3.0;
    QString fitalgID="fit_lmfit";
    QString fitalgIRLSID="fit_irls_lmfit";
    QFFitAlgorithm* alg=QFPluginServices::getInstance()->getFitAlgorithmManager()->createAlgorithm(fitalgID);
    if (!alg) return;
    QFFitAlgorithm* algIRLS=QFPluginServices::getInstance()->getFitAlgorithmManager()->createAlgorithm(fitalgIRLSID);
    if (!algIRLS) {
        algIRLS=QFPluginServices::getInstance()->getFitAlgorithmManager()->createAlgorithm(fitalgID);
    }
    if (!algIRLS) return;

    // fit function object for 1D Gaussian fits:
    QFFitFunction* ff1D=new QFFitFunctionPSFGaussian();
    if (!ff1D) return;
    QFFitFunction* ffGBwidth=new QFFitFunctionPSFGaussianBeamWidth();
    if (!ffGBwidth) return;
    QVector<double> ff1Dmin=ff1D->getInitialParamMins();
    QVector<double> ff1Dmax=ff1D->getInitialParamMaxs();
    QVector<double> ffGBwidthmin=ffGBwidth->getInitialParamMins();
    QVector<double> ffGBwidthmax=ffGBwidth->getInitialParamMaxs();

    // fit function object for 3D Gaussian fits:
    QFFitFunction3D* ff3D=new QFFitFunctionPSF3DGaussian();
    if (!ff3D) return;
    QVector<double> ff3Dmin=ff3D->getInitialParamMins();
    QVector<double> ff3Dmax=ff3D->getInitialParamMaxs();

    /*qDebug()<<"channels="<<channels;
    qDebug()<<"size_z="<<size_z;
    qDebug()<<"width="<<width;
    qDebug()<<"height="<<height;
    qDebug()<<"zsteps="<<zsteps;
    qDebug()<<"est_psf_width="<<est_psf_width;
    qDebug()<<"est_psf_height="<<est_psf_height;

    qDebug()<<"deltaXY="<<deltaXY;
    qDebug()<<"deltaZ="<<deltaZ;
    qDebug()<<"ROIxy="<<ROIxy;
    qDebug()<<"ROIz="<<ROIz;
    qDebug()<<"pixels_per_frame="<<pixels_per_frame;
    qDebug()<<"min_distance="<<min_distance;*/
    bool useMask=true;
    bool* mask=NULL;
    if (maskI && maskI->maskGetHeight()==height && maskI->maskGetWidth()==width) {
        mask=maskI->maskGet();
    }
    int updateCounter=0;
    QList<int> initial_beads_x, initial_beads_y, initial_beads_z;


    // FIND INITIAL BEAD POSITIONS
    updateCounter=0;
    if (dlgEvaluationProgress) {
        dlgEvaluationProgress->setLabelText(tr("finding beads in 3D stack ... "));
        dlgEvaluationProgress->setRange(ROIz/2, (size_z-ROIz/2)*2);
    }
    for (int z=ROIz/2; z<size_z-ROIz/2; z=z+zsteps) {
        if (dlgEvaluationProgress) dlgEvaluationProgress->setValue(z);
        double* frame=data->getImageStack(stack, z, 0);
        // DETERMINE level for SEGMENTATION
        double level=statisticsQuantile(frame, width*height, 1.0-double(pixels_per_frame)/double(width*height));
        //qDebug()<<"step "<<z<<"  level="<<level;

        // FIND INITIAL BEADS by SEGMENTATION with level
        QVector<int> pixs_x, pixs_y;
        QVector<bool> ignored;
        for (int y=0; y<height; y++) {
            for (int x=0; x<width; x++) {
                const int i=y*width+x;
                if ((!useMask || !mask || !mask[i]) && frame[i]>=level && x>ROIxy/2 && x<width-ROIxy/2 && y>ROIxy/2 && y<height-ROIxy/2) {
                    pixs_x<<x;
                    pixs_y<<y;
                    ignored<<false;
                    //qDebug()<<"    possible: "<<x<<y;
                }
            }
        }
        // SETUP DISTANCE MATRIX OF INITIAL BEADS
        double* distance_matrix=qfCallocT<double>(pixs_x.size()*pixs_x.size());
        for (int i=0; i<pixs_x.size(); i++) {
            for (int j=0; j<pixs_x.size(); j++) {
                distance_matrix[i*pixs_x.size()+j]=sqrt(qfSqr((double)pixs_x[i]-(double)pixs_x[j])+qfSqr((double)pixs_y[i]-(double)pixs_y[j]));
            }
        }

        // FILTER DISTANCE MATRIX FOR MINIMUM DISTANCE
        bool done=false;
        while (!done) {
            done=true;
            // determine the nextPix to look at
            int nextPix=0;
            while (nextPix<pixs_x.size() && ignored[nextPix]) {
                nextPix++;
            }
            //qDebug()<<"    nextPix="<<nextPix;
            if (nextPix<pixs_x.size()) {

                int count=0;
                // count beads in vicinity of the nextPix
                double maxI=0;
                int brightestIdx=-1;
                QList<int> nearPixels;
                for (int i=0; i<pixs_x.size(); i++) {
                    if (i!=nextPix && !ignored[i] && distance_matrix[nextPix*pixs_x.size()+i]<min_distance) {
                        count++;
                        if (brightestIdx<0) {
                            maxI=frame[i];
                            brightestIdx=i;
                            nearPixels<<i;
                        } else if (frame[i]>maxI) {
                            maxI=frame[i];
                            brightestIdx=i;
                            nearPixels<<i;
                        }
                    }
                }
                //qDebug()<<"      count="<<count;
                //qDebug()<<"      brightestIdx="<<brightestIdx;
                //qDebug()<<"      maxI="<<maxI;
                //qDebug()<<"      nearPixels="<<nearPixels;

                // if count==0: add bead
                if (count==0) {
                    initial_beads_x<<pixs_x[nextPix];
                    initial_beads_y<<pixs_y[nextPix];
                    initial_beads_z<<z;
                    ignored[nextPix]=true;
                    done=false;
                    //qDebug()<<"    added single-pix: "<<nextPix;
                } else if (count>0 && brightestIdx>=0) { // if more beads: add the brightest and ignore the rest
                    initial_beads_x<<pixs_x[brightestIdx];
                    initial_beads_y<<pixs_y[brightestIdx];
                    initial_beads_z<<z;
                    ignored[brightestIdx]=true;
                    ignored[nextPix]=true;
                    for (int i=0; i<nearPixels.size(); i++) {
                        ignored[nearPixels[i] ]=true;
                    }
                    //qDebug()<<"    added more-pix:   "<<brightestIdx<<" ( "<<nearPixels.size()<<" more)";
                    done=false;
                }
            }
        }
        // filter output beads once again:
        for (int i=initial_beads_x.size()-1; i>=0; i--) {
            for (int j=0; j<i; j++) {
                double d=sqrt(qfSqr(deltaXY*(initial_beads_x[i]-initial_beads_x[j]))+qfSqr(deltaXY*(initial_beads_y[i]-initial_beads_y[j]))+qfSqr(deltaZ*(initial_beads_z[i]-initial_beads_z[j])));
                if (d<min_distance*deltaXY) {
                    initial_beads_x.removeAt(i);
                    initial_beads_y.removeAt(i);
                    initial_beads_z.removeAt(i);
                    break;
                }
            }
        }

        qfFree(distance_matrix);

        if (updateCounter%5==0) {
            if (dlgEvaluationProgress) dlgEvaluationProgress->setLabelText(tr("finding beads in 3D stack ... found %1").arg(initial_beads_x.size()));
            QApplication::processEvents();
        }
        if (dlgEvaluationProgress&& dlgEvaluationProgress->wasCanceled()) break;
        updateCounter++;
    }
    //qDebug()<<"beads: "<<initial_beads_x.size();

    if (!dlgEvaluationProgress || !dlgEvaluationProgress->wasCanceled()) {
        // write back fit results to record!
        record->disableEmitResultsChanged();
        record->resultsSetNumber(evalID, "pixel_size", deltaXY);
        record->resultsSetNumber(evalID, "step_size", deltaZ);
        record->resultsSetInteger(evalID, "initial_beads_found", initial_beads_x.size());
        record->resultsSetInteger(evalID, "roi_size_xy", ROIxy);
        record->resultsSetInteger(evalID, "roi_size_z", ROIz);
        record->resultsSetInteger(evalID, "beadsearch_zstep_size", zsteps);
        record->resultsSetInteger(evalID, "beadsearch_pixels_per_frame", pixels_per_frame);
        record->resultsSetNumberList(evalID, "beadsearch_initial_positions_x", initial_beads_x);
        record->resultsSetNumberList(evalID, "beadsearch_initial_positions_y", initial_beads_y);
        record->resultsSetNumberList(evalID, "beadsearch_initial_positions_z", initial_beads_z);
        record->enableEmitResultsChanged();



        if (dlgEvaluationProgress) dlgEvaluationProgress->setRange(0, channels*initial_beads_x.size());
        if (dlgEvaluationProgress) dlgEvaluationProgress->setLabelText(tr("evaluating beads ..."));
        // EVALUATE BEADS IN STACK
        updateCounter=0;
        for (int c=0; c<channels; c++) {
            cimg_library::CImg<double> image(data->getImageStack(stack, 0, c), width, height, size_z, true);

            for (int b=0; b<initial_beads_x.size(); b++) {
                if (dlgEvaluationProgress) dlgEvaluationProgress->setValue(c*initial_beads_x.size()+b);

                // CUT ROI AROUND PIXEL
                cimg_library::CImg<double> roi=image.get_crop(initial_beads_x[b]-ROIxy/2, initial_beads_y[b]-ROIxy/2, initial_beads_z[b]-ROIz/2, initial_beads_x[b]+ROIxy/2, initial_beads_y[b]+ROIxy/2, initial_beads_z[b]+ROIz/2);
                //roi.save_tiff(QString("c:\\temp\\roi_c%1_b%2.tif").arg(c).arg(b).toLatin1().data());
                // COORDINATES OF TOP-LEFT CORNER OF ROI
                const double x0tl=double(initial_beads_x[b]-ROIxy/2)*deltaXY;
                const double y0tl=double(initial_beads_y[b]-ROIxy/2)*deltaXY;
                const double z0tl=double(initial_beads_z[b]-ROIz/2)*deltaZ;
                // COORDINATES OF THE CENTER OF ROI in absolute nanometers
                const double x0=double(initial_beads_x[b])*deltaXY;
                const double y0=double(initial_beads_y[b])*deltaXY;
                const double z0=double(initial_beads_z[b])*deltaZ;
                // COORDINATES OF THE CENTER OF ROI as index into ROI
                const int x0i=ROIxy/2;
                const int y0i=ROIxy/2;
                const int z0i=ROIz/2;
                // absolute X,Y,Z coordinates (in nanometers) of the ROI
                QVector<double> X, Y, Z;
                qfGridXYZ(X, Y, Z, x0tl, y0tl, z0tl, roi.width(), roi.height(), roi.depth(), deltaXY, deltaXY, deltaZ);

                // cuts through center pixel with x-coordinate vectors
                QVector<double> cutX(roi.width()); //=image.get_crop(0, y0i, z0i, roi.width()-1, y0i, z0i).unroll('x');
                QVector<double> cutXX;
                qfGridX(cutXX, x0tl, roi.width(), deltaXY);
                for (int i=0; i<cutX.size(); i++) {
                    cutX[i]=roi(i, y0i, z0i);
                }
                QVector<double> cutY(roi.height()); //=image.get_crop(x0i, 0, z0i, x0i, roi.height()-1, z0i).unroll('x');
                QVector<double> cutYX;
                qfGridX(cutYX, y0tl, roi.height(), deltaXY);
                for (int i=0; i<cutY.size(); i++) {
                    cutY[i]=roi(x0i, i, z0i);
                }
                QVector<double> cutZ(roi.depth()); //=image.get_crop(x0i, y0i, 0, x0i, y0i, roi.depth()-1).unroll('x');
                QVector<double> cutZX;
                qfGridX(cutZX, z0tl, roi.depth(), deltaZ);
                for (int i=0; i<cutZ.size(); i++) {
                    cutZ[i]=roi(x0i, y0i, i);
                }

                // estimate initial params
                double init_background=qfstatisticsAverage(constructQVectorFromItems(
                                                               statisticsQuantile(cutX.data(), cutX.size(), 0.1),
                                                               statisticsQuantile(cutY.data(), cutY.size(), 0.1),
                                                               statisticsQuantile(cutZ.data(), cutZ.size(), 0.1))
                                                           );
                double init_amplitude=qfstatisticsAverage(constructQVectorFromItems(
                                                              statisticsMax(cutX.data(), cutX.size()),
                                                              statisticsMax(cutY.data(), cutY.size()),
                                                              statisticsMax(cutZ.data(), cutZ.size()))
                                                          )-init_background;
                double init_w12=est_psf_width;
                double init_w3=est_psf_height;

                // fit X-cut
                QVector<double> cutXP=ff1D->getInitialParamValues();
                for (int i=0; i<cutXP.size(); i++) {
                    if (ff1D->getParameterID(i)=="offset") cutXP[i]=init_background;
                    else if (ff1D->getParameterID(i)=="amplitude") cutXP[i]=init_amplitude;
                    else if (ff1D->getParameterID(i)=="position") cutXP[i]=x0;
                    else if (ff1D->getParameterID(i)=="width") cutXP[i]=init_w12;
                }
                alg->fit(cutXP.data(), NULL, cutXX.data(), cutX.data(), NULL, cutX.size(), ff1D, cutXP.data(), NULL, ff1Dmin.data(), ff1Dmax.data());


                // fit Y-cut
                QVector<double> cutYP=ff1D->getInitialParamValues();
                for (int i=0; i<cutYP.size(); i++) {
                    if (ff1D->getParameterID(i)=="offset") cutYP[i]=init_background;
                    else if (ff1D->getParameterID(i)=="amplitude") cutYP[i]=init_amplitude;
                    else if (ff1D->getParameterID(i)=="position") cutYP[i]=y0;
                    else if (ff1D->getParameterID(i)=="width") cutYP[i]=init_w12;
                }
                alg->fit(cutYP.data(), NULL, cutYX.data(), cutY.data(), NULL, cutY.size(), ff1D, cutYP.data(), NULL, ff1Dmin.data(), ff1Dmax.data());



                // fit Z-cut
                QVector<double> cutZP=ff1D->getInitialParamValues();
                for (int i=0; i<cutZP.size(); i++) {
                    if (ff1D->getParameterID(i)=="offset") cutZP[i]=init_background;
                    else if (ff1D->getParameterID(i)=="amplitude") cutZP[i]=init_amplitude;
                    else if (ff1D->getParameterID(i)=="position") cutZP[i]=z0;
                    else if (ff1D->getParameterID(i)=="width") cutZP[i]=init_w3;
                }


                // fit X/Y-cuts along Z-axis
                QVector<double> zpos, fitZX_width, fitZY_width;
                for (int z=z0i-ROIz/4; z<=z0i+ROIz/4; z++) {
                    zpos<<z;

                    // fit X-cut
                    for (int i=0; i<cutX.size(); i++) {
                        cutX[i]=roi(i, y0i, z);
                    }
                    QVector<double> cutZXP=ff1D->getInitialParamValues();
                    int wid=-1;
                    for (int i=0; i<cutZXP.size(); i++) {
                        if (ff1D->getParameterID(i)=="offset") cutZXP[i]=init_background;
                        else if (ff1D->getParameterID(i)=="amplitude") cutZXP[i]=init_amplitude;
                        else if (ff1D->getParameterID(i)=="position") cutZXP[i]=x0;
                        else if (ff1D->getParameterID(i)=="width") { wid=i; cutZXP[i]=init_w12; }
                    }
                    alg->fit(cutZXP.data(), NULL, cutXX.data(), cutX.data(), NULL, cutX.size(), ff1D, cutZXP.data(), NULL, ff1Dmin.data(), ff1Dmax.data());
                    if (wid>=0) fitZX_width<<cutZXP[wid];


                    // fit Y-cut
                    for (int i=0; i<cutX.size(); i++) {
                        cutX[i]=roi(x0i, i, z);
                    }
                    QVector<double> cutZYP=ff1D->getInitialParamValues();
                    wid=-1;
                    for (int i=0; i<cutZYP.size(); i++) {
                        if (ff1D->getParameterID(i)=="offset") cutZYP[i]=init_background;
                        else if (ff1D->getParameterID(i)=="amplitude") cutZYP[i]=init_amplitude;
                        else if (ff1D->getParameterID(i)=="position") cutZYP[i]=y0;
                        else if (ff1D->getParameterID(i)=="width") {wid=i; cutZYP[i]=init_w12; }
                    }
                    alg->fit(cutZYP.data(), NULL, cutYX.data(), cutY.data(), NULL, cutY.size(), ff1D, cutZYP.data(), NULL, ff1Dmin.data(), ff1Dmax.data());
                    if (wid>=0) fitZY_width<<cutZYP[wid];
                }

                // fit gaussian beam width to X/Y cut results
                QVector<double> fitXZCutResults=ffGBwidth->getInitialParamValues();
                QVector<double> fitYZCutResults=ffGBwidth->getInitialParamValues();

                for (int i=0; i<fitXZCutResults.size(); i++) {
                    if (ffGBwidth->getParameterID(i)=="zR") { fitXZCutResults[i]=init_w3; fitYZCutResults[i]=init_w3; }
                    else if (ffGBwidth->getParameterID(i)=="position") {  fitXZCutResults[i]=z0; fitYZCutResults[i]=z0;  }
                    else if (ffGBwidth->getParameterID(i)=="w0") { fitXZCutResults[i]=init_w12; fitYZCutResults[i]=init_w12; }
                }
                alg->fit(fitXZCutResults.data(), NULL, zpos.data(), fitZX_width.data(), NULL, zpos.size(), ffGBwidth, fitXZCutResults.data(), NULL, ffGBwidthmin.data(), ffGBwidthmax.data());
                alg->fit(fitYZCutResults.data(), NULL, zpos.data(), fitZY_width.data(), NULL, zpos.size(), ffGBwidth, fitYZCutResults.data(), NULL, ffGBwidthmin.data(), ffGBwidthmax.data());



                // fit XYZ-stack to 3D model
                QVector<double> fit3DP=ff3D->getInitialParamValues();
                int xposi=-1, yposi=-1, zposi=-1;
                for (int i=0; i<fit3DP.size(); i++) {
                    if (ff3D->getParameterID(i)=="offset") fit3DP[i]=init_background;
                    else if (ff3D->getParameterID(i)=="amplitude") fit3DP[i]=init_amplitude;
                    else if (ff3D->getParameterID(i)=="position_x") {xposi=i; fit3DP[i]=x0;}
                    else if (ff3D->getParameterID(i)=="position_y") {yposi=i; fit3DP[i]=y0;}
                    else if (ff3D->getParameterID(i)=="position_z") {zposi=i; fit3DP[i]=z0;}
                    else if (ff3D->getParameterID(i)=="width1") fit3DP[i]=init_w12;
                    else if (ff3D->getParameterID(i)=="width2") fit3DP[i]=init_w12;
                    else if (ff3D->getParameterID(i)=="width3") fit3DP[i]=init_w3;
                }
                //qDebug()<<"c="<<c<<"  b="<<b;
                //qDebug()<<"init: "<<fit3DP;
                alg->fit3D(fit3DP.data(), NULL, X.data(), Y.data(), Z.data(), roi.data(), NULL, roi.size(), ff3D, fit3DP.data(), NULL, ff3Dmin.data(), ff3Dmax.data());
                //qDebug()<<"fit:  "<<fit3DP<<"\n\n";






                if (updateCounter%1==0) {
                    if (dlgEvaluationProgress) dlgEvaluationProgress->setLabelText(tr("evaluating beads ... (c=%1, b=%2)").arg(c).arg(b));
                    QApplication::processEvents();
                }
                if (dlgEvaluationProgress&& dlgEvaluationProgress->wasCanceled()) break;
                updateCounter++;


                record->disableEmitResultsChanged();
                record->resultsSetNumber(evalID, QString("channel%1_beads").arg(c), b+1);
                record->resultsSetString(evalID, QString("cutx_fitfunction"), ff1D->id());
                record->resultsSetStringList(evalID, QString("cutx_fitfunction_params"), ff1D->getParameterIDs());
                record->resultsSetString(evalID, QString("cuty_fitfunction"), ff1D->id());
                record->resultsSetStringList(evalID, QString("cuty_fitfunction_params"), ff1D->getParameterIDs());
                record->resultsSetString(evalID, QString("cutz_fitfunction"), ff1D->id());
                record->resultsSetStringList(evalID, QString("cutz_fitfunction_params"), ff1D->getParameterIDs());
                record->resultsSetString(evalID, QString("fit3d_fitfunction"), ff3D->id());
                record->resultsSetStringList(evalID, QString("fit3d_fitfunction_params"), ff3D->getParameterIDs());
                record->resultsSetString(evalID, QString("cutxz_gaussianbeam_fitfunc"), ffGBwidth->id());
                record->resultsSetStringList(evalID, QString("cutxz_gaussianbeam_fitfunc"), ffGBwidth->getParameterIDs());
                record->resultsSetString(evalID, QString("cutyz_gaussianbeam_fitfunc"), ffGBwidth->id());
                record->resultsSetStringList(evalID, QString("cutyz_gaussianbeam_fitfunc"), ffGBwidth->getParameterIDs());

                record->resultsSetNumberList(evalID, QString("channel%1_bead%2_cutx_fitresult").arg(c).arg(b), cutXP);
                record->resultsSetNumberList(evalID, QString("channel%1_bead%2_cuty_fitresult").arg(c).arg(b), cutYP);
                record->resultsSetNumberList(evalID, QString("channel%1_bead%2_cutz_fitresult").arg(c).arg(b), cutZP);
                record->resultsSetNumberList(evalID, QString("channel%1_bead%2_cutxz_zpos").arg(c).arg(b), zpos);
                record->resultsSetNumberList(evalID, QString("channel%1_bead%2_cutxz_width").arg(c).arg(b), fitZX_width);
                record->resultsSetNumberList(evalID, QString("channel%1_bead%2_cutxz_gaussianbeam_results").arg(c).arg(b), fitXZCutResults);
                record->resultsSetNumberList(evalID, QString("channel%1_bead%2_cutyz_zpos").arg(c).arg(b), zpos);
                record->resultsSetNumberList(evalID, QString("channel%1_bead%2_cutyz_width").arg(c).arg(b), fitZY_width);
                record->resultsSetNumberList(evalID, QString("channel%1_bead%2_cutyz_gaussianbeam_results").arg(c).arg(b), fitYZCutResults);

                record->resultsSetNumberList(evalID, QString("channel%1_bead%2_fit3d_results").arg(c).arg(b), fit3DP);
                if (c>0 && xposi>=0 && yposi>=0 && zposi>=0) {
                    bool ok=true;
                    QVector<double> fitres0=record->resultsGetAsDoubleList(evalID, QString("channel%1_bead%2_fit3d_results").arg(0).arg(b), &ok);
                    if (ok && xposi<fitres0.size() && yposi<fitres0.size() && zposi<fitres0.size()) {
                        QVector<double> d;
                        d<<(fitres0[xposi]-fit3DP[xposi]);
                        d<<(fitres0[yposi]-fit3DP[yposi]);
                        d<<(fitres0[zposi]-fit3DP[zposi]);
                        d<<sqrt(qfSqr(fitres0[xposi]-fit3DP[xposi])+qfSqr(fitres0[yposi]-fit3DP[yposi])+qfSqr(fitres0[zposi]-fit3DP[zposi]));
                        record->resultsSetNumberList(evalID, QString("channel%1_bead%2_fit3d_distc0").arg(0).arg(b), d);
                        record->resultsSetNumberList(evalID, QString("channel%1_bead%2_fit3d_distc0").arg(c).arg(b), d);
                        //qDebug()<<d;
                    }
                }

                record->enableEmitResultsChanged();
            }

            if (dlgEvaluationProgress&& dlgEvaluationProgress->wasCanceled()) break;

            // write back fit results to record!
            record->disableEmitResultsChanged();
            record->resultsSetNumber(evalID, "channels", c+1);
            record->enableEmitResultsChanged();

        }
    }

    
    if (alg) delete alg;
    alg=NULL;
    if (ff1D) delete ff1D;
    ff1D=NULL;
    if (ff3D) delete ff3D;
    ff3D=NULL;

}
