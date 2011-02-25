/* RADHARD2LIB.H*/

#ifndef __RADHARD2LIBH__
#define __RADHARD2LIBH__

/*! \file radhard2lib.h
\brief Container of useful definitions.
\ingroup radhard2

This header file contains some definitions that are used in the project. The
following areas are covered:
	- USB-related constants and definitions,
	- All the commands that are recognized by RADHARD2,
	- Some system-wide constants.
*/


//============================================================================
// USB
//============================================================================
/*! \def EP_OUT			4
\ingroup radhard2
This constant represents the output endpoint of the FX2 USB chip.
*/
/*! \def EP_IN			8
\ingroup radhard2
This constant represents the input endpoint of the FX2 USB chip.
*/
/*! \def ID_VENDOR		0x0547
\ingroup radhard2
This constant represents the vendor code of the usb device used.
*/
/*! \def ID_PRODUCT		0x1002
\ingroup radhard2
This constant represents the product ID of the usb device used.
*/
/*! \def USB_TIMEOUT		4048
\ingroup radhard2
This constant represents the timeout value for usb communications.
*/
/*! \def MAX_USB_PACKET_SIZE	512
\ingroup radhard2
This constant represents the size in bytes of each usb packet.
*/
/*! \def PICTURE_SIZE		32*32
\ingroup radhard2
This constant represents the size in pixels of the picture taken by RADHARD2.
Each pixel is represented by a 32bit unsigned integer variable containing the
current photon count.
*/
/*! \def NR_OF_RETRIES		3
\ingroup radhard2
This constant represents the number of consecutive retries in case of unsuccessful
usb communication. After the last attempt the communication is considered
permanently lost.
*/
#define EP_OUT			4
#define EP_IN			8
#define ID_VENDOR		0x0547
#define ID_PRODUCT		0x1002
#define USB_TIMEOUT		4048
#define MAX_USB_PACKET_SIZE	512
#define PICTURE_SIZE		32*32
#define NR_OF_RETRIES		3
//============================================================================


//============================================================================
// RADHARD2 COMMANDS
//============================================================================
/*! \def CMD_SET_ROW_E			0x00000000
\ingroup radhard2
This constant represents the SET ROW_E command.

The constants ON and OFF can be used to control the behaviour of this command.

\remark
It is not mandatory to use the constants ON and OFF. Any unsigned integer
can be provided to Radhard2::SendValue(). However, it is important to know
that the SET ROW_E command will interpret only the LSB of the provided value.
This means that an even value is equivalent to the OFF constant and an odd value
is equivalent to the ON constant.
*/
/*! \def CMD_SET_ROW_ADDR		0x01000000
\ingroup radhard2
This constant represents the SET ROW_ADDR command.

This command must be sent with the numerical address of the desired pixel row.

\remark
This command will instruct Radhard2::SendCommand() to clip the provided value.
In other words, any passed value that is greater than 31 will be forced to 31.
*/
/*! \def CMD_SET_COL_E			0x02000000
This constant represents the SET COL_E command.

The constants ON and OFF can be used to control the behaviour of this command.

\remark
It is not mandatory to use the constants ON and OFF. Any unsigned integer
can be provided to Radhard2::SendValue(). However, it is important to know
that the SET COL_E command will interpret only the LSB of the provided value.
This means that an even value is equivalent to the OFF constant and an odd value
is equivalent to the ON constant.
*/
/*! \def CMD_SET_COL_ADDR		0x03000000
\ingroup radhard2
This constant represents the SET COL_ADDR command.

This command must be sent with the numerical address of the desired pixel column.

\remark
This command will instruct Radhard2::SendCommand() to clip the provided value.
In other words, any passed value that is greater than 31 will be forced to 31.
*/
/*! \def CMD_SET_MOD1			0x04000000
\ingroup radhard2
This constant represents the SET MOD1 command.

This command does not need any value to operate.

\remark
This command will instruct Radhard2::SendCommand() to plainly ignore any value
passed to it.
*/
/*! \def CMD_SET_MOD2			0x05000000
\ingroup radhard2
This constant represents the SET MOD2 command.

This command does not need any value to operate.

\remark
This command will instruct Radhard2::SendCommand() to plainly ignore any value
passed to it.
*/
/*! \def CMD_SET_RS			0x06000000
\ingroup radhard2
This constant represents the SET RS command.

The constants ON and OFF can be used to control the behaviour of this command.

\remark
It is not mandatory to use the constants ON and OFF. Any unsigned integer
can be provided to Radhard2::SendValue(). However, it is important to know
that the SET RS command will interpret only the LSB of the provided value.
This means that an even value is equivalent to the OFF constant and an odd value
is equivalent to the ON constant.

\remark
THIS IS A DEBUG FEATURE THAT IS EXPOSED FOR COMPLETENESS ONLY. DO NOT USE IT.
USE CMD_PULSE_RS INSTEAD FOR A CORRECT APPLICATION OF THE RESET PULSE TO THE
RADHARD2 SYSTEM.
*/
/*! \def CMD_PULSE_RS			0x07000000
\ingroup radhard2
This constant represents the PULSE RS command.

This command does not need any value to operate.

\remark
This command will instruct Radhard2::SendCommand() to plainly ignore any value
passed to it.
*/
/*! \def CMD_PULSE_CLK			0x08000000
\ingroup radhard2
This constant represents the PULSE CLK command.

This command does not need any value to operate.

\remark
This command will instruct Radhard2::SendCommand() to plainly ignore any value
passed to it.
*/
/*! \def CMD_SET_ITERATIONS		0x09000000
\ingroup radhard2
This constant represents the SET ITERATIONS command.

This command must be sent with the new value of acquisition iterations.

\remark
Any value between 1 and 0xFFFFFF is allowed. Default is 1024.
*/
/*! \def CMD_START_ACQUISITION		0x0A000000
\ingroup radhard2
This constant represents the START ACQUISITION command.

This command does not need any value to operate.

\remark
This command will instruct Radhard2::SendCommand() to plainly ignore any value
passed to it.
*/
/*! \def CMD_START_TEST_ACQUISITION	0x0B000000
\ingroup radhard2
This constant represents the START TEST ACQUISITION command.

This command does not need any value to operate.

\remark
This command will instruct Radhard2::SendCommand() to plainly ignore any value
passed to it.
*/
/*! \def CMD_GET_STATUS			0x40000000
\ingroup radhard2
This constant represents the GET STATUS command.

This command does not need any value to operate.

The return value for this command is the concatenation of the following data,
starting from the MSB:
	- 0000000000000
	- test_out
	- test_out_ctrl
	- test_mem_o
	- test_mem_i
	- rs
	- mod2
	- mod1
	- col_addr<4:0>
	- col_e
	- row_addr<4:0>
	- row_e

\remark
This command will instruct Radhard2::SendCommand() to plainly ignore any value
passed to it.
*/
/*! \def CMD_GET_ITERATIONS		0x90000000
\ingroup radhard2
This constant represents the GET ITERATIONS command.

This command does not need any value to operate.

The return value for this command is the current value assigned to the acquisition
iteration counter.

\remark
This command will instruct Radhard2::SendCommand() to plainly ignore any value
passed to it.
*/
/*! \def CMD_SET_DIVIDER		0x10000000
\ingroup radhard2
This constant represents the SET DIVIDER command.

This command must be sent with the new value of the clock divider.

\remark
Any value between 1 and 0xFFFFFF is allowed. Default is 0x30 (48), that gives
a programmable clock of about 2MHz.
*/
/*! \def CMD_GET_DIVIDER		0x20000000
\ingroup radhard2
This constant represents the GET DIVIDER command.

This command does not need any value to operate.

The return value for this command is the current value assigned to the clock
divider.

\remark
This command will instruct Radhard2::SendCommand() to plainly ignore any value
passed to it.
*/
/*! \def CMD_GET_CURRENT_OUTPUT		0x30000000
\ingroup radhard2
This constant represents the GET CURRENT OUTPUT command.

This command does not need any value to operate.

The return value for this command is the current value exposed by the addressed
row of pixels.

\remark
This command will instruct Radhard2::SendCommand() to plainly ignore any value
passed to it.
*/
/*! \def CMD_GET_TEST_SCREEN		0x50000000
\ingroup radhard2
This constant represents the GET TEST SCREEN command.

This command does not need any value to operate.

The return value for this command is a constant pattern representing a vertical
gradient image.

\remark
This command will instruct Radhard2::SendCommand() to plainly ignore any value
passed to it.
*/
/*! \def CMD_TEST_COMLINK		0x80000000
\ingroup radhard2
This constant represents the TEST COMLINK command.

This command does not need any value to operate.

The return value for this command is the systemwide constant MAGIC_NUMBER.

\remark
This command will instruct Radhard2::SendCommand() to plainly ignore any value
passed to it.
*/
/*! \def CMD_GET_MEMORY			0x60000000
\ingroup radhard2
This constant represents the GET MEMORY command.

This command does not need any value to operate.

The return value for this command is the current content of the RADHARD2 memory,
i.e. the last taken picture.

\remark
This command will instruct Radhard2::SendCommand() to plainly ignore any value
passed to it.
*/
/*! \def CMD_GET_TEST_MEMORY		0x70000000
\ingroup radhard2
This constant represents the GET TEST MEMORY command.

This command does not need any value to operate.

The return value for this command is a the value contained in the memory
associated with the test pixel.

\remark
This command will instruct Radhard2::SendCommand() to plainly ignore any value
passed to it.
*/
#define CMD_SET_ROW_E			0x00000000 /* needs 1bit value */
#define CMD_SET_ROW_ADDR		0x01000000 /* needs 5bit value */
#define CMD_SET_COL_E			0x02000000 /* needs 1bit value */
#define CMD_SET_COL_ADDR		0x03000000 /* needs 5bit value */
#define CMD_SET_MOD1			0x04000000
#define CMD_SET_MOD2			0x05000000
#define CMD_SET_RS			0x06000000 /* needs 1bit value */
#define CMD_PULSE_RS			0x07000000
#define CMD_PULSE_CLK			0x08000000
#define CMD_SET_ITERATIONS		0x09000000 /* needs 24bit value */
#define CMD_START_ACQUISITION		0x0A000000
#define CMD_START_TEST_ACQUISITION	0x0B000000
#define CMD_GET_STATUS			0x40000000
#define CMD_GET_ITERATIONS		0x90000000

#define CMD_SET_DIVIDER			0x10000000 /* needs 24bit value */
#define CMD_GET_DIVIDER			0x20000000

#define CMD_GET_CURRENT_OUTPUT		0x30000000

#define CMD_GET_TEST_SCREEN		0x50000000
#define CMD_TEST_COMLINK		0x80000000

#define CMD_GET_MEMORY			0x60000000
#define CMD_GET_TEST_MEMORY		0x70000000
//============================================================================


//============================================================================
// CONSTANTS
//============================================================================
/*! \def TRUE			1
\ingroup radhard2
This constant represents the true boolean constant in contexts where a bool type
is not appropriate or convenient.
*/
/*! \def YES			1
\ingroup radhard2
This constant represents a positive answer or condition.
*/
/*! \def ON			1
\ingroup radhard2
This constant represents the will to activate something.
*/
/*! \def FALSE			0
\ingroup radhard2
This constant represents the false boolean constant in contexts where a bool type
is not appropriate or convenient.
*/
/*! \def NO			0
\ingroup radhard2
This constant represents a negative answer or condition.
*/
/*! \def OFF			0
\ingroup radhard2
This constant represents the will to deactivate something.
*/
/*! \def MAGIC_NUMBER		0x10FF1CE1
\ingroup radhard2
This constant is a sort of "signature" of the system and it is used to verify the
quality of the communication channel.
*/
/*! \def OPERATION_DONE		0xFEEDB0B0
\ingroup radhard2
This constant is the code that RADHARD2 uses to inform the control software that
a certain operation is over.
*/
/*! \def IMG_MAX_COUNT		0
\ingroup radhard2
This constant represents the default saturation threshold that is used to build
a normalized greyscale bitmap out of raw image objects. Its value of 0 is
defaulted to the maximum count of the image.
*/

#ifndef TRUE
#  define TRUE				1
#endif
#define YES				1
#define ON				1
#ifndef FALSE
#  define FALSE				0
#endif
#define NO				0
#define OFF				0

#define MAGIC_NUMBER			0x10FF1CE1
#define OPERATION_DONE			0xFEEDB0B0

#define SAT_NORMAL			0
//============================================================================

// MACROS

#define FDEBUG
#define FVERBOSE

/*! \def DEBUG(x)
\ingroup radhard2
This macro provides a convenient way to print its argument x to the output console.
If the FDEBUG token is #undefined, then this macro expands to nothing. This is a
convenient way to remove all debug printouts from source, thus effectively reducing
code size and improving execution speed.

Debug printouts can be suppressed at runtime too by switching a boolean flag.
However, this kind of suppression has no effect on code size and execution speed.
Switching the runtime boolean flag has no effect on program behaviour if debug
printouts have been suppressed at compile time by #undefining FDEBUG.
*/
#if defined(FDEBUG)
   #define DEBUG(x)   std::cerr << x << endl
#else
   #define DEBUG(x)   ;
#endif

/*! \def VERB(x)
\ingroup radhard2
This macro provides a convenient way to print its argument x to the output console.
If the FVERBOSE token is #undefined, then this macro expands to nothing. This is a
convenient way to remove all information printouts from source, thus effectively reducing
code size and improving execution speed.

Information printouts can be suppressed at runtime too by switching a boolean flag.
However, this kind of suppression has no effect on code size and execution speed.
Switching the runtime boolean flag has no effect on program behaviour if information
printouts have been suppressed at compile time by #undefining FVERBOSE.
*/
#if defined(FVERBOSE)
   #define VERB(x)   std::cerr << x << endl
#else
   #define VERB(x)   ;
#endif

//============================================================================

#endif
