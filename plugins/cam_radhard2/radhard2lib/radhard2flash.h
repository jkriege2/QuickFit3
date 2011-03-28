#ifndef RADHARD2FLASH_H
#define RADHARD2FLASH_H


//-----------------------------------------------------------------------------
//
//  Project:      UMPP
//
//  File name:    main.c
//
//  Author:       Jerome Maye
//  Modified:     Theo Kluter
//
//  Created:      31/08/2006
//  Modification: 25/04/2007
//
//  Description:  This program is responsible for downloading a bitstream into
//                a Xilinx Virtex-II(PRO) FPGA via the FX2 USB chip, using Select
//                Map programmation mode.
//
//  Copyright (c) 2007 EPFL-LAP EPFL-GR-CH All rights reserved.
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//
//  Includes
//
//-----------------------------------------------------------------------------
// standards
#include <stdio.h>
#include <string.h>

// USB functions
#include <usb.h>



/*! \brief check header of bitfile
    \ingroup radhard2
    This function parses a bitfile and checks if the header matches. At the
    end, the pointer into the file is at the first byte to write to the FPGA.

    \param infile bitstream file
    \return  length of the bitstream (without header) if the file is correct, else 0
*/
unsigned int check_bit_header(FILE * infile, char* message);


/*! \brief flash a bitfile bitfile
    \ingroup radhard2
    \param infile bitstream file
    \param message buffer for a return message
    \return \c 0 if this was not successful success
*/
int flash_bitfile(char* infile, char* message, char fpga='m');

#endif // RADHARD2FLASH_H
