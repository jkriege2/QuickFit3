
/*
  Name: sdffsequenceelements.h
  Copyright: (c) 2008
  Author: Jan Krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/

/** \file sdffsequenceelements.h
 * \ingroup hlseqgen_sdff_elements
 *
 * In this file you will find some tool methods and classes that define the elements a SDFF sequence
 * is built up from (ramps, ...)
 */
/*@{*/

#ifndef SDFFSEQUENCEELEMENTS_H
#define SDFFSEQUENCEELEMENTS_H

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <cstdio>
#include <stdint.h>
#include <cmath>

#include "tools.h"
#include "datatable.h"
#include "sequencererror.h"
#include "channelmanager.h"
#include "sequencegenerator.h"
#include "sdfftools.h"


namespace sequencer {


class SDFFelementManager; // forward declaration
class SDFFparser; // forward declaration


/** \brief this class represents all sequence elements, i.e. ramps, valuechanges, clocks ...
 * \ingroup hlseqgen_sdff_elements
 *
 * This class also implements a double linkes list that can be used to store more than one element.
 * when constructed bot \c previous and \c next point to \c NULL, i.e. the element is sole. So you can
 * use set_next() and set_previous() to put the element into a list. The main list management shell be
 * done by SDFFelementManager.
 */
class SDFFsequenceElement {
  protected:
    /** \brief hidden parameterless constructor */
    SDFFsequenceElement() {}
    /** \brief start time of element */
    double t_start;
    /** \brief duration of element */
    double t_duration;
    /** \brief channel the element belongs/writes to */
    std::string channel;
    /** \brief parser object this element belongs to */
    SDFFelementManager* emanager;
    /** \brief file this element is defined in */
    std::string filename;
    /** \brief line number of the definition of this element */
    unsigned long line;

    /** \brief double linked list implementation: points to the next element */
    SDFFsequenceElement* next;
    /** \brief double linked list implementation: points to the previous element */
    SDFFsequenceElement* previous;

    /** \brief if this is set (after a call of check_name() ) the channel is a digital channel, otherwise
     *         it's an analog channel. */
    bool digital;

    /** \brief check whether the channel exists in the given channelManager \a cmanager object. \a parser is used to
     *         issue error messages. This also sets the \c digital member.
     */
    bool check_name(SDFFparser* parser, channelManager* cmanager);
  public:
    /** \brief class constructor */
    SDFFsequenceElement(SDFFelementManager* emanager, std::string channel, double t_start, double t_duration=0) {
      this->channel=channel;
      this->t_start=t_start;
      this->t_duration=t_duration;
      this->filename="";
      this->line=0;
      this->emanager=emanager;
      this->next=NULL;
      this->previous=NULL;
    };

    /** \brief returns the next element */
    GetMacro(SDFFsequenceElement*, next);
    /** \brief sets the next element */
    SetMacro(SDFFsequenceElement*, next);

    /** \brief returns the previous element */
    GetMacro(SDFFsequenceElement*, previous);
    /** \brief sets the previous element */
    SetMacro(SDFFsequenceElement*, previous);


    /** \brief class destructor */
    virtual ~SDFFsequenceElement() {};

    /** \brief returns the start time */
    GetMacro(double, t_start);
    /** \brief sets the start time */
    SetMacro(double, t_start);

    /** \brief returns the duration */
    GetMacro(double, t_duration);
    /** \brief sets the duration */
    SetMacro(double, t_duration);

    /** \brief returns the channel */
    GetMacro(std::string, channel);
    /** \brief sets the channel */
    SetMacro(std::string, channel);

    /** \brief returns the filename */
    GetMacro(std::string, filename);
    /** \brief sets the filename */
    SetMacro(std::string, filename);

    /** \brief returns the parser */
    GetMacro(SDFFelementManager*, emanager);
    /** \brief sets the parser */
    SetMacro(SDFFelementManager*, emanager);

    /** \brief returns the line */
    GetMacro(unsigned long, line);
    /** \brief sets the line */
    SetMacro(unsigned long, line);

    /** \brief this method returns the real t_start (in microseconds) when also looking at the sampling
     *
     * To understand this method you have to know that you can define times freely in a SDFF sequence. But
     * when generating the data that is sent to DAC hardware you will hav to do some sampling of the times.
     * So this method uses the sampling period that is stored in the parser object to determine the real
     * t_start.
     *
     * \image html sample_timestep.png
     *
     * From this we can deduce that we will round up to the next timestep. So we get this formula:
     *
     * \f[ \mbox{\ttfamily real\_t\_start}=\mbox{ceil}\left(\frac{\mbox{\ttfamily t\_start}}{\mbox{\ttfamily sample\_timestep}}\right)\cdot \mbox{\ttfamily sample\_timestep} \f]
     */
    virtual double extract_real_t_start();

    /** \brief this method returns the end time (in microseconds) when also looking at the sampling
     *
     * \f[ \mbox{\ttfamily real\_t\_start}=\mbox{floor}\left(\frac{\mbox{\ttfamily t\_start}+\mbox{\ttfamily t\_duration}}{\mbox{\ttfamily sample\_timestep}}-1\right)\cdot \mbox{\ttfamily sample\_timestep} \f]
     */
    virtual double extract_real_t_end();

    /** \brief generate the sequence element that is represented by this class.
     *
     * This method gets a sequenceGenerator object from the SDFFelementManager stored in \a emanager.
     */
    virtual void generate()=0;

    /** \brief do an error check for the data in this object, e.g. check if we cross min/max ...
     *
     * This method uses the parser object that can be extracted from the SDFFelementManager to issue error messages.
     */
    virtual void check()=0;

    /** \brief returns the value at t=extract_real_t_end() */
    virtual double get_endvalue()=0;

    /** \brief returns the value at t=extract_real_t_start() */
    virtual double get_startvalue()=0;
};



/** \brief this is a set instruction, the duration is until the next instruction or the end of the sequence
 * \ingroup hlseqgen_sdff_elements
 */
class SDFFSEset: public SDFFsequenceElement {
  protected:
    /** \brief hidden parameterless constructor */
    SDFFSEset() {}
    /** \brief value after the \c set command: \c 0,1 for digital channels and any value for analog channels */
    double value;
  public:
    /** \brief class constructor */
    SDFFSEset(SDFFelementManager* emanager, std::string channel, double t_start, double value=0):
      SDFFsequenceElement(emanager, channel, t_start, 0)
    {
      this->value=value;
    };

    /** \brief class destructor */
    virtual ~SDFFSEset() {};

    /** \brief returns the value */
    GetMacro(double, value);
    /** \brief setss the value */
    SetMacro(double, value);

    /** \brief generate the sequence element that is represented by this class.
     *
     * This method gets a sequenceGenerator object from the SDFFelementManager stored in \a emanager.
     */
    virtual void generate();

    /** \brief do an error check for the data in this object, e.g. check if we cross min/max ...
     *
     * This method uses the parser object that can be extracted from the SDFFelementManager to issue error messages.
     */
    virtual void check();

    /** \brief returns the value at t=extract_real_t_end() */
    virtual double get_endvalue();

    /** \brief returns the value at t=extract_real_t_start() */
    virtual double get_startvalue();
};

/** \brief types of functions
 * \ingroup hlseqgen_sdff_elements
 */
enum sdff_ramp_type {
  sdffRTLinear,          /*!< \brief a linear ramp */
  sdffRTExponential,     /*!< \brief an exponential ramp */
  sdffRTCosine,          /*!< \brief a cosine ramp */
  sdffRTSigmoidal        /*!< \brief a sigmoidal ramp */
};


/** \brief this is a ramp instruction
 * \ingroup hlseqgen_sdff_elements
 *
 * \section ramptypes ramp types
 * \subsection linramp linear ramp
 *  \image html ramp_lin.png "linear ramp"
 *
 *  \f[ r(t)=x_0-\frac{x_1-x_0}{\mbox{duration}}\cdot(t-t_0) \f]
 *
 * \subsection expramp exponential ramp
 *  \image html ramp_exp.png "exponential ramps for <tt>tau=6*duration/7, 2*duration/2, 0.2*duration/7</tt> "
 *    \f[ r(t)=A\cdot e^{-t/\tau}+A_0 \f]
 *  where we set:
 *    \f[ A=\frac{x_0-x_1}{e^{-t_0/\tau}-e^{-t_1/\tau}},\ \ \ \ \ A_0=A\cdot e^{-t_0/\tau}-x_0 \f]
 *
 * \subsection cosramp cosine ramp
 *  \image html ramp_cos.png "cosine ramp"
 *    \f[ r(t)=\frac{A}{2}\cdot\left[\cos(2\pi f\cdot(t-t_0))+1\right]-x_1 \f]
 *  where we set:
 *    \f[ f=\frac{1}{2\cdot\mbox{duration}},\ \ \ \ \ A=x_1-x_0 \f]
 *
 * \subsection sigramp sigmoidal ramp
 *  \image html ramp_sig.png "sigmoidal ramp for <tt>tau=1, 2, 0.2, 20</tt> "
 *    \f[ r(t)=f_0(\tau)+f_1(\tau)\cdot \mbox{sig}\left(\frac{20\cdot(t-t_0)}{t_1-t_0}-10\right)\f]
 *  where we set:
 *    \f[ \mbox{sig}(t,\tau)=\frac{1}{1+\exp(-t/\tau)} \f]
 *    \f[ f_1(\tau)=\frac{x_1-x_0}{\mbox{sig}(10,\tau)-\mbox{sig}(-10,\tau)},\ \ \ \ \ f_0=x_0-f_1(\tau)\cdot\mbox{sig}(-10,\tau) \f]
 */
class SDFFSEramp: public SDFFsequenceElement {
  protected:
    /** \brief hidden parameterless constructor */
    SDFFSEramp() {}
    /** \brief value at the end of the ramp */
    double end_value;

    /** \brief \f$ \tau \f$ parameter for exponential ramps, given in microseconds */
    double tau;

    /** \brief type of the ramp */
    sdff_ramp_type type;

    /** \brief a sigmoidal function */
    inline double sig(double t, double tau) { return 1/(1+exp(-t/tau)); };
  public:
    /** \brief class constructor */
    SDFFSEramp(SDFFelementManager* emanager, std::string channel, double t_start, double t_duration, sdff_ramp_type type=sdffRTLinear, double endvalue=0):
      SDFFsequenceElement(emanager, channel, t_start, t_duration)
    {
      this->end_value=endvalue;
      this->tau=2.0*t_duration/7.0; // standard value
      this->type=type;
    };

    /** \brief class destructor */
    virtual ~SDFFSEramp() {};

    /** \brief returns the endvalue */
    GetMacro(double, end_value);
    /** \brief setss the endvalue */
    SetMacro(double, end_value);

    /** \brief returns the type of the ramp */
    GetMacro(sdff_ramp_type, type);
    /** \brief sets the type of the ramp */
    SetMacro(sdff_ramp_type, type);

    /** \brief returns the \f$ \tau \f$ parameter */
    GetMacro(double, tau);
    /** \brief setss the \f$ \tau \f$ parameter */
    SetMacro(double, tau);

    /** \brief generate the sequence element that is represented by this class.
     *
     * This method gets a sequenceGenerator object from the SDFFelementManager stored in \a emanager.
     */
    virtual void generate();

    /** \brief do an error check for the data in this object, e.g. check if we cross min/max ...
     *
     * This method uses the parser object that can be extracted from the SDFFelementManager to issue error messages.
     */
    virtual void check();

    /** \brief returns the value at t=extract_real_t_end() */
    virtual double get_endvalue();

    /** \brief returns the value at t=extract_real_t_start() */
    virtual double get_startvalue();
};


/** \brief this is a rectangular pulse of defined length
 * \ingroup hlseqgen_sdff_elements
 *
 * this class generates a pulse:
 * \verbatim
        |
value   +                   +-----------------+
        |                   |                 |
        |                   |<--  duration -->|
        |                   |                 |
current +   ----------------+                 +----------
        |____________________________________________________ time
\endverbatim
 */
class SDFFSEpulse: public SDFFsequenceElement {
  protected:
    /** \brief hidden parameterless constructor */
    SDFFSEpulse() {}
    /** \brief value at the end of the ramp */
    double value;

    /** \brief type of the ramp */
    sdff_ramp_type type;
  public:
    /** \brief class constructor */
    SDFFSEpulse(SDFFelementManager* emanager, std::string channel, double t_start, double t_duration, double value=1):
      SDFFsequenceElement(emanager, channel, t_start, t_duration)
    {
      this->value=value;
    };

    /** \brief class destructor */
    virtual ~SDFFSEpulse() {};

    /** \brief returns the value */
    GetMacro(double, value);
    /** \brief setss the value */
    SetMacro(double, value);

    /** \brief generate the sequence element that is represented by this class.
     *
     * This method gets a sequenceGenerator object from the SDFFelementManager stored in \a emanager.
     */
    virtual void generate();

    /** \brief do an error check for the data in this object, e.g. check if we cross min/max ...
     *
     * This method uses the parser object that can be extracted from the SDFFelementManager to issue error messages.
     */
    virtual void check();

    /** \brief returns the value at t=extract_real_t_end() */
    virtual double get_endvalue();

    /** \brief returns the value at t=extract_real_t_start() */
    virtual double get_startvalue();
};



/** \brief this class manages all sequence elements (SDFFsequenceElement descendant).
 * \ingroup hlseqgen_sdff_elements
 *
 * \section hlseqgen_sdff_elements_mands Datastructures
 * This class has a map that has an entry for each channel. Suborderd to each such channel is a vector that holds
 * the elements. In addition to that the \c next and \c previous properties of the elements are set correctly, so that
 * each element may always access its predecessor and successor. This is somehow a double data structure, but it presents
 * the nice feature to be easily able to sort in an array and do an index access while still we can quickly extract
 * predecessor and successor information.
 *
 * \dotfile elementmanager.dot "schematic of the used data structure"
 *
 * \section hlseqgen_sdff_elements_manalgo Algorithm of generate()
 * For generating a sequence this classes uses a quite simple algorithm (implemented in generate() ). There is
 * only one important point that has to be taken care of: If there is a gap between two successive elements,
 * the algorithm will have to fill this gap, i.e. it outputs the voltage at the end of the previuos element until
 * the next element starts. The algorithm looks like this:
 *
 * <pre>
 * <b>for every</b> channel ch <b>in</b> elements <b>do</b>
 *   \c v0 = 0;
 *   \b if first element starts at \i t=0 \b then  \c v0 = startvalue of first element;
 *   \c t0 = 0;
 *   \b for \b every element \c i in channel ch \b do
 *     \c t1 = start time of \c i -th element;
 *     \c te = end time of \c i -th element;
 *     \b if <tt>t1>t0</tt> \b then
 *       output \c v0 from \c t0 to \c t1 ;
 *     <b>end if</b>
 *     output sequence element \c i from \c t0 to \c te ;
 *     <tt>t0 = te</tt>;
 *     \c v0 = end value of element \c i
 *   <b>end for </b>
 *   output \c v0 from \c t0 to end of sequence
 * <b>end for</b>
 * </pre>
 *
 */
class SDFFelementManager {
  protected:
    /** \brief hidden constructor */
    SDFFelementManager() {};

    /** \brief duration of one sample in microseconds */
    double sample_timestep;

    /** \brief sequence generator used to build the sequence */
    sequenceGenerator* generator;

    /** \brief parser object used for error output */
    SDFFparser* parser;

    /** \brief this map stores all the sequence elements */
    std::map<std::string, std::vector<SDFFsequenceElement*> > elements;

    /** \brief this specifies the duration of the sequence in microseconds. If it is set to a value
     *         smaller than 0 (default) this class will use extract_end_time() to deduce the end
     *         time automatically.
     */
    double end_time;

    /** \brief this is the end time actually used in the generation step. It is set by generate()
     *         according to the value of end_time.
     */
    double use_end_time;

    /** \brief sort all the elements according to their start_time
     *
     * This method utilises the  simple <a href="http://en.wikipedia.org/wiki/Bubble_sort">bubble sort algorithm</a>.
     * It makes only one change to the basic version of this algorithm: It stops whenever there were no swaps in one
     * field sweep. This allows for O(N) in sorted arrays instead of O(N^2). it can also reduce the run-time on partly
     * sorted data.
     *
     * When sorting we have to swap two elements. When doing this we have to care for the \c next and \c previous
     * properties of the swapped elements. The following diagramshows the old (blue) and new (red) links
     * \dotfile doublelinked_swap.dot
     *
     */
    void sort();
  public:
    /** \brief class constructor
     *
     * If \a sample_timestep\c =-1 (default) this constructor will extract \a sample_timestep from the
     * \a generator. If \a generator\c =NULL this constructor will extract the sequence generator from the
     * given parser object
     */
    SDFFelementManager(SDFFparser* parser, sequenceGenerator* generator=NULL, double sample_timestep=-1);

    /** \brief class destructor */
    virtual ~SDFFelementManager() {  };


    /** \brief set the sequence generator */
    SetMacro(sequenceGenerator*, generator);
    /** \brief get the sequence generator */
    GetMacro(sequenceGenerator*, generator);

    /** \brief set the parser object */
    SetMacro(SDFFparser*, parser);
    /** \brief get the parser object */
    GetMacro(SDFFparser*, parser);

    /** \brief set the duration of one sample in microseconds */
    SetMacro(double, sample_timestep);
    /** \brief get the duration of one sample in microseconds */
    GetMacro(double, sample_timestep);

    /** \brief clear all elements. If \a delete_elements is \c true the all elements stored in this object will
     *         be deleted (default). Otherwise this has to be done by the user.
     */
    void clear(bool delete_elements=true);

    /** \brief add an element to the end of the list */
    void add(std::string channel, SDFFsequenceElement* element);

    /** \brief generate the sequence from the stored information. See \ref hlseqgen_sdff_elements_manalgo
     *         for more information
     */
    void generate();

    /** \brief do a consistency check for the stored elements, i.e. do they collide?
     *
     * \returns \c -1 if there was an error and \c 0 if there are no collisions
     */
    int check();

    /** \brief check whether there are sequence elements for the specified channel */
    inline bool channel_exists(std::string name){
      return elements.find(name)!=elements.end();
    };

    /** \brief extracts the end time of the sequence.
     *
     * This method iterates through all sequence elements and then extracts the latest time mark from them.
     */
    double extract_end_time();
};

};
#endif // SDFFSEQUENCEELEMENTS_H


/*@}*/
