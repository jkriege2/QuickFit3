// RADHARD2.H

// This file contains the declaration of the class "radhard2", the abstractor
// of the Radhard2 system. It implements the USB communication protocol.

#ifndef __RADHARD2H__
#define __RADHARD2H__

#include <string>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdint.h>

#undef FDEBUG
#undef FVERBOSE

#include "radhard2lib.h" // General RADHARD2 definitions.
#include <usb.h>         // Main header file of the libusb library.

//! The RADHARD2 main class.
/*!
    \ingroup radhard2
    This class represents the entire RADHARD2 system (LASP mainboard + RADHARD2
    daughterboard) and provides an abstraction layer that simplifies the
    communication with the device. It is possible for a client programmer to
    inherit from Radhard2 and define a custom implementation of its methods.

    This class uses global system resources. Only one instance of this class
    should exist in your software, possibly at global scope.

    The class encapsulates the libusb library for USB communication. Please check
    libusb website (http://libusb.wiki.sourceforge.net/) to get further details
    concerning the library, its implementation and licensing.
*/
class Radhard2
{
    // PUBLIC INTERFACE
    public:

    // Constructors and destructors
    //! The default constructor.
    Radhard2();
    //! The default destructor.
    virtual ~Radhard2();

    // Property management
    //! Returns the magic number.
    /*!
        This method can be used in all context where it is required to know the value of the magic number.
        The magic number is a constant 32 bit value that is returned by the system upon request and that
        is meant to be used to test the communication channel.

        This method provides a convenient way to access the value of the magic number without having to
        scatter hard-coded copies of the constant in the source code.

        \return An unsiged integer representing the value of the magic number.
        */
    inline unsigned int GetMagicNumber() {return MAGIC_NUMBER;};
    //! Returns the opcode of the end of an operation.
    /*!
        This method can be used in all context where it is required to know the value of the constant
        that represent the "operation done" information. The RADHARD2 system will return this value
        to inform the software that the previously required operation has been completed.

        This method provides a convenient way to access the value of the described constant without
        having to scatter hard-coded copies of it in the source code.

        \return An unsiged integer representing the value of the "operation done" constant.
        */
    inline unsigned int GetOperationDone() {return OPERATION_DONE;};

    // Main methods
    //! The USB initializer method.
    virtual int OpenUsbDevice();
    //! The USB shutdown method.
    virtual void CloseUsbDevice();
    //! The command sender.
    virtual bool SendCommand(unsigned int command, unsigned int value = 0);
    //! A data receiver.
    virtual bool ReadData(unsigned int* destination);
    //! A data receiver.
    virtual bool ReadImage(uint32_t* destination);
    //! The syncronization method.
    virtual void Synchronize();

    // PRIVATE IMPLEMENTATION
        private:

    // Private methods

    // Private attributes
    struct usb_device* laspMainboard;	// Pointer to the data structure that represents the motherboard.
    usb_dev_handle* device;			// Pointer to the structure that represents the USB device.
};

#endif
