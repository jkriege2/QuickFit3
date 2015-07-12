#ifndef QFVISUALIZE3DWIDGET_H
#define QFVISUALIZE3DWIDGET_H

#include <qglview.h>
class QGLSceneNode;
class QGLBuilder;


// see also: http://www.cg.informatik.uni-siegen.de/data/Tutorials/EG2006/RTVG03_VolumeRendering.pdf
class QFVisualize3DWidget : public QGLView
{
        Q_OBJECT
    public:
        enum VisualizationMode {
            TextureRendering
        };

        struct VolumeData {
            /** \brief percentile of lower invisible pixel values */
            double transparentLowerThreshold;
            /** \brief percentile of upper invisible pixel values */
            double transparentUpperThreshold;
            VisualizationMode vizMode;
            double* volumeData;
            int sizeX, sizeY, sizeZ;
            bool internalData;
            QVector<int> lut, lut_alpha;
            bool showTeapot;

            void clear();
        };

        explicit QFVisualize3DWidget(QWidget *parent = 0);
        virtual ~QFVisualize3DWidget();

        /** \brief set new volume dataset, data is managed externally */
        int addVolumeData(double* data, int sX, int sY, int sZ);
        void setVolumeData(double* data, int sX, int sY, int sZ);
        void deleteVolumeData(int i);

        void getVolumeDataCount() const { return vdata.size(); }
        VolumeData getVolumeData(int i) const { return vdata[i]; }
        int getCurrentVolumeDataID() const { return currentVData; }
        /** \brief displays the famous teapot */
        void setTeapot();
        /** \brief clear the current volume dataset */
        void clearVolumeData();

        double getTransparentLowerThreshold() const { return vdata[currentVData].transparentLowerThreshold ;}
        double getTransparentUpperThreshold() const { return vdata[currentVData].transparentUpperThreshold ;}

        
    signals:
        
    public slots:
        void setCurrentVolumeData(int i);
        /** \brief update the displayed scene from the volume data set with setVolumeData() */
        void updateVolumeVisualization(bool updateWidget=true);

        void setVisualizationMode(VisualizationMode mode);

        void setLUT(const QVector<int>& lut);
        void setAlphaLUT(const QVector<int>& lut);

        void setTransparentLowerThreshold(double val);
        void setTransparentUpperThreshold(double val);

    protected:
         void initializeGL(QGLPainter *painter);
         void paintGL(QGLPainter *painter);

        QGLSceneNode *scene;

        QList<VolumeData> vdata;

        int currentVData;



        int lutSize;


        int lastMaxDir;
        bool lastDirPos;

        void findMaxViewDir(int& maxDir, bool& dirPos);


        void addStackNodeX(QGLBuilder& builder, QRgb* lut, int lutSize, double lmin, double lmax, bool posDir);
        void addStackNodeY(QGLBuilder& builder, QRgb* lut, int lutSize, double lmin, double lmax, bool posDir);
        void addStackNodeZ(QGLBuilder& builder, QRgb* lut, int lutSize, double lmin, double lmax, bool posDir);

        inline float arrayX2WorldX(int x) {
            return float(x)-float(sizeX-1)/2.0;
        }
        inline float arrayY2WorldY(int y) {
            return float(y)-float(sizeY-1)/2.0;
        }
        inline float arrayZ2WorldZ(int z) {
            return float(z)-float(sizeZ-1)/2.0;
        }
};

#endif // QFVISUALIZE3DWIDGET_H
