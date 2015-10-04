@echo off
rem Use avrdude as programming-software with the AVRProg compatible bootloader
rem Martin Thomas, 2006

rem Verfiy that the bootloader is configured with #define DEVTYPE DEVTYPE_ISP
rem since it seems that avrdude does not work with "Boot" device-types and needs
rem ISP device-types (at least in version 5.1 as in WinAVR 4/06)

set HEXFILE=cansniffer.hex
set PROGRAMMER=avr109
set PORT=com2
set BAUD=19200
set PART=atmega32

rem * disable safemode - bootloader can not "restore" fuses anyway
set DIS_SAVE=-u

avrdude %DIS_SAVE% -p %PART% -P %PORT% -c %PROGRAMMER% -b %BAUD% -v -U flash:w:%HEXFILE% 

rem pause