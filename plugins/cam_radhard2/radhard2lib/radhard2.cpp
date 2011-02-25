// RADHARD2.CPP
// The main system abstractor.

#include "radhard2.h"

using namespace std;



//------------------------------------------------
/*!
This constructor automatically opens and configures the USB device, thus preparing the
communication channel. After construction, the Radhard2 object will provide a fully functional
communication link with the RADHARD2 motherboard. No further action is needed to set
up the bus.

\sa Radhard2::~Radhard2().
*/
Radhard2::Radhard2()
{
    DEBUG("RHCS     >> DEBUG(Radhard2::Radhard2) -- Starting the construction of a Radhard2 object.");
    OpenUsbDevice();
    DEBUG("RHCS     >> DEBUG(Radhard2::Radhard2) -- Radhard2 object successfully constructed.");
}

//------------------------------------------------
/*!
    This destructor automatically closes the USB communication channel that is handled and owned
    by the current Radhard2 object. All the resources that were automatically allocated during
    USB setup are automatically released here, so no further action is required.

    \sa Radhard2::Radhard2().
*/
Radhard2::~Radhard2()
{
    DEBUG("RHCS     >> DEBUG(Radhard2::~Radhard2) -- Starting the destruction of the Radhard2 object.");
    CloseUsbDevice();
    DEBUG("RHCS     >> DEBUG(Radhard2::~Radhard2) -- Radhard2 object successfully destroyed.");
}

//------------------------------------------------
/*!
    This method can be used to send a command to the RADHARD2 motherboard via the USB. The provided
    command/value pair will be formatted in a single 32 bit value and sent to the motherboard. The
    write operation is fully monitored and eventual transmission errors will be streamed out to the
    console.

    \param[in] command Numerical value representing the command to be sent to the RADHARD2 motherboard.
    A comprehensive list of available commands is defined in the file Radhard2lib.h.
    It is advisable to use the defined alias instead of the numerical values directly.
    It is clearer and less error-prone.

    \param[in] value Numerical value associated with the current command to be sent. The actual effect of
    the value depends on the command it is associated to. A complete list of commands
    and their accepted values is provided in the file Radhard2lib.h. Note that although
    the maximum width of this parameter is 24 bit, some commands need values in a lower
    range (the address change commands need values in the 0-31 range) or need no value
    at all. Refer to Radhard2lib.h for further details.

    \return A boolean success code. If the transmission is faulty, then this method will write an error
    message to the console and return false.

    \sa Radhard2::ReadData().
*/
bool Radhard2::SendCommand(unsigned int command, unsigned int value)
{
    unsigned char buffer[4] = {0, 0, 0, 0};
    int result;

    DEBUG("RHCS     >> DEBUG(Radhard2::SendCommand) -- Passed command -- value: " << hex << command << " -- " << hex << value);
    VERB("RHCS     >> INFO -- Preparing transmission...");

    if (value > 0xFFFFFF)
    {
        VERB("RHCS     >> INFO -- Passed value is greater than 0xFFFFFF.");
        VERB("RHCS     >> INFO -- Passed value is clipped to 0xFFFFFF.");

        value = 0xFFFFFF;
    }

    unsigned int cmdval = command + value;

    // Prepare the data to send.
    buffer[0] = ( (cmdval) & 0xFF );
    buffer[1] = ( (cmdval>>8) & 0xFF);
    buffer[2] = ( (cmdval>>16) & 0xFF);
    buffer[3] = ( (cmdval>>24) & 0xFF);


    #ifdef FDEBUG
        printf ("> 0x%02X 0x%02X 0x%02X 0x%02X\n" , buffer[3] , buffer[2] ,buffer[1] ,buffer[0] );
    #endif

    DEBUG("RHCS     >> DEBUG(Radhard2::SendCommand) -- Now calling usb_bulk_write.");
    DEBUG("RHCS     >> DEBUG(Radhard2::SendCommand) -- DEVICE: " << device);
    DEBUG("RHCS     >> DEBUG(Radhard2::SendCommand) -- EP_OUT: " << EP_OUT);
    DEBUG("RHCS     >> DEBUG(Radhard2::SendCommand) -- BUFFER: " << buffer);
    DEBUG("RHCS     >> DEBUG(Radhard2::SendCommand) -- USB TIMEOUT: " << USB_TIMEOUT);

    // Send!
    result = usb_bulk_write(device, EP_OUT, (char *)buffer, 4, USB_TIMEOUT);
    if( result < 0 )
    {
        cout << "RHCS     >> ERROR. Usb transmission falied to send data." << endl;
        return false;
    }
    if( result != 4 )
    {
        cout << "RHCS     >> ERROR. Usb transmission falied to send 4 bytes." << endl;
        DEBUG("RHCS     >> DEBUG(Radhard2::SendCommand) -- RHCS sent " << result << " bytes.");

        return false;
    }

    VERB("RHCS     >> INFO -- Transmission successful.");

    return true;
}

//------------------------------------------------
/*!
This method can be used to read a value sent from the RADHARD2 motherboard. Since the entire system
works with 32 bit values and messages, this method expects to read four bytes. These bytes are then
assembled in an integer variable that represent the result of the operation.

The entire read process is monitored. Eventual errors are reported to the output console.

This method is timeouted. It will return with an error if it fails to read 4 bytes in the allocated
maximum time (defined in Radhard2lib.h).

\remark
This method has been designed to work with all commands that return a single value or code as the
result of their operation. It is not advisable to use it to read multiple values (an array readout)
due to the high resulting overhead.

\param[in] destination Pointer to an unsigned integer variable that will be assigned with the result
of the read operation.

\return A boolean success code. If the reception is faulty, then this method will write an error
message to the console and return false.

\sa Radhard2::SendCommand().
*/
bool Radhard2::ReadData(unsigned int* destination)
{

    unsigned char buffer[4];

    VERB("RHCS     >> INFO -- Preparing reception...");

    // Read one value.
    int result = usb_bulk_read(device, EP_IN, (char *)buffer, 4, USB_TIMEOUT);
    if(result < 0 )
    {
        cout << "RADHARD2 >> ERROR. Problem during USB operation." << endl;
        cout << "RHCS     >> ERROR. Usb transmission falied to receive data." << endl;
        return false;
    }
    if( result != 4 )
    {
        cout << "RHCS     >> ERROR. Usb transmission falied to receive 4 bytes." << endl;
        DEBUG("RHCS     >> DEBUG(Radhard2::ReadData) -- RHCS received " << result << " bytes.");

        return false;
    }


    // Build the result.
    *destination = ( buffer[0]) | ( (buffer[1] << 8) ) | ( (buffer[2] << 16) ) | ( (buffer[3] << 24) );


    //*destination = static_cast<unsigned int>( (unsigned char)buffer[0]);
    //*destination |= static_cast<unsigned int>( (unsigned char)(buffer[1] << 8) );
    //*destination |= static_cast<unsigned int>( (unsigned char)(buffer[2] << 16) );
    //*destination |= static_cast<unsigned int>( (unsigned char)(buffer[3] << 24) );

    #ifdef FDEBUG
        printf ("< 0x%02X 0x%02X 0x%02X 0x%02X\n" , buffer[3] , buffer[2] ,buffer[1] ,buffer[0] );
    #endif

    DEBUG("RHCS     >> DEBUG(Radhard2::ReadData) -- buffer[0] = " << buffer[0]);
    DEBUG("RHCS     >> DEBUG(Radhard2::ReadData) -- buffer[1] = " << buffer[1]);
    DEBUG("RHCS     >> DEBUG(Radhard2::ReadData) -- buffer[2] = " << buffer[2]);
    DEBUG("RHCS     >> DEBUG(Radhard2::ReadData) -- buffer[3] = " << buffer[3]);
    DEBUG("RHCS     >> DEBUG(Radhard2::ReadData) -- destination = " << hex << *destination);
    DEBUG("RHCS     >> DEBUG(Radhard2::ReadData) -- RHCS received the expected number of bytes from the USB.");
    VERB("RHCS     >> INFO -- Reception successful.");

    return true;
}



/*!
    This method can be used to read an array of 1024 32bit values sent from the RADHARD2 motherboard. The
    read values will be used to format and assign the raw counts array owned by the provided RawImage object.

    The entire read process is monitored. Eventual errors are reported to the output console.

    This method is timeouted. It will return with an error if it fails to read 4096 bytes in the allocated
    maximum time (defined in Radhard2lib.h).

    \remark
    This method has been designed to work with the CMD_GET_MEMORY and CMD_GET_TEST_MEMORY commands. Using
    it in other contexts is meaningless.

    \param[in] destination reference to a data section

    \return A boolean success code. If the reception is faulty, then this method will write an error
    message to the console and return false.

    \sa Radhard2::SendCommand().
*/
bool Radhard2::ReadImage(uint32_t* destination)
{
    unsigned char buffer[8*MAX_USB_PACKET_SIZE];

    VERB("RHCS     >> INFO -- Preparing reception...");

    // Get a poiter to destination.
    unsigned int* destinationPtr = destination;

    // Read values.
    int result;

    DEBUG("RHCS     >> DEBUG(Radhard2::ReadData) -- Reading packets.");

    // Read packet.
    result = usb_bulk_read(device, EP_IN, (char *)buffer, 8*MAX_USB_PACKET_SIZE, 4*USB_TIMEOUT);
    if(result < 0 )
    {
        cout << "RHCS     >> ERROR. Usb transmission falied to receive data, possibly due to a timeout or an error." << endl;
        return false;
    }
    if( result != 8*MAX_USB_PACKET_SIZE )
    {
        cout << "RHCS     >> ERROR. Usb transmission falied to receive " << dec << 8*MAX_USB_PACKET_SIZE << "." << endl;
        DEBUG("RHCS     >> DEBUG(Radhard2::ReadData) -- Radhard2 received " << dec << result << " bytes corresponding to " << dec << static_cast<unsigned int>(result/4) << " pixels.");

        return false;
    }

    DEBUG("RHCS     >> DEBUG(Radhard2::ReadData) -- Radhard2 received " << dec << result << " bytes corresponding to " << dec << static_cast<unsigned int>(result/4) << " pixels.");
    DEBUG("RHCS     >> DEBUG(Radhard2::ReadData) -- Starting conversion.");


    // Build the result.
    for(int convertLoop=0; convertLoop<(8*MAX_USB_PACKET_SIZE/4); ++convertLoop)
    {
        /*(unsigned int)*/destinationPtr[convertLoop] = buffer[convertLoop*4];
        /*(unsigned int)*/destinationPtr[convertLoop] |= (buffer[convertLoop*4+1] << 8);
        /*(unsigned int)*/destinationPtr[convertLoop] |= (buffer[convertLoop*4+2] << 16);
        /*(unsigned int)*/destinationPtr[convertLoop] |= (buffer[convertLoop*4+3] << 24);

            #ifdef FDEBUG
            printf ("< 0x%08X " , (unsigned int)destinationPtr[convertLoop] );
            #endif
       DEBUG(".");
    }

    DEBUG("");
    DEBUG("RHCS     >> DEBUG(Radhard2::ReadData) -- Partial conversion done.");
    VERB("RHCS     >> INFO -- Reception successful.");

    return true;
}


//------------------------------------------------
/*!
    This method will initialize and configure the USB device. It will scan the USB looking for a connected
    RADHARD2 motherboard. Then it will perform the standard USB setup procedure:
    - Open the USB device.
    - Set the configuration.
    - Claim the communication interface.
    - Ensure the synchronization with the motherboard controller.

    \return An integer error code, negative in case of error.

    \sa Radhard2::CloseUsbDevice().
*/
int Radhard2::OpenUsbDevice()
{
    struct usb_bus* bus;
    int flag, magic, retry_count;

    DEBUG("RHCS     >> DEBUG(Radhard2::OpenUsbDevice) -- Opening USB channel...");
    VERB("RHCS     >> INFO -- Opening USB channel...");
    DEBUG("RHCS     >> DEBUG(Radhard2::OpenUsbDevice) -- Now calling usb_init.");

    usb_init();

    DEBUG("RHCS     >> DEBUG(Radhard2::OpenUsbDevice) -- Now calling usb_find_busses.");

    usb_find_busses();

    DEBUG("RHCS     >> DEBUG(Radhard2::OpenUsbDevice) -- Now calling usb_find_devices.");

    usb_find_devices();

    laspMainboard = NULL;
    flag = 0;

    for(bus = usb_busses; bus; bus = bus->next)
    {
        for(laspMainboard = bus->devices; laspMainboard; laspMainboard = laspMainboard->next)
        {
            if( laspMainboard->descriptor.idVendor == ID_VENDOR && laspMainboard->descriptor.idProduct == ID_PRODUCT)
            {
                flag = 1;
                break;
            }
        }
        if(flag)
            break;
    }

    if( !laspMainboard)
    {
        cout << "RHCS     >> ERROR. No board detected." << endl;

        DEBUG("RHCS     >> DEBUG(Radhard2::OpenUsbDevice) -- BUS: " << bus);
        DEBUG("RHCS     >> DEBUG(Radhard2::OpenUsbDevice) -- LASPMAINBOARD: " << laspMainboard);
        DEBUG("RHCS     >> DEBUG(Radhard2::OpenUsbDevice) -- DEVICE: " << device);

        return -1;
    }

    DEBUG("RHCS     >> DEBUG(Radhard2::OpenUsbDevice) -- BUS: " << bus);
    DEBUG("RHCS     >> DEBUG(Radhard2::OpenUsbDevice) -- LASPMAINBOARD: " << laspMainboard);
    DEBUG("RHCS     >> DEBUG(Radhard2::OpenUsbDevice) -- DEVICE: " << device);
    DEBUG("RHCS     >> DEBUG(Radhard2::OpenUsbDevice) -- IDVENDOR received: " << laspMainboard->descriptor.idVendor);
    DEBUG("RHCS     >> DEBUG(Radhard2::OpenUsbDevice) -- IDVENDOR expected: " << ID_VENDOR);
    DEBUG("RHCS     >> DEBUG(Radhard2::OpenUsbDevice) -- IDPRODUCT received: " << laspMainboard->descriptor.idProduct);
    DEBUG("RHCS     >> DEBUG(Radhard2::OpenUsbDevice) -- IDPRODUCT expected: " << ID_PRODUCT);
    DEBUG("RHCS     >> DEBUG(Radhard2::OpenUsbDevice) -- Now calling usb_open on the device pointer.");

    device = usb_open(laspMainboard);

    DEBUG("RHCS     >> DEBUG(Radhard2::OpenUsbDevice) -- DEVICE: " << device);

    if( usb_set_configuration(device, laspMainboard->config->bConfigurationValue))
    {
        cout << "RHCS     >> ERROR. Could not set USB configuration. Exiting." << endl;

        DEBUG("RHCS     >> DEBUG(Radhard2::OpenUsbDevice) -- BUS: " << bus);
        DEBUG("RHCS     >> DEBUG(Radhard2::OpenUsbDevice) -- LASPMAINBOARD: " << laspMainboard);
        DEBUG("RHCS     >> DEBUG(Radhard2::OpenUsbDevice) -- DEVICE: " << device);

        usb_reset(device);
        usb_close(device);

        exit(EXIT_FAILURE);
    }

    while( usb_claim_interface(device, laspMainboard->config->interface->altsetting->bInterfaceNumber));

    DEBUG("RHCS     >> DEBUG(Radhard2::OpenUsbDevice) -- USB interface claimed!");
    DEBUG("RHCS     >> DEBUG(Radhard2::OpenUsbDevice) -- Starting dummy reception (synchronization).");

    unsigned char buffer[513];
    unsigned int result = 0;

    result = usb_bulk_read(device, EP_IN, (char *)buffer, MAX_USB_PACKET_SIZE, 1000);
    DEBUG("RHCS     >> DEBUG(Radhard2::OpenUsbDevice) -- The dummy readout operation received " << dec << result << " bytes at startup.");

    result = usb_bulk_read(device, EP_IN, (char *)buffer, MAX_USB_PACKET_SIZE, 1000);
    DEBUG("RHCS     >> DEBUG(Radhard2::OpenUsbDevice) -- The dummy readout operation received " << dec << result << " bytes at startup.");

    result = usb_bulk_read(device, EP_IN, (char *)buffer, MAX_USB_PACKET_SIZE, 1000);
    DEBUG("RHCS     >> DEBUG(Radhard2::OpenUsbDevice) -- The dummy readout operation received " << dec << result << " bytes at startup.");



    /*do
{
result = usb_bulk_read(device, EP_IN, (char *)buffer, MAX_USB_PACKET_SIZE, USB_TIMEOUT);

if(debug_flag)
cout << "RHCS     >> DEBUG(Radhard2::OpenUsbDevice) -- The dummy readout operation received " << result << " bytes at startup." << endl;
} while(result!=0);*/

    DEBUG("RHCS     >> DEBUG(Radhard2::OpenUsbDevice) -- End of dummy readout.");

    /*unsigned int initialMagic = 0;

for(retry_count=0; retry_count < NR_OF_RETRIES; ++retry_count)
{
SendCommand(CMD_TEST_COMLINK);
ReadData(&initialMagic);
if(initialMagic == MAGIC_NUMBER)
return 0;
else
{
if(debug_flag)
cout << "RHCS     >> DEBUG(Radhard2::OpenUsbDevice) -- received magic: " << initialMagic << " MAGIC: " << MAGIC_NUMBER << endl;

if(verbose_flag)
cout << "RHCS     >> INFO -- USB communication attempt number " << retry_count + 1 << " failed." << endl;
}
}

cout << "RHCS     >> ERROR. The board did not respond after " << retry_count << " attempts." << endl;
return -1;*/
    return 0;
}

//------------------------------------------------
/*!
This method will release all the resources that were allocated by the system to manage the USB communication
with the motherboard.

\return Nothing.

\sa Radhard2::OpenUsbDevice().
*/
void Radhard2::CloseUsbDevice()
{
    DEBUG("RHCS     >> DEBUG(Radhard2::CloseUsbDevice) -- Closing USB channel...");
    DEBUG("RHCS     >> DEBUG(Radhard2::CloseUsbDevice) -- I am calling the usb_release_interface, usb_reset and then the usb_close functions...");
    DEBUG("RHCS     >> DEBUG(Radhard2::CloseUsbDevice) -- device: " << device);
    DEBUG("RHCS     >> DEBUG(Radhard2::CloseUsbDevice) -- laspMainboard: " << laspMainboard);
    DEBUG("RHCS     >> DEBUG(Radhard2::CloseUsbDevice) -- bInterfaceNumber: " << (laspMainboard->config->interface->altsetting->bInterfaceNumber==true?"true":"false"));


    VERB("RHCS     >> INFO -- Closing USB channel...");

    // Release the usb resource.
    usb_reset(device);
    usb_release_interface(device, laspMainboard->config->interface->altsetting->bInterfaceNumber);

    DEBUG("RHCS     >> DEBUG(Radhard2::CloseUsbDevice) -- Now calling usb_close.");

    usb_close(device);

    DEBUG("RHCS     >> DEBUG(Radhard2::CloseUsbDevice) -- USB channel closed.");

    VERB("RHCS     >> INFO -- USB channel closed.");

    return;
}

//------------------------------------------------
/*!
This method forses a resynchronization of the system. It flushes the read FIFO and signals it as empty.

\return Nothing.
*/
void Radhard2::Synchronize()
{
    unsigned char buffer[513];
    unsigned int result = 0;

    result = usb_bulk_read(device, EP_IN, (char *)buffer, MAX_USB_PACKET_SIZE, 1000);
    DEBUG("RHCS     >> DEBUG(Radhard2::Synchronize) -- The dummy readout operation received " << dec << result << " bytes at startup.");

    result = usb_bulk_read(device, EP_IN, (char *)buffer, MAX_USB_PACKET_SIZE, 1000);
    DEBUG("RHCS     >> DEBUG(Radhard2::Synchronize) -- The dummy readout operation received " << dec << result << " bytes at startup.");

    result = usb_bulk_read(device, EP_IN, (char *)buffer, MAX_USB_PACKET_SIZE, 1000);
    DEBUG("RHCS     >> DEBUG(Radhard2::Synchronize) -- The dummy readout operation received " << dec << result << " bytes at startup.");

    return;
}

