/*
Copyright (c) 2012-2015 by Sebastian Isbaner

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

#ifndef BURST_H
#define BURST_H
#include "parameters.h"

struct Photon;
struct Photons;
struct Burst;
struct BurstDual;

#define PHOTONMACRO
//struct Flag {
//    unsigned char channel : 4;
//    unsigned char marker : 4;
//};
#ifdef PHOTONMACRO
#define isDex(photon) (((photon).flags&DonorEx))
#define isDem(photon)  ( (photon).flags&DonorEm)
#define isAex(photon)  ( (photon).flags&AcceptorEx)
#define isAem(photon)  ( (photon).flags&AcceptorEm)
#define isMarker(photon, marker)  ( (photon).flags&marker)
#define isChannel(photon, channel)  ( (photon).flags&channel)
#define SetDex(photon) { (photon).flags&=(~Markers);(photon).flags|=DonorEx;}
#define SetDem(photon) { (photon).flags&=(~Channels); (photon).flags|=DonorEm;}
#define SetAex(photon) { (photon).flags&=(~Markers); (photon).flags|=AcceptorEx;}
#define SetAem(photon) { (photon).flags&=(~Channels); (photon).flags|=AcceptorEm;}
#define SetChannel(photon,channel) { (photon).flags=photon.flags&(~Channels); (photon).flags|=(channel);}
#define SetMarker (photon,marker) { (photon).flags&=(~Markers); (photon).flags|=(marker);)
#define SetFlags (photon,channel, marker) { (photon).flags=0; (photon).flags|=(channel); (photon).flags|=(marker); }
#define isDem_Dex(photon)  ( ((photon).flags&DonorEx)&&((photon).flags&DonorEm))
#define isDem_Aex(photon)  ( ((photon).flags&AcceptorEx)&&((photon).flags&DonorEm))
#define isAem_Dex(photon)  ( ((photon).flags&DonorEx)&&((photon).flags&AcceptorEm))
#define isAem_Aex(photon)  ( ((photon).flags&AcceptorEx)&&((photon).flags&AcceptorEm))
#define isPhotonFlag(photon, _flags)  (((photon).flags&(_flags))==(_flags))

struct Photon
{
    double time; ///< arrival time in s
    unsigned char flags; ///< contains flags. 8bit. 4 bits for channel(2 used), 4bits for marker (3 used)

    bool isPhotonFlags(const QVector<uint> _flags) const; ///< for multiple flags

    // added temporarily for AlexSim
    inline void setDex() { flags&=(~Markers); flags|=DonorEx;}
    inline void setDem() { flags&=(~Channels); flags|=DonorEm;}
    inline void setAex() { flags&=(~Markers); flags|=AcceptorEx;}
    inline void setAem() { flags&=(~Channels); flags|=AcceptorEm;}
    inline void setChannel(Channel ch) { flags=flags&(~Channels); flags|=ch;}
    inline void setMarker(Marker marker) { flags=flags&(~Markers); flags|=marker;}
    inline void setFlags(Channel ch,Marker marker) { flags=0; flags|=ch; flags=flags|marker; }
};
#else
struct Photon
{
    double time; ///< arrival time in s
    unsigned char flags; ///< contains flags. 8bit. 4 bits for channel(2 used), 4bits for marker (3 used)
    inline bool isDex() const {return (flags&DonorEx);}
    inline bool isDem() const {return (flags&DonorEm);}
    inline bool isAex() const {return (flags&AcceptorEx);}
    inline bool isAem() const {return (flags&AcceptorEm);}
    inline bool isMarker(const Marker marker) const {return flags&marker;}
    inline bool isChannel(const Channel channel) const {return flags&channel;}
    inline bool isPhotonFlag(const uint channel,const uint marker) const {return isPhotonFlag((channel|marker));}
    inline bool isPhotonFlag(const uint _flags) const {return (flags&_flags)==_flags;} ///< for one selection (one channel and one marker)
    bool isPhotonFlags(const QVector<uint> _flags) const; ///< for multiple flags
    inline void setDex() { flags&=(~Markers); flags|=DonorEx;}
    inline void setDem() { flags&=(~Channels); flags|=DonorEm;}
    inline void setAex() { flags&=(~Markers); flags|=AcceptorEx;}
    inline void setAem() { flags&=(~Channels); flags|=AcceptorEm;}
    inline void setChannel(Channel ch) { flags=flags&(~Channels); flags|=ch;}
    inline void setMarker(Marker marker) { flags=flags&(~Markers); flags|=marker;}
    inline void setFlags(Channel ch,Marker marker) { flags=0; flags|=ch; flags=flags|marker; }
    inline bool isDem_Dex() const {return (flags&DonorEx)&&(flags&DonorEm);}
    inline bool isDem_Aex() const {return (flags&AcceptorEx)&&(flags&DonorEm);}
    inline bool isAem_Dex() const {return (flags&DonorEx)&&(flags&AcceptorEm);}
    inline bool isAem_Aex() const {return (flags&AcceptorEx)&&(flags&AcceptorEm);}
};
#endif

struct Photons : QVector<Photon>
{
    void vectorChannelMarker(QVector<double>& vectorTime, const uchar _flags, const int start=0, const int stop=INT_MAX) const;
    QVector<double> vectorChannelMarker(const uchar _flags, const int start=0, int stop=INT_MAX) const;
    inline void vectorChannel(QVector<double>& vectorTime,const Channel channel, const int start=0, int stop=INT_MAX) const {return vectorChannelMarker(vectorTime,channel|DonorEx|AcceptorEx,start, stop);}
    inline void vectorMarker(QVector<double>& vectorTime, const Marker marker, const int start=0, int stop=INT_MAX) const {vectorChannelMarker(vectorTime,DonorEm|AcceptorEm|marker,start, stop);}
    Photons();
    double countrateDem_Dex() const {return nDem_Dex/duration();}
    double countrateDem_Aex() const {return nDem_Aex/duration();}
    double countrateAem_Dex() const {return nAem_Dex/duration();}
    double countrateAem_Aex() const {return nAem_Aex/duration();}
    double countrateDem() const {return nDem()/duration();}
    double countrateDex() const {return nDex()/duration();}
    double countrateAem() const {return nAem()/duration();}
    double countrateAex() const {return nAex()/duration();}
    void countPhotons();
    int nDem_Dex;
    int nDem_Aex;
    int nAem_Dex;
    int nAem_Aex;
    int nDex() const {return nDem_Dex + nAem_Dex;}
    int nAex() const {return nDem_Aex + nAem_Aex;}
    int nAem() const {return nAem_Dex + nAem_Aex;}
    int nDem() const {return nDem_Dex + nDem_Aex;}
    inline double duration() const { if(size()>1) return last().time-first().time; else return 0;}    
    void sort();

};
bool earlierThan(const Photon& p1, const Photon& p2);

/// data struct for one channel burst search
struct Burst
{
    double startTime;
    float duration; ///< burst duration in seconds
    int startIdx; ///< start photon index
    int numberOfPhotons; ///< number of photons in the burst
    /// burstrate in Hz
    inline double burstrate() const {return numberOfPhotons/duration;}
    /// index of the last photon in arrival times vector
    inline int endIdx() const {return (startIdx+numberOfPhotons);}
    /// end time of the burst in sec
    inline double endTime() const {return (startTime+duration);}
    /// initializes all variables to zero
    Burst();
};



/// data struct for one channel burst search (not used yet)
struct burstStruct2
{
    double startTime;
    float duration; ///< burst duration in seconds
    int startIdx; ///< start photon index
    int green; ///< number of green photons
    int red; ///< number of red photons
    inline int numberOfPhotons() const {return (green+red);}
    inline double burstrate() const {return (numberOfPhotons()/duration);}
    inline int endIdx() const {return (startIdx+numberOfPhotons());}
    inline double endTime() const {return (startTime+duration);}
};


enum burstClassification { /// only used for display
    Unclassified=0,
    SingleBurst,
    FRET,
    NoFRET,
    Donly,
    Aonly,
    Dual,
    Background
};

/// data struct for dual channel burst search
struct BurstDual
{
    burstClassification type;
    QString typeName() const;
    double startTime; ///< start time of the burst in sec
    float duration; ///< duration of the burst in seconds
    float durationDonor; ///< duration of the burst in the donor channel in seconds --> used to compare burst durations and their influence on the proximity ratio
    float durationAcceptor; ///< duration of the burst in the acceptor channel in seconds (original burst, not the overlapping part)
    int startIdxDonor; ///< start photon index in channel 1
    int startIdxAcceptor;///< start photon index in channel 2
    int startIdx;///< start photon index
    int numberOfPhotons; ///< total number of photons in the burst. FIXME: may be replaced by: inline int numberOfPhotons() {return (n_Dexc_Aem+n_Aexc_Aem+n_Dexc_Dem+n_Aexc_Dem); }
    int n_Dem_Dex; ///< number of photons in the Donor emission channel during Donor exciatation period
    int n_Aem_Dex; ///< number of photons in the acceptor emission channel during Donor exciatation period
    int n_Dem_Aex; ///< number of photons in the Donor emission channel during Donor exciatation period
    int n_Aem_Aex; ///< number of photons in the acceptor emission channel during acceptor exciatation period

    /// number of photons in the donor excitation period
    inline int n_Dex() const {return (n_Dem_Dex+n_Aem_Dex);}
    /// number of photons in the acceptor excitation period
    inline int n_Aex() const {return (n_Dem_Aex+n_Aem_Aex);}
    /// number of photons in the donor emission period
    inline int n_Dem() const {return (n_Dem_Dex+n_Dem_Aex);}
    /// number of photons in the acceptor emission period
    inline int n_Aem() const {return (n_Aem_Dex+n_Aem_Aex);}
    /// duration in ms
    inline double duration_ms() const {return duration*1e3;}
    /// burstrate in Hz of the dual burst
    inline double burstrate() const {return numberOfPhotons/duration;}
    /// burstrate in kHz of the dual burst
    inline double burstrate_kHz() const {return (numberOfPhotons/duration)*1e-3;}
    /// burst size of the dual burst
    inline double burstSize() const {return numberOfPhotons;}
    /// index of the last photon in arrival times vector of donor emission
    inline int endIdxDonor() const {return (startIdxDonor+n_Dem());}
    /// index of the last photon in arrival times vector of acceptor emission
    inline int endIdxAcceptor() const {return (startIdxAcceptor+n_Aem());}
    /// end time of the burst in sec
    inline double endTime() const {return (startTime+duration);}
    inline double endIdx() const {return (startIdx+numberOfPhotons);}

    // data for the results of a burst
    // raw values should be functions
    double proximityRatioRaw; ///< proximity ratio without corrections
    double stoichiometryRatioRaw; ///< stoichiometry ratio without corrections
    double proximityRatio; ///< proximity ratio with crosstalk, direct excitation and scaling
    double stoichiometryRatio;///< stoichiometry ratio with crosstalk, direct excitation and scaling based on photon number ratio
    double stoichiometryRatioIntensity;///< stoichiometry ratio with crosstalk, direct excitation and scaling based on intensity ratio
    double energyTransfer;///< energy transfer with crosstalk and direct excitation and scaling and gamma factor //FIXME
    double stoichiometryFraction;///<  //FIXME

    /// initializes all variables to zero
    inline void reset(){
        type=Unclassified;
        startTime=0;
        duration=0;
        durationDonor=0;
        durationAcceptor=0;
        startIdxDonor=0;
        startIdxAcceptor=0;
        numberOfPhotons=0;
        n_Dem_Dex=0;
        n_Aem_Dex=0;
        n_Dem_Aex=0;
        n_Aem_Aex=0;
        proximityRatioRaw=0;
        stoichiometryRatioRaw=0;
        proximityRatio=0;
        stoichiometryRatio=0;
        stoichiometryRatioIntensity=0;
        energyTransfer=0;
    }
    /// initializes all variables to zero
    BurstDual();

    /// checks if the burst is outside the selection criteria defined by upper and lower boundaries for S and PR
    inline bool isMasked(const double &minS, const double &maxS, const double &minP, const double &maxP ) const {
        return  (this->stoichiometryRatio < minS) ||
                (this->stoichiometryRatio > maxS) ||
                (this->proximityRatio     < minP) ||
                (this->proximityRatio     > maxP);
    }

    /// \overload inline bool isMasked(const double &minS, const double &maxS, const double &minP, const double &maxP ) const
    /// Masked data are excluded data points. This function returns true, if the data point is excluded by the selection criteria defined in FRETparams.
    inline bool isMasked( const FRETparams_t &FRETparams) const {return this->isMasked( FRETparams.MinS,  FRETparams.MaxS,  FRETparams.MinP,  FRETparams.MaxP);}

    /// write all variables to a string in a \verbatim variable=value\\n \endverbatim format
    const std::string toString() const;
    /// write all variables to a string in a \verbatim value delimiter value delimiter value ... \endverbatim format
    const std::string toString(std::string prefix,std::string delimiter="\t") const;
    const std::string toStringHeader(std::string prefix="# ",std::string delimiter="\t") const;

    /*
     *these function have the wrong duration (approximate exc periods by duration/2
    /// burstrate in Hz in the donor emission channel during donor excitation
    inline double burstrateDemDex() const {return ((n_Dem_Dex)/duration*2);}
    /// burstrate in Hz in the acceptor emission channel during donor excitation
    inline double burstrateAemDex() const {return (n_Aem_Dex)/duration*2;}
    /// burstrate in Hz in the donor emission channel during acceptor excitation
    inline double burstrateDemAex() const {return (n_Dem_Aex)/duration*2;}
    /// burstrate in Hz in the acceptor emission channel during acceptor excitation
    inline double burstrateAemAex() const {return (n_Aem_Aex)/duration*2;}

    /// burstrate in Hz in the donor emission channel
    inline double burstrate_Dem() const {return ((n_Dem_Dex+n_Dem_Aex)/duration);}
    /// burstrate in Hz in the acceptor emission channel
    inline double burstrate_Aem() const {return (n_Aem_Dex+n_Aem_Aex)/duration;}
    /// burstrate in Hz during donor excitation FIXME  approximated duration/2 to be the excitation period
    inline double burstrate_Dex() const {return (n_Dem_Dex+n_Aem_Dex)/duration*2;}
    /// burstrate in Hz during acceptor excitation FIXME  approximated duration/2 to be the excitation period
    inline double burstrate_Aex() const {return (n_Dem_Aex+n_Aem_Aex)/duration*2;}

    */
};


#endif // BURST_H
