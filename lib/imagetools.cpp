/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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

#include "imagetools.h"


QVector<bool> resizeBoolVecImage(const QVector<bool>& image, int w, int h, int newWidth, int newHeight, int* askUserResizeMasks, const QString& image_name, const QString& comment) {
    if (w==newWidth && h==newHeight) return image;
    if (image.size()<w*h) return QVector<bool>();
    double ax=double(w)/double(h);
    double newax=double(newWidth)/double(newHeight);
    bool transform=false;
    if (askUserResizeMasks) {
        transform=((*askUserResizeMasks)>0);
        if ((*askUserResizeMasks)<0) {
            int res=QMessageBox::question(NULL, QObject::tr("resize %1").arg(image_name), QObject::tr("%2\nShould the %1 be resized?").arg(image_name).arg(comment), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
            if (res==QMessageBox::Yes) {
                *askUserResizeMasks=1;
                transform=true;
            } else if (res==QMessageBox::No) {
                *askUserResizeMasks=0;
                transform=false;
            }
        }
    } else {
        transform=true;
    }
    if (transform) {
        QVector<bool> r;

        if (w*h==newWidth*newHeight) {
            for (int i=0; i<newWidth*newHeight; i++) {
                r<<image[i];
            }
        } else if (newWidth*newHeight>w*h) {
            for (int i=0; i<newWidth*newHeight; i++) {
                int x=i%w;
                int y=i/w;
                int x2=round(double(x)/double(newWidth)*double(w));
                int y2=round(double(y)/double(newHeight)*double(h));
                int i2=qBound(0, y2*w+x2, w*h);
                r<<image[i2];
            }
        } else if (newWidth*newHeight<w*h) {
            QVector<int> rr;
            for (int i=0; i<newWidth*newHeight; i++) {
                r<<false;
                rr<<0;
            }
            int max=0;
            for (int i=0; i<w*h; i++) {
                int x=i%w;
                int y=i/h;
                int x2=round(double(x)/double(w)*double(newWidth));
                int y2=round(double(y)/double(h)*double(newHeight));
                int i2=y2*newWidth+x2;
                if (i2>=0 && i2<w*h && image[i]) {
                    rr[i2]=rr[i2]+1;
                    max=qMax(max, rr[i2]);
                }
            }
            for (int i=0; i<newWidth*newHeight; i++) {
                r[i]=(rr[i]>=max/2);
            }
        }

        return r;
    }
    return QVector<bool>();
}
