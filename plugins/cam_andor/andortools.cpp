/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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

#include "andortools.h"
#include <QtCore>


unsigned int andorGetCameraCount() {
    at_32 lNumCameras;
    GetAvailableCameras(&lNumCameras);
    return lNumCameras;
}

bool selectCamera (int iSelectedCamera) {
    //std::cout<<"select ANDOR camera "<<iSelectedCamera<<std::endl;
    #ifdef __WINDOWS__
    long int SelectedCamera=0;
    #else
    int SelectedCamera=0;
    #endif
    GetCurrentCamera(&SelectedCamera);
    if (iSelectedCamera==SelectedCamera) return true;
    if ((iSelectedCamera < (long)andorGetCameraCount()) && (iSelectedCamera >= 0) ) {
        at_32 lCameraHandle;
        GetCameraHandle(iSelectedCamera, &lCameraHandle);
        SetCurrentCamera(lCameraHandle);
        return true;
    } else  {
        return false;
    }
}


QString andorErrorToString(unsigned int error) {
    switch (error) {
        case DRV_SUCCESS: return QObject::tr("success"); break;
        case DRV_ERROR_CODES : return QObject::tr("DRV_ERROR_CODES"); break;
        case DRV_VXDNOTINSTALLED : return QObject::tr("VxD not installed"); break;
        case DRV_ERROR_SCAN : return QObject::tr("DRV_ERROR_SCAN"); break;
        case DRV_ERROR_CHECK_SUM : return QObject::tr("DRV_ERROR_CHECK_SUM"); break;
        case DRV_ERROR_FILELOAD : return QObject::tr("DRV_ERROR_FILELOAD"); break;
        case DRV_UNKNOWN_FUNCTION : return QObject::tr("unknown function"); break;
        case DRV_ERROR_VXD_INIT : return QObject::tr("DRV_ERROR_VXD_INIT"); break;
        case DRV_ERROR_ADDRESS : return QObject::tr("DRV_ERROR_ADDRESS"); break;
        case DRV_ERROR_PAGELOCK : return QObject::tr("DRV_ERROR_PAGELOCK"); break;
        case DRV_ERROR_PAGEUNLOCK : return QObject::tr("DRV_ERROR_PAGEUNLOCK"); break;
        case DRV_ERROR_BOARDTEST : return QObject::tr("DRV_ERROR_BOARDTEST"); break;
        case DRV_ERROR_ACK : return QObject::tr("DRV_ERROR_ACK"); break;
        case DRV_ERROR_UP_FIFO : return QObject::tr("DRV_ERROR_UP_FIFO"); break;
        case DRV_ERROR_PATTERN : return QObject::tr("DRV_ERROR_PATTERN"); break;

        case DRV_ACQUISITION_ERRORS : return QObject::tr("RV_ACQUISITION_ERRORS"); break;
        case DRV_ACQ_BUFFER : return QObject::tr("DRV_ACQ_BUFFER"); break;
        case DRV_ACQ_DOWNFIFO_FULL : return QObject::tr("DRV_ACQ_DOWNFIFO_FULL"); break;
        case DRV_PROC_UNKONWN_INSTRUCTION : return QObject::tr("DRV_PROC_UNKONWN_INSTRUCTION"); break;
        case DRV_ILLEGAL_OP_CODE : return QObject::tr("illegal opcode"); break;
        case DRV_KINETIC_TIME_NOT_MET : return QObject::tr("kinetic time not met"); break;
        case DRV_ACCUM_TIME_NOT_MET : return QObject::tr("accumulation time not met"); break;
        case DRV_NO_NEW_DATA : return QObject::tr("no new data"); break;
        case DRV_SPOOLERROR : return QObject::tr("DRV_SPOOLERROR"); break;
        case DRV_SPOOLSETUPERROR : return QObject::tr("DRV_SPOOLSETUPERROR"); break;
        case DRV_FILESIZELIMITERROR : return QObject::tr("DRV_FILESIZELIMITERROR"); break;
        case DRV_ERROR_FILESAVE : return QObject::tr("DRV_ERROR_FILESAVE"); break;

        case DRV_TEMPERATURE_CODES : return QObject::tr("DRV_TEMPERATURE_CODES"); break;
        case DRV_TEMPERATURE_OFF : return QObject::tr("DRV_TEMPERATURE_OFF"); break;
        case DRV_TEMPERATURE_NOT_STABILIZED : return QObject::tr("temperature not stabilized"); break;
        case DRV_TEMPERATURE_STABILIZED : return QObject::tr("temperature stabilized"); break;
        case DRV_TEMPERATURE_NOT_REACHED : return QObject::tr("temperature not reached"); break;
        case DRV_TEMPERATURE_OUT_RANGE : return QObject::tr("temperature out of range"); break;
        case DRV_TEMPERATURE_NOT_SUPPORTED : return QObject::tr("temperature not supported"); break;
        case DRV_TEMPERATURE_DRIFT : return QObject::tr("DRV_TEMPERATURE_DRIFT"); break;


        case DRV_GENERAL_ERRORS : return QObject::tr("general error"); break;
        case DRV_INVALID_AUX : return QObject::tr("DRV_INVALID_AUX"); break;
        case DRV_COF_NOTLOADED : return QObject::tr("DRV_COF_NOTLOADED"); break;
        case DRV_FPGAPROG : return QObject::tr("DRV_FPGAPROG"); break;
        case DRV_FLEXERROR : return QObject::tr("DRV_FLEXERROR"); break;
        case DRV_GPIBERROR : return QObject::tr("GPIB error"); break;
        case DRV_EEPROMVERSIONERROR : return QObject::tr("DRV_EEPROMVERSIONERROR"); break;

        case DRV_DATATYPE : return QObject::tr("DRV_DATATYPE"); break;
        case DRV_DRIVER_ERRORS : return QObject::tr("DRV_DRIVER_ERRORS"); break;
        case DRV_P1INVALID : return QObject::tr("parameter 1 invalid"); break;
        case DRV_P2INVALID : return QObject::tr("parameter 2 invalid"); break;
        case DRV_P3INVALID : return QObject::tr("parameter 3 invalid"); break;
        case DRV_P4INVALID : return QObject::tr("parameter 4 invalid"); break;
        case DRV_INIERROR : return QObject::tr("DRV_INIERROR"); break;
        case DRV_COFERROR : return QObject::tr("DRV_COFERROR"); break;
        case DRV_ACQUIRING : return QObject::tr("DRV_ACQUIRING"); break;
        case DRV_IDLE : return QObject::tr("DRV_IDLE"); break;
        case DRV_TEMPCYCLE : return QObject::tr("DRV_TEMPCYCLE"); break;
        case DRV_NOT_INITIALIZED : return QObject::tr("system not initialized"); break;
        case DRV_P5INVALID : return QObject::tr("parameter 5 invalid"); break;
        case DRV_P6INVALID : return QObject::tr("parameter 6 invalid"); break;
        case DRV_INVALID_MODE : return QObject::tr("invalid mode"); break;
        case DRV_INVALID_FILTER : return QObject::tr("invalid filter"); break;

        case DRV_I2CERRORS : return QObject::tr("DRV_I2CERRORS"); break;
        case DRV_I2CDEVNOTFOUND : return QObject::tr("DRV_I2CDEVNOTFOUND"); break;
        case DRV_I2CTIMEOUT : return QObject::tr("DRV_I2CTIMEOUT"); break;
        case DRV_P7INVALID : return QObject::tr("DRV_P7INVALID"); break;
        case DRV_P8INVALID : return QObject::tr("DRV_P8INVALID"); break;
        case DRV_P9INVALID : return QObject::tr("DRV_P9INVALID"); break;
        case DRV_P10INVALID : return QObject::tr("DRV_P10INVALID"); break;


        case DRV_USBERROR : return QObject::tr("DRV_USBERROR"); break;
        case DRV_IOCERROR : return QObject::tr("DRV_IOCERROR"); break;
        case DRV_VRMVERSIONERROR : return QObject::tr("DRV_VRMVERSIONERROR"); break;
        case DRV_USB_INTERRUPT_ENDPOINT_ERROR : return QObject::tr("DRV_USB_INTERRUPT_ENDPOINT_ERROR"); break;
        case DRV_RANDOM_TRACK_ERROR : return QObject::tr("DRV_RANDOM_TRACK_ERROR"); break;
        case DRV_INVALID_TRIGGER_MODE : return QObject::tr("DRV_INVALID_TRIGGER_MODE"); break;
        case DRV_LOAD_FIRMWARE_ERROR : return QObject::tr("DRV_LOAD_FIRMWARE_ERROR"); break;
        case DRV_DIVIDE_BY_ZERO_ERROR : return QObject::tr("DRV_DIVIDE_BY_ZERO_ERROR"); break;
        case DRV_INVALID_RINGEXPOSURES : return QObject::tr("DRV_INVALID_RINGEXPOSURES"); break;
        case DRV_BINNING_ERROR : return QObject::tr("DRV_BINNING_ERROR"); break;
        case DRV_INVALID_AMPLIFIER : return QObject::tr("DRV_INVALID_AMPLIFIER"); break;

        case DRV_ERROR_NOCAMERA: return QObject::tr("no camera"); break;
        case DRV_NOT_SUPPORTED: return QObject::tr("not supported"); break;
        case DRV_NOT_AVAILABLE: return QObject::tr("feature not available"); break;

        case DRV_ERROR_MAP: return QObject::tr("DRV_ERROR_MAP"); break;
        case DRV_ERROR_UNMAP: return QObject::tr("DRV_ERROR_UNMAP"); break;
        case DRV_ERROR_MDL: return QObject::tr("DRV_ERROR_MDL"); break;
        case DRV_ERROR_UNMDL: return QObject::tr("DRV_ERROR_UNMDL"); break;
        case DRV_ERROR_BUFFSIZE: return QObject::tr("DRV_ERROR_BUFFSIZE"); break;
        case DRV_ERROR_NOHANDLE: return QObject::tr("DRV_ERROR_NOHANDLE"); break;

        case DRV_GATING_NOT_AVAILABLE: return QObject::tr("gating not available"); break;
        case DRV_FPGA_VOLTAGE_ERROR: return QObject::tr("FPGA voltage error"); break;

        case DRV_OW_CMD_FAIL: return QObject::tr("DRV_OW_CMD_FAIL"); break;
        case DRV_OWMEMORY_BAD_ADDR: return QObject::tr("DRV_OWMEMORY_BAD_ADDR"); break;
        case DRV_OWCMD_NOT_AVAILABLE: return QObject::tr("DRV_OWCMD_NOT_AVAILABLE"); break;
        case DRV_OW_NO_SLAVES: return QObject::tr("DRV_OW_NO_SLAVES"); break;
        case DRV_OW_NOT_INITIALIZED: return QObject::tr("DRV_OW_NOT_INITIALIZED"); break;
        case DRV_OW_ERROR_SLAVE_NUM: return QObject::tr("DRV_OW_ERROR_SLAVE_NUM"); break;
        case DRV_MSTIMINGS_ERROR: return QObject::tr("DRV_MSTIMINGS_ERROR"); break;

        case DRV_OA_NULL_ERROR: return QObject::tr("DRV_OA_NULL_ERROR"); break;
        case DRV_OA_PARSE_DTD_ERROR: return QObject::tr("DRV_OA_PARSE_DTD_ERROR"); break;
        case DRV_OA_DTD_VALIDATE_ERROR: return QObject::tr("DRV_OA_DTD_VALIDATE_ERROR"); break;
        case DRV_OA_FILE_ACCESS_ERROR: return QObject::tr("DRV_OA_FILE_ACCESS_ERROR"); break;
        case DRV_OA_FILE_DOES_NOT_EXIST: return QObject::tr("DRV_OA_FILE_DOES_NOT_EXIST"); break;
        case DRV_OA_XML_INVALID_OR_NOT_FOUND_ERROR: return QObject::tr("DRV_OA_XML_INVALID_OR_NOT_FOUND_ERROR"); break;
        case DRV_OA_PRESET_FILE_NOT_LOADED: return QObject::tr("DRV_OA_PRESET_FILE_NOT_LOADED"); break;
        case DRV_OA_USER_FILE_NOT_LOADED: return QObject::tr("DRV_OA_USER_FILE_NOT_LOADED"); break;
        case DRV_OA_PRESET_AND_USER_FILE_NOT_LOADED: return QObject::tr("DRV_OA_PRESET_AND_USER_FILE_NOT_LOADED"); break;
        case DRV_OA_INVALID_FILE: return QObject::tr("DRV_OA_INVALID_FILE"); break;
        case DRV_OA_FILE_HAS_BEEN_MODIFIED: return QObject::tr("DRV_OA_FILE_HAS_BEEN_MODIFIED"); break;
        case DRV_OA_BUFFER_FULL: return QObject::tr("DRV_OA_BUFFER_FULL"); break;
        case DRV_OA_INVALID_STRING_LENGTH: return QObject::tr("DRV_OA_INVALID_STRING_LENGTH"); break;
        case DRV_OA_INVALID_CHARS_IN_NAME: return QObject::tr("DRV_OA_INVALID_CHARS_IN_NAME"); break;
        case DRV_OA_INVALID_NAMING: return QObject::tr("DRV_OA_INVALID_NAMING"); break;
        case DRV_OA_MODE_ALREADY_EXISTS: return QObject::tr("DRV_OA_MODE_ALREADY_EXISTS"); break;
        case DRV_OA_STRINGS_NOT_EQUAL: return QObject::tr("DRV_OA_STRINGS_NOT_EQUAL"); break;
        case DRV_OA_NO_USER_DATA: return QObject::tr("DRV_OA_NO_USER_DATA"); break;
        case DRV_OA_VALUE_NOT_SUPPORTED: return QObject::tr("DRV_OA_VALUE_NOT_SUPPORTED"); break;
        case DRV_OA_MODE_DOES_NOT_EXIST: return QObject::tr("DRV_OA_MODE_DOES_NOT_EXIST"); break;

    #ifdef __LINUX__
        case DRV_PROCESSING_FAILED: return QObject::tr("processing failed"); break;
        case DRV_OA_FAILED_TO_GET_MODE: return QObject::tr("DRV_OA_FAILED_TO_GET_MODE"); break;
        case DRV_OA_CAMERA_NOT_SUPPORTED: return QObject::tr("DRV_OA_CAMERA_NOT_SUPPORTED"); break;
        case DRV_OA_GET_CAMERA_ERROR: return QObject::tr("DRV_OA_GET_CAMERA_ERROR"); break;
        case KERN_MEM_ERROR : return QObject::tr("KERN_MEM_ERROR"); break;
        case DRV_P11INVALID : return QObject::tr("DRV_P11INVALID"); break;
        case DRV_INVALID_COUNTCONVERT_MODE: return QObject::tr("DRV_INVALID_COUNTCONVERT_MODE"); break;
    #endif

        default: return QString(""); break;
    }
    return "";
}
