#include "qftcspcreaderpicoquantpt3.h"
#include <QObject>

QFTCSPCReaderPicoquantPT3::QFTCSPCReaderPicoquantPT3() {
    tttrfile=NULL;
    currentTTTRRecordNum=0;
    ofltime=0;
    overflows=0;
    duration=0;
}

QFTCSPCReaderPicoquantPT3::~QFTCSPCReaderPicoquantPT3() {
    if (tttrfile) fclose(tttrfile);
}

QString QFTCSPCReaderPicoquantPT3::filter() const {
    return QObject::tr("PicoQuant PicoHarp 300 File (*.pt3)");
}

QString QFTCSPCReaderPicoquantPT3::formatName() const {
    return QObject::tr("PicoQuant PicoHarp 300 PT3 File");
}

bool QFTCSPCReaderPicoquantPT3::open(QString filename) {
    close();
    tttrfile=fopen(filename.toAscii().data(), "rb");
    if (tttrfile) {
        QString error="";
        bool ok=PT3ReadConfiguration(tttrfile, &txtHeader, &binHeader, &boardHeader, &TTTRHeader, error);
        syncperiod = (double)1E9 / TTTRHeader.CntRate0; //in nanoseconds

        if (!ok) {
            setLastError(error);
            return false;
        }

        fgetpos(tttrfile, &fileResetPos);
        currentTTTRRecordNum=0;
        current.microtime_offset=0;
        current.microtime_deltaT=boardHeader.Resolution;
        nextRecord();

        // read some photons to estimate a countrate
        cr.clear();
        double t=0;
        uint16_t cnt=0;
        do {
            QFTCSPCRecord r=getCurrentRecord();
            t=r.absoluteTime();
            cr[r.input_channel]=cr.value(r.input_channel, 0)+1;
            cnt++;
        } while (nextRecord() && (t<0.01) && (cnt<10000));
        for (int i=0; i<inputChannels(); i++) {
            cr[i]=cr.value(i, 0)/t/1000.0;
        }

        fsetpos(tttrfile, &fileResetPos);
        currentTTTRRecordNum=0;
        current.microtime_offset=0;
        current.microtime_deltaT=boardHeader.Resolution;

        duration=double(binHeader.Tacq)/1000.0;

        return true;
    }
    setLastError(QObject::tr("could not open TTTR file '%1'").arg(filename));
    duration=0;
    return false;
}

void QFTCSPCReaderPicoquantPT3::close() {
    if (tttrfile) fclose(tttrfile);
    tttrfile=NULL;
    //fileResetPos=0;
    currentTTTRRecordNum=0;
    ofltime=0;
    overflows=0;

}

void QFTCSPCReaderPicoquantPT3::reset() {
    fgetpos(tttrfile, &fileResetPos);
    currentTTTRRecordNum=0;
    ofltime=0;
    overflows=0;
    nextRecord();

}

bool QFTCSPCReaderPicoquantPT3::nextRecord() {
    PT3Record TTTRrecord;

    do {
        // READ ONE RECORD and check for errors
        int result = fread( &TTTRrecord, 1, sizeof(TTTRrecord) ,tttrfile);
        if (result!= sizeof(TTTRrecord)) {
            setLastError(QObject::tr("error reading input file."));
            return false;
        }
        currentTTTRRecordNum++;

        // calculate the true time of the photon
        double truensync = ((double)ofltime+(double)TTTRrecord.bits.numsync);
        double truetime = (truensync*syncperiod + (double)TTTRrecord.bits.dtime*(double)boardHeader.Resolution)/1000000000.0;

        if(TTTRrecord.bits.channel!=0xF) {
            current.macrotime=truetime;
            current.input_channel=TTTRrecord.bits.channel;
            current.microtime_channel=0;
        } else { /* this means we have a 'special record' */
            if(TTTRrecord.special.markers==0) { // not a marker means overflow
                ofltime += PT3T3WRAPAROUND; /* unwrap the time tag overflow */
                overflows++;
            } else {
                // we have a marker
                truetime = truensync*syncperiod/1000000000.0;
            }
        }
    } while (TTTRrecord.bits.channel==0xF);


    return (int64_t(currentTTTRRecordNum)<TTTRHeader.Records && !feof(tttrfile));
}

double QFTCSPCReaderPicoquantPT3::measurementDuration() const {
    //return double(binHeader.Tacq)/1000.0;
    return duration;
}

uint16_t QFTCSPCReaderPicoquantPT3::inputChannels() const {

    return binHeader.RoutingChannels;
}

double QFTCSPCReaderPicoquantPT3::avgCountRate(uint16_t channel) const {
    return cr.value(channel, 0);
}

QFTCSPCRecord QFTCSPCReaderPicoquantPT3::getCurrentRecord() const {
    return current;
}

double QFTCSPCReaderPicoquantPT3::percentCompleted() const {
    return double(currentTTTRRecordNum)/double(TTTRHeader.Records)*100.0;
}

