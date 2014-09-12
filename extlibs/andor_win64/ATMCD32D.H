
// Windows C Interface
#if !defined(__atmcd32d_h)
#define __atmcd32d_h
#pragma hdrstop

#include "windows.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef EXPNETFUNCS
#define EXPNETTYPE __declspec(dllexport)
#else
#define EXPNETTYPE __declspec(dllimport)
#endif

#define at_32 long
#define at_u32 unsigned long

#define at_64 long long
#define at_u64 unsigned long long
		
// ===================================
// Version Information Definitions
// ===================================

//Version Information Enumeration - NOTE: Increment the count constant below when
//                                        this structure is extended
// Using large numbers to force size to an integer
typedef enum {
	AT_SDKVersion = 0x40000000, 
	AT_DeviceDriverVersion = 0x40000001
} AT_VersionInfoId;

// Count of the number of elements in the Version Information Enumeration
// NOTE: Increment when extending enumeration
#define AT_NoOfVersionInfoIds 2

// Minimum recommended length of the Version Info buffer parameter
#define AT_VERSION_INFO_LEN 80
// Minimum recommended length of the Controller Card Model buffer parameter
#define AT_CONTROLLER_CARD_MODEL_LEN 80
// ===================================
		
// ===================================
// DDG Lite Definitions
// ===================================

//Channel enumeration
typedef enum {
// Using large numbers to force size to an integer
  AT_DDGLite_ChannelA = 0x40000000, 
	AT_DDGLite_ChannelB = 0x40000001, 
	AT_DDGLite_ChannelC = 0x40000002
} AT_DDGLiteChannelId;
// Control byte flags
#define AT_DDGLite_ControlBit_GlobalEnable   0x01

#define AT_DDGLite_ControlBit_ChannelEnable  0x01
#define AT_DDGLite_ControlBit_FreeRun        0x02
#define AT_DDGLite_ControlBit_DisableOnFrame 0x04
#define AT_DDGLite_ControlBit_RestartOnFire  0x08
#define AT_DDGLite_ControlBit_Invert         0x10
#define AT_DDGLite_ControlBit_EnableOnFire   0x20
// ===================================

// ===================================
// USB iStar Definitions
// ===================================

#define AT_DDG_POLARITY_POSITIVE  0
#define AT_DDG_POLARITY_NEGATIVE  1
#define AT_DDG_TERMINATION_50OHMS 0
#define AT_DDG_TERMINATION_HIGHZ  1

#define AT_STEPMODE_CONSTANT      0
#define AT_STEPMODE_EXPONENTIAL   1
#define AT_STEPMODE_LOGARITHMIC   2
#define AT_STEPMODE_LINEAR        3
#define AT_STEPMODE_OFF           100

#define AT_GATEMODE_FIRE_AND_GATE 0
#define AT_GATEMODE_FIRE_ONLY     1
#define AT_GATEMODE_GATE_ONLY     2
#define AT_GATEMODE_CW_ON         3
#define AT_GATEMODE_CW_OFF        4
#define AT_GATEMODE_DDG           5
// ===================================
	
typedef struct ANDORCAPS
{
  ULONG ulSize;
	ULONG ulAcqModes;
	ULONG ulReadModes;
	ULONG ulTriggerModes;
	ULONG ulCameraType;
	ULONG ulPixelMode;
	ULONG ulSetFunctions;
	ULONG ulGetFunctions;
	ULONG ulFeatures;
	ULONG ulPCICard;
	ULONG ulEMGainCapability;
	ULONG ulFTReadModes;
} AndorCapabilities;
	
typedef struct COLORDEMOSAICINFO
{
  int iX;
	int iY;
	int iAlgorithm;
	int iXPhase;
	int iYPhase;
	int iBackground;
} ColorDemosaicInfo;
	
typedef struct WHITEBALANCEINFO
{
  int iSize;
	int iX;
	int iY;
	int iAlgorithm;
	int iROI_left;
	int iROI_right;
	int iROI_top;
	int iROI_bottom;
	int iOperation;
} WhiteBalanceInfo;
	
EXPNETTYPE unsigned int WINAPI AbortAcquisition(void);
EXPNETTYPE unsigned int WINAPI CancelWait(void);
EXPNETTYPE unsigned int WINAPI CoolerOFF(void);
EXPNETTYPE unsigned int WINAPI CoolerON(void);
EXPNETTYPE unsigned int WINAPI DemosaicImage(WORD * grey, WORD * red, WORD * green, WORD * blue, ColorDemosaicInfo * info);
EXPNETTYPE unsigned int WINAPI EnableKeepCleans(int iMode);
EXPNETTYPE unsigned int WINAPI FreeInternalMemory(void);
EXPNETTYPE unsigned int WINAPI GetAcquiredData(at_32 * arr, unsigned long size);
EXPNETTYPE unsigned int WINAPI GetAcquiredData16(WORD * arr, unsigned long size);
EXPNETTYPE unsigned int WINAPI GetAcquiredFloatData(float * arr, unsigned long size);
EXPNETTYPE unsigned int WINAPI GetAcquisitionProgress(long * acc, long * series);
EXPNETTYPE unsigned int WINAPI GetAcquisitionTimings(float * exposure, float * accumulate, float * kinetic);
EXPNETTYPE unsigned int WINAPI GetAdjustedRingExposureTimes(int inumTimes, float * fptimes);
EXPNETTYPE unsigned int WINAPI GetAllDMAData(at_32 * arr, unsigned long size);
EXPNETTYPE unsigned int WINAPI GetAmpDesc(int index, char * name, int length);
EXPNETTYPE unsigned int WINAPI GetAmpMaxSpeed(int index, float * speed);
EXPNETTYPE unsigned int WINAPI GetAvailableCameras(long * totalCameras);
EXPNETTYPE unsigned int WINAPI GetBackground(at_32 * arr, unsigned long size);
EXPNETTYPE unsigned int WINAPI GetBitDepth(int channel, int * depth);
EXPNETTYPE unsigned int WINAPI GetCameraEventStatus(DWORD * camStatus);
EXPNETTYPE unsigned int WINAPI GetCameraHandle(long cameraIndex, long * cameraHandle);
EXPNETTYPE unsigned int WINAPI GetCameraInformation(int index, long * information);
EXPNETTYPE unsigned int WINAPI GetCameraSerialNumber(int * number);
EXPNETTYPE unsigned int WINAPI GetCapabilities(AndorCapabilities * caps);
EXPNETTYPE unsigned int WINAPI GetControllerCardModel(char * controllerCardModel);
EXPNETTYPE unsigned int WINAPI GetCurrentCamera(long * cameraHandle);
EXPNETTYPE unsigned int WINAPI GetCYMGShift(int * iXshift, int * iYShift);
EXPNETTYPE unsigned int WINAPI GetDDGExternalOutputEnabled(at_u32 uiIndex, at_u32 * puiEnabled);
EXPNETTYPE unsigned int WINAPI GetDDGExternalOutputPolarity(at_u32 uiIndex, at_u32 * puiPolarity);
EXPNETTYPE unsigned int WINAPI GetDDGExternalOutputStepEnabled(at_u32 uiIndex, at_u32 * puiEnabled);
EXPNETTYPE unsigned int WINAPI GetDDGExternalOutputTime(at_u32 uiIndex, at_u64 * puiDelay, at_u64 * puiWidth);
EXPNETTYPE unsigned int WINAPI GetDDGGateTime(at_u64 * puiDelay, at_u64 * puiWidth);
EXPNETTYPE unsigned int WINAPI GetDDGInsertionDelay(int * piState);
EXPNETTYPE unsigned int WINAPI GetDDGIntelligate(int * piState);
EXPNETTYPE unsigned int WINAPI GetDDGIOC(int * state);
EXPNETTYPE unsigned int WINAPI GetDDGIOCFrequency(double * frequency);
EXPNETTYPE unsigned int WINAPI GetDDGIOCNumber(unsigned long * numberPulses);
EXPNETTYPE unsigned int WINAPI GetDDGIOCNumberRequested(at_u32 * pulses);
EXPNETTYPE unsigned int WINAPI GetDDGIOCPeriod(at_u64 * period);
EXPNETTYPE unsigned int WINAPI GetDDGIOCPulses(int * pulses);

// DDG Lite functions
EXPNETTYPE unsigned int WINAPI GetDDGLiteGlobalControlByte(unsigned char * control);
EXPNETTYPE unsigned int WINAPI GetDDGLiteControlByte(AT_DDGLiteChannelId channel, unsigned char * control);
EXPNETTYPE unsigned int WINAPI GetDDGLiteInitialDelay(AT_DDGLiteChannelId channel, float * fDelay);
EXPNETTYPE unsigned int WINAPI GetDDGLitePulseWidth(AT_DDGLiteChannelId channel, float * fWidth);
EXPNETTYPE unsigned int WINAPI GetDDGLiteInterPulseDelay(AT_DDGLiteChannelId channel, float * fDelay);
EXPNETTYPE unsigned int WINAPI GetDDGLitePulsesPerExposure(AT_DDGLiteChannelId channel, at_u32 * ui32Pulses);

EXPNETTYPE unsigned int WINAPI GetDDGPulse(double wid, double resolution, double * Delay, double * Width);
EXPNETTYPE unsigned int WINAPI GetDDGStepCoefficients(at_u32 mode, double * p1, double * p2);
EXPNETTYPE unsigned int WINAPI GetDDGStepMode(at_u32 * mode);
EXPNETTYPE unsigned int WINAPI GetDetector(int * xpixels, int * ypixels);
EXPNETTYPE unsigned int WINAPI GetDICameraInfo(void * info);
EXPNETTYPE unsigned int WINAPI GetEMCCDGain(int * gain);
EXPNETTYPE unsigned int WINAPI GetEMGainRange(int * low, int * high);
EXPNETTYPE unsigned int WINAPI GetExternalTriggerTermination(at_u32 * puiTermination);
EXPNETTYPE unsigned int WINAPI GetFastestRecommendedVSSpeed(int * index, float * speed);
EXPNETTYPE unsigned int WINAPI GetFIFOUsage(int * FIFOusage);
EXPNETTYPE unsigned int WINAPI GetFilterMode(int * mode);
EXPNETTYPE unsigned int WINAPI GetFKExposureTime(float * time);
EXPNETTYPE unsigned int WINAPI GetFKVShiftSpeed(int index, int * speed);
EXPNETTYPE unsigned int WINAPI GetFKVShiftSpeedF(int index, float * speed);
EXPNETTYPE unsigned int WINAPI GetFrontEndStatus(int * piFlag);
EXPNETTYPE unsigned int WINAPI GetGateMode(int * piGatemode);
EXPNETTYPE unsigned int WINAPI GetHardwareVersion(unsigned int * PCB, unsigned int * Decode, unsigned int * dummy1, unsigned int * dummy2, unsigned int * CameraFirmwareVersion, unsigned int * CameraFirmwareBuild);
EXPNETTYPE unsigned int WINAPI GetHeadModel(char * name);
EXPNETTYPE unsigned int WINAPI GetHorizontalSpeed(int index, int * speed);
EXPNETTYPE unsigned int WINAPI GetHSSpeed(int channel, int typ, int index, float * speed);
EXPNETTYPE unsigned int WINAPI GetHVflag(int * bFlag);
EXPNETTYPE unsigned int WINAPI GetID(int devNum, int * id);
EXPNETTYPE unsigned int WINAPI GetImageFlip(int * iHFlip, int * iVFlip);
EXPNETTYPE unsigned int WINAPI GetImageRotate(int * iRotate);
EXPNETTYPE unsigned int WINAPI GetImages(long first, long last, at_32 * arr, unsigned long size, long * validfirst, long * validlast);
EXPNETTYPE unsigned int WINAPI GetImages16(long first, long last, WORD * arr, unsigned long size, long * validfirst, long * validlast);
EXPNETTYPE unsigned int WINAPI GetImagesPerDMA(unsigned long * images);
EXPNETTYPE unsigned int WINAPI GetIRQ(int * IRQ);
EXPNETTYPE unsigned int WINAPI GetKeepCleanTime(float * KeepCleanTime);
EXPNETTYPE unsigned int WINAPI GetMaximumBinning(int ReadMode, int HorzVert, int * MaxBinning);
EXPNETTYPE unsigned int WINAPI GetMaximumExposure(float * MaxExp);
EXPNETTYPE unsigned int WINAPI GetMCPGain(int * piGain);
EXPNETTYPE unsigned int WINAPI GetMCPGainRange(int * iLow, int * iHigh);
EXPNETTYPE unsigned int WINAPI GetMCPGainTable(int iNum, int * piGain, float * pfPhotoepc);
EXPNETTYPE unsigned int WINAPI GetMCPVoltage(int * iVoltage);
EXPNETTYPE unsigned int WINAPI GetMinimumImageLength(int * MinImageLength);
EXPNETTYPE unsigned int WINAPI GetMinimumNumberInSeries(int * number);
EXPNETTYPE unsigned int WINAPI GetMostRecentColorImage16(unsigned long size, int algorithm, WORD * red, WORD * green, WORD * blue);
EXPNETTYPE unsigned int WINAPI GetMostRecentImage(at_32 * arr, unsigned long size);
EXPNETTYPE unsigned int WINAPI GetMostRecentImage16(WORD * arr, unsigned long size);
EXPNETTYPE unsigned int WINAPI GetMSTimingsData(SYSTEMTIME * TimeOfStart, float * pfDifferences, int inoOfImages);
EXPNETTYPE unsigned int WINAPI GetMetaDataInfo(SYSTEMTIME * TimeOfStart, float * pfTimeFromStart, unsigned int index);
EXPNETTYPE unsigned int WINAPI GetMSTimingsEnabled(void);
EXPNETTYPE unsigned int WINAPI GetNewData(at_32 * arr, unsigned long size);
EXPNETTYPE unsigned int WINAPI GetNewData16(WORD * arr, unsigned long size);
EXPNETTYPE unsigned int WINAPI GetNewData8(unsigned char * arr, unsigned long size);
EXPNETTYPE unsigned int WINAPI GetNewFloatData(float * arr, unsigned long size);
EXPNETTYPE unsigned int WINAPI GetNumberADChannels(int * channels);
EXPNETTYPE unsigned int WINAPI GetNumberAmp(int * amp);
EXPNETTYPE unsigned int WINAPI GetNumberAvailableImages(at_32 * first, at_32 * last);
EXPNETTYPE unsigned int WINAPI GetNumberDDGExternalOutputs(at_u32 * puiCount);
EXPNETTYPE unsigned int WINAPI GetNumberDevices(int * numDevs);
EXPNETTYPE unsigned int WINAPI GetNumberFKVShiftSpeeds(int * number);
EXPNETTYPE unsigned int WINAPI GetNumberHorizontalSpeeds(int * number);
EXPNETTYPE unsigned int WINAPI GetNumberHSSpeeds(int channel, int typ, int * speeds);
EXPNETTYPE unsigned int WINAPI GetNumberNewImages(long * first, long * last);
EXPNETTYPE unsigned int WINAPI GetNumberPreAmpGains(int * noGains);
EXPNETTYPE unsigned int WINAPI GetNumberRingExposureTimes(int * ipnumTimes);
EXPNETTYPE unsigned int WINAPI GetNumberIO(int * iNumber);
EXPNETTYPE unsigned int WINAPI GetNumberVerticalSpeeds(int * number);
EXPNETTYPE unsigned int WINAPI GetNumberVSAmplitudes(int * number);
EXPNETTYPE unsigned int WINAPI GetNumberVSSpeeds(int * speeds);
EXPNETTYPE unsigned int WINAPI GetOldestImage(at_32 * arr, unsigned long size);
EXPNETTYPE unsigned int WINAPI GetOldestImage16(WORD * arr, unsigned long size);
EXPNETTYPE unsigned int WINAPI GetPhosphorStatus(int * piFlag);
EXPNETTYPE unsigned int WINAPI GetPhysicalDMAAddress(unsigned long * Address1, unsigned long * Address2);
EXPNETTYPE unsigned int WINAPI GetPixelSize(float * xSize, float * ySize);
EXPNETTYPE unsigned int WINAPI GetPreAmpGain(int index, float * gain);
EXPNETTYPE unsigned int WINAPI GetDualExposureTimes(float * exposure1, float * exposure2);
EXPNETTYPE unsigned int WINAPI GetQE(char * sensor, float wavelength, float * QE);
EXPNETTYPE unsigned int WINAPI GetReadOutTime(float * ReadOutTime);
EXPNETTYPE unsigned int WINAPI GetRegisterDump(int * mode);
EXPNETTYPE unsigned int WINAPI GetRingExposureRange(float * fpMin, float * fpMax);
EXPNETTYPE unsigned int WINAPI GetSensitivity(int channel, int horzShift, int amplifier, int pa, float * sensitivity);
EXPNETTYPE unsigned int WINAPI GetSizeOfCircularBuffer(long * index);
EXPNETTYPE unsigned int WINAPI GetSlotBusDeviceFunction(DWORD * dwslot, DWORD * dwBus, DWORD * dwDevice, DWORD * dwFunction);
EXPNETTYPE unsigned int WINAPI GetSoftwareVersion(unsigned int * eprom, unsigned int * coffile, unsigned int * vxdrev, unsigned int * vxdver, unsigned int * dllrev, unsigned int * dllver);
EXPNETTYPE unsigned int WINAPI GetSpoolProgress(long * index);
EXPNETTYPE unsigned int WINAPI GetStartUpTime(float * time);
EXPNETTYPE unsigned int WINAPI GetStatus(int * status);
EXPNETTYPE unsigned int WINAPI GetTECStatus(int * piFlag);
EXPNETTYPE unsigned int WINAPI GetTemperature(int * temperature);
EXPNETTYPE unsigned int WINAPI GetTemperatureF(float * temperature);
EXPNETTYPE unsigned int WINAPI GetTemperatureRange(int * mintemp, int * maxtemp);
EXPNETTYPE unsigned int WINAPI GetTemperatureStatus(float * SensorTemp, float * TargetTemp, float * AmbientTemp, float * CoolerVolts);
EXPNETTYPE unsigned int WINAPI GetTotalNumberImagesAcquired(long * index);
EXPNETTYPE unsigned int WINAPI GetIODirection(int index, int * iDirection);
EXPNETTYPE unsigned int WINAPI GetIOLevel(int index, int * iLevel);
EXPNETTYPE unsigned int WINAPI GetVersionInfo(AT_VersionInfoId arr, char * szVersionInfo, at_u32 ui32BufferLen);
EXPNETTYPE unsigned int WINAPI GetVerticalSpeed(int index, int * speed);
EXPNETTYPE unsigned int WINAPI GetVirtualDMAAddress(void ** Address1, void ** Address2);
EXPNETTYPE unsigned int WINAPI GetVSSpeed(int index, float * speed);
EXPNETTYPE unsigned int WINAPI GPIBReceive(int id, short address, char * text, int size);
EXPNETTYPE unsigned int WINAPI GPIBSend(int id, short address, char * text);
EXPNETTYPE unsigned int WINAPI I2CBurstRead(BYTE i2cAddress, long nBytes, BYTE * data);
EXPNETTYPE unsigned int WINAPI I2CBurstWrite(BYTE i2cAddress, long nBytes, BYTE * data);
EXPNETTYPE unsigned int WINAPI I2CRead(BYTE deviceID, BYTE intAddress, BYTE * pdata);
EXPNETTYPE unsigned int WINAPI I2CReset(void);
EXPNETTYPE unsigned int WINAPI I2CWrite(BYTE deviceID, BYTE intAddress, BYTE data);
EXPNETTYPE unsigned int WINAPI IdAndorDll(void);
EXPNETTYPE unsigned int WINAPI InAuxPort(int port, int * state);
EXPNETTYPE unsigned int WINAPI Initialize(char * dir);
EXPNETTYPE unsigned int WINAPI InitializeDevice(char * dir);
EXPNETTYPE unsigned int WINAPI IsAmplifierAvailable(int iamp);
EXPNETTYPE unsigned int WINAPI IsCoolerOn(int * iCoolerStatus);
EXPNETTYPE unsigned int WINAPI IsInternalMechanicalShutter(int * InternalShutter);
EXPNETTYPE unsigned int WINAPI IsPreAmpGainAvailable(int channel, int amplifier, int index, int pa, int * status);
EXPNETTYPE unsigned int WINAPI IsTriggerModeAvailable(int iTriggerMode);
EXPNETTYPE unsigned int WINAPI Merge(const at_32 * arr, long nOrder, long nPoint, long nPixel, float * coeff, long fit, long hbin, at_32 * output, float * start, float * step_Renamed);
EXPNETTYPE unsigned int WINAPI OutAuxPort(int port, int state);
EXPNETTYPE unsigned int WINAPI PrepareAcquisition(void);
EXPNETTYPE unsigned int WINAPI SaveAsBmp(char * path, char * palette, long ymin, long ymax);
EXPNETTYPE unsigned int WINAPI SaveAsCommentedSif(char * path, char * comment);
EXPNETTYPE unsigned int WINAPI SaveAsEDF(char * szPath, int iMode);
EXPNETTYPE unsigned int WINAPI SaveAsFITS(char * szFileTitle, int typ);
EXPNETTYPE unsigned int WINAPI SaveAsRaw(char * szFileTitle, int typ);
EXPNETTYPE unsigned int WINAPI SaveAsSif(char * path);
EXPNETTYPE unsigned int WINAPI SaveAsSPC(char * path);
EXPNETTYPE unsigned int WINAPI SaveAsTiff(char * path, char * palette, int position, int typ);
EXPNETTYPE unsigned int WINAPI SaveAsTiffEx(char * path, char * palette, int position, int typ, int mode);
EXPNETTYPE unsigned int WINAPI SaveEEPROMToFile(char * cFileName);
EXPNETTYPE unsigned int WINAPI SaveToClipBoard(char * palette);
EXPNETTYPE unsigned int WINAPI SelectDevice(int devNum);
EXPNETTYPE unsigned int WINAPI SendSoftwareTrigger(void);
EXPNETTYPE unsigned int WINAPI SetAccumulationCycleTime(float time);
EXPNETTYPE unsigned int WINAPI SetAcqStatusEvent(HANDLE statusEvent);
EXPNETTYPE unsigned int WINAPI SetAcquisitionMode(int mode);
EXPNETTYPE unsigned int WINAPI SetAcquisitionType(int typ);
EXPNETTYPE unsigned int WINAPI SetADChannel(int channel);
EXPNETTYPE unsigned int WINAPI SetAdvancedTriggerModeState(int iState);
EXPNETTYPE unsigned int WINAPI SetBackground(at_32 * arr, unsigned long size);
EXPNETTYPE unsigned int WINAPI SetBaselineClamp(int state);
EXPNETTYPE unsigned int WINAPI SetBaselineOffset(int offset);
EXPNETTYPE unsigned int WINAPI SetCameraStatusEnable(DWORD Enable);
EXPNETTYPE unsigned int WINAPI SetComplexImage(int numAreas, int * areas);
EXPNETTYPE unsigned int WINAPI SetCoolerMode(int mode);
EXPNETTYPE unsigned int WINAPI SetCountConvertMode(int Mode);
EXPNETTYPE unsigned int WINAPI SetCountConvertWavelength(float wavelength);
EXPNETTYPE unsigned int WINAPI SetCropMode(int active, int cropHeight, int reserved);
EXPNETTYPE unsigned int WINAPI SetCurrentCamera(long cameraHandle);
EXPNETTYPE unsigned int WINAPI SetCustomTrackHBin(int bin);
EXPNETTYPE unsigned int WINAPI SetDataType(int typ);
EXPNETTYPE unsigned int WINAPI SetDACOutput(int iOption, int iResolution, int iValue);
EXPNETTYPE unsigned int WINAPI SetDACOutputScale(int iScale);
EXPNETTYPE unsigned int WINAPI SetDDGAddress(BYTE t0, BYTE t1, BYTE t2, BYTE t3, BYTE address);
EXPNETTYPE unsigned int WINAPI SetDDGExternalOutputEnabled(at_u32 uiIndex, at_u32 uiEnabled);
EXPNETTYPE unsigned int WINAPI SetDDGExternalOutputPolarity(at_u32 uiIndex, at_u32 uiPolarity);
EXPNETTYPE unsigned int WINAPI SetDDGExternalOutputStepEnabled(at_u32 uiIndex, at_u32 uiEnabled);
EXPNETTYPE unsigned int WINAPI SetDDGExternalOutputTime(at_u32 uiIndex, at_u64 uiDelay, at_u64 uiWidth);
EXPNETTYPE unsigned int WINAPI SetDDGGain(int gain);
EXPNETTYPE unsigned int WINAPI SetDDGGateStep(double step_Renamed);
EXPNETTYPE unsigned int WINAPI SetDDGGateTime(at_u64 uiDelay, at_u64 uiWidth);
EXPNETTYPE unsigned int WINAPI SetDDGInsertionDelay(int state);
EXPNETTYPE unsigned int WINAPI SetDDGIntelligate(int state);
EXPNETTYPE unsigned int WINAPI SetDDGIOC(int state);
EXPNETTYPE unsigned int WINAPI SetDDGIOCFrequency(double frequency);
EXPNETTYPE unsigned int WINAPI SetDDGIOCNumber(unsigned long numberPulses);
EXPNETTYPE unsigned int WINAPI SetDDGIOCPeriod(at_u64 period);

// DDG Lite functions
EXPNETTYPE unsigned int WINAPI SetDDGLiteGlobalControlByte(unsigned char control);
EXPNETTYPE unsigned int WINAPI SetDDGLiteControlByte(AT_DDGLiteChannelId channel, unsigned char control);
EXPNETTYPE unsigned int WINAPI SetDDGLiteInitialDelay(AT_DDGLiteChannelId channel, float fDelay);
EXPNETTYPE unsigned int WINAPI SetDDGLitePulseWidth(AT_DDGLiteChannelId channel, float fWidth);
EXPNETTYPE unsigned int WINAPI SetDDGLiteInterPulseDelay(AT_DDGLiteChannelId channel, float fDelay);
EXPNETTYPE unsigned int WINAPI SetDDGLitePulsesPerExposure(AT_DDGLiteChannelId channel, at_u32 ui32Pulses);

EXPNETTYPE unsigned int WINAPI SetDDGStepCoefficients(at_u32 mode, double p1, double p2);
EXPNETTYPE unsigned int WINAPI SetDDGStepMode(at_u32 mode);
EXPNETTYPE unsigned int WINAPI SetDDGTimes(double t0, double t1, double t2);
EXPNETTYPE unsigned int WINAPI SetDDGTriggerMode(int mode);
EXPNETTYPE unsigned int WINAPI SetDDGVariableGateStep(int mode, double p1, double p2);
EXPNETTYPE unsigned int WINAPI SetDelayGenerator(int board, short address, int typ);
EXPNETTYPE unsigned int WINAPI SetDMAParameters(int MaxImagesPerDMA, float SecondsPerDMA);
EXPNETTYPE unsigned int WINAPI SetDriverEvent(HANDLE driverEvent);
EXPNETTYPE unsigned int WINAPI SetEMAdvanced(int state);
EXPNETTYPE unsigned int WINAPI SetEMCCDGain(int gain);
EXPNETTYPE unsigned int WINAPI SetEMClockCompensation(int EMClockCompensationFlag);
EXPNETTYPE unsigned int WINAPI SetEMGainMode(int mode);
EXPNETTYPE unsigned int WINAPI SetExposureTime(float time);
EXPNETTYPE unsigned int WINAPI SetExternalTriggerTermination(at_u32 uiTermination);
EXPNETTYPE unsigned int WINAPI SetFanMode(int mode);
EXPNETTYPE unsigned int WINAPI SetFastExtTrigger(int mode);
EXPNETTYPE unsigned int WINAPI SetFastKinetics(int exposedRows, int seriesLength, float time, int mode, int hbin, int vbin);
EXPNETTYPE unsigned int WINAPI SetFastKineticsEx(int exposedRows, int seriesLength, float time, int mode, int hbin, int vbin, int offset);
EXPNETTYPE unsigned int WINAPI SetFilterMode(int mode);
EXPNETTYPE unsigned int WINAPI SetFilterParameters(int width, float sensitivity, int range, float accept, int smooth, int noise);
EXPNETTYPE unsigned int WINAPI SetFKVShiftSpeed(int index);
EXPNETTYPE unsigned int WINAPI SetFPDP(int state);
EXPNETTYPE unsigned int WINAPI SetFrameTransferMode(int mode);
EXPNETTYPE unsigned int WINAPI SetFrontEndEvent(HANDLE driverEvent);
EXPNETTYPE unsigned int WINAPI SetFullImage(int hbin, int vbin);
EXPNETTYPE unsigned int WINAPI SetFVBHBin(int bin);
EXPNETTYPE unsigned int WINAPI SetGain(int gain);
EXPNETTYPE unsigned int WINAPI SetGate(float delay, float width, float stepRenamed);
EXPNETTYPE unsigned int WINAPI SetGateMode(int gatemode);
EXPNETTYPE unsigned int WINAPI SetHighCapacity(int state);
EXPNETTYPE unsigned int WINAPI SetHorizontalSpeed(int index);
EXPNETTYPE unsigned int WINAPI SetHSSpeed(int typ, int index);
EXPNETTYPE unsigned int WINAPI SetImage(int hbin, int vbin, int hstart, int hend, int vstart, int vend);
EXPNETTYPE unsigned int WINAPI SetImageFlip(int iHFlip, int iVFlip);
EXPNETTYPE unsigned int WINAPI SetImageRotate(int iRotate);
EXPNETTYPE unsigned int WINAPI SetIsolatedCropMode(int active, int cropheight, int cropwidth, int vbin, int hbin);
EXPNETTYPE unsigned int WINAPI SetKineticCycleTime(float time);
EXPNETTYPE unsigned int WINAPI SetMCPGain(int gain);
EXPNETTYPE unsigned int WINAPI SetMCPGating(int gating);
EXPNETTYPE unsigned int WINAPI SetMessageWindow(HWND wnd);
EXPNETTYPE unsigned int WINAPI SetMetaData(int state);
EXPNETTYPE unsigned int WINAPI SetMultiTrack(int number, int height, int offset, int * bottom, int * gap);
EXPNETTYPE unsigned int WINAPI SetMultiTrackHBin(int bin);
EXPNETTYPE unsigned int WINAPI SetMultiTrackHRange(int iStart, int iEnd);
EXPNETTYPE unsigned int WINAPI SetMultiTrackScan(int trackHeight, int numberTracks, int iSIHStart, int iSIHEnd, int trackHBinning, int trackVBinning, int trackGap, int trackOffset, int trackSkip, int numberSubFrames);
EXPNETTYPE unsigned int WINAPI SetNextAddress(at_32 * data, long lowAdd, long highAdd, long length, long physical);
EXPNETTYPE unsigned int WINAPI SetNextAddress16(at_32 * data, long lowAdd, long highAdd, long length, long physical);
EXPNETTYPE unsigned int WINAPI SetNumberAccumulations(int number);
EXPNETTYPE unsigned int WINAPI SetNumberKinetics(int number);
EXPNETTYPE unsigned int WINAPI SetNumberPrescans(int iNumber);
EXPNETTYPE unsigned int WINAPI SetOutputAmplifier(int typ);
EXPNETTYPE unsigned int WINAPI SetOverlapMode(int mode);
EXPNETTYPE unsigned int WINAPI SetPCIMode(int mode, int value);
EXPNETTYPE unsigned int WINAPI SetPhotonCounting(int state);
EXPNETTYPE unsigned int WINAPI SetPhotonCountingThreshold(long min, long max);
EXPNETTYPE unsigned int WINAPI SetPhosphorEvent(HANDLE driverEvent);
EXPNETTYPE unsigned int WINAPI SetPhotonCountingDivisions(at_u32 noOfDivisions, at_32 * divisions);
EXPNETTYPE unsigned int WINAPI SetPixelMode(int bitdepth, int colormode);
EXPNETTYPE unsigned int WINAPI SetPreAmpGain(int index);
EXPNETTYPE unsigned int WINAPI SetDualExposureTimes(float expTime1, float expTime2);
EXPNETTYPE unsigned int WINAPI SetDualExposureMode(int mode);
EXPNETTYPE unsigned int WINAPI SetRandomTracks(int numTracks, int * areas);
EXPNETTYPE unsigned int WINAPI SetReadMode(int mode);
EXPNETTYPE unsigned int WINAPI SetRegisterDump(int mode);
EXPNETTYPE unsigned int WINAPI SetRingExposureTimes(int numTimes, float * times);
EXPNETTYPE unsigned int WINAPI SetSaturationEvent(HANDLE saturationEvent);
EXPNETTYPE unsigned int WINAPI SetShutter(int typ, int mode, int closingtime, int openingtime);
EXPNETTYPE unsigned int WINAPI SetShutterEx(int typ, int mode, int closingtime, int openingtime, int extmode);
EXPNETTYPE unsigned int WINAPI SetShutters(int typ, int mode, int closingtime, int openingtime, int exttype, int extmode, int dummy1, int dummy2);
EXPNETTYPE unsigned int WINAPI SetSifComment(char * comment);
EXPNETTYPE unsigned int WINAPI SetSingleTrack(int centre, int height);
EXPNETTYPE unsigned int WINAPI SetSingleTrackHBin(int bin);
EXPNETTYPE unsigned int WINAPI SetSpool(int active, int method, char * path, int framebuffersize);
EXPNETTYPE unsigned int WINAPI SetStorageMode(long mode);
EXPNETTYPE unsigned int WINAPI SetTECEvent(HANDLE driverEvent);
EXPNETTYPE unsigned int WINAPI SetTemperature(int temperature);
EXPNETTYPE unsigned int WINAPI SetTemperatureEvent(HANDLE temperatureEvent);
EXPNETTYPE unsigned int WINAPI SetTriggerMode(int mode);
EXPNETTYPE unsigned int WINAPI SetTriggerInvert(int mode);
EXPNETTYPE unsigned int WINAPI GetTriggerLevelRange(float * minimum, float * maximum);
EXPNETTYPE unsigned int WINAPI SetTriggerLevel(float f_level);
EXPNETTYPE unsigned int WINAPI SetIODirection(int index, int iDirection);
EXPNETTYPE unsigned int WINAPI SetIOLevel(int index, int iLevel);
EXPNETTYPE unsigned int WINAPI SetUserEvent(HANDLE userEvent);
EXPNETTYPE unsigned int WINAPI SetUSGenomics(long width, long height);
EXPNETTYPE unsigned int WINAPI SetVerticalRowBuffer(int rows);
EXPNETTYPE unsigned int WINAPI SetVerticalSpeed(int index);
EXPNETTYPE unsigned int WINAPI SetVirtualChip(int state);
EXPNETTYPE unsigned int WINAPI SetVSAmplitude(int index);
EXPNETTYPE unsigned int WINAPI SetVSSpeed(int index);
EXPNETTYPE unsigned int WINAPI ShutDown(void);
EXPNETTYPE unsigned int WINAPI StartAcquisition(void);
EXPNETTYPE unsigned int WINAPI UnMapPhysicalAddress(void);
EXPNETTYPE unsigned int WINAPI WaitForAcquisition(void);
EXPNETTYPE unsigned int WINAPI WaitForAcquisitionByHandle(long cameraHandle);
EXPNETTYPE unsigned int WINAPI WaitForAcquisitionByHandleTimeOut(long cameraHandle, int iTimeOutMs);
EXPNETTYPE unsigned int WINAPI WaitForAcquisitionTimeOut(int iTimeOutMs);
EXPNETTYPE unsigned int WINAPI WhiteBalance(WORD * wRed, WORD * wGreen, WORD * wBlue, float * fRelR, float * fRelB, WhiteBalanceInfo * info);

EXPNETTYPE unsigned int WINAPI OA_Initialize(const char * const pcFilename, unsigned int uiFileNameLen);
EXPNETTYPE unsigned int WINAPI OA_EnableMode(const char * const pcModeName);
EXPNETTYPE unsigned int WINAPI OA_GetModeAcqParams(const char * const pcModeName, char ** const ppcListOfParams);
EXPNETTYPE unsigned int WINAPI OA_GetUserModeNames(char ** ppcListOfModes);
EXPNETTYPE unsigned int WINAPI OA_GetPreSetModeNames(char ** ppcListOfModes);
EXPNETTYPE unsigned int WINAPI OA_GetNumberOfUserModes(unsigned int * const puiNumberOfModes);
EXPNETTYPE unsigned int WINAPI OA_GetNumberOfPreSetModes(unsigned int * const puiNumberOfModes);
EXPNETTYPE unsigned int WINAPI OA_GetNumberOfAcqParams(const char * const pcModeName, unsigned int * const puiNumberOfParams);
EXPNETTYPE unsigned int WINAPI OA_AddMode(char * pcModeName, unsigned int uiModeNameLen, char * pcModeDescription, unsigned int uiModeDescriptionLen);
EXPNETTYPE unsigned int WINAPI OA_WriteToFile(const char * const pcFileName, unsigned int uiFileNameLen);
EXPNETTYPE unsigned int WINAPI OA_DeleteMode(const char * const pcModeName, unsigned int uiModeNameLen);
EXPNETTYPE unsigned int WINAPI OA_SetInt(const char * const pcModeName, const char * pcModeParam, const int iIntValue);
EXPNETTYPE unsigned int WINAPI OA_SetFloat(const char * const pcModeName, const char * pcModeParam, const float fFloatValue);
EXPNETTYPE unsigned int WINAPI OA_SetString(const char * const pcModeName, const char * pcModeParam, char * pcStringValue, const unsigned int uiStringLen);
EXPNETTYPE unsigned int WINAPI OA_GetInt(const char * const pcModeName, const char * const pcModeParam, int * iIntValue);
EXPNETTYPE unsigned int WINAPI OA_GetFloat(const char * const pcModeName, const char * const pcModeParam, float * fFloatValue);
EXPNETTYPE unsigned int WINAPI OA_GetString(const char * const pcModeName, const char * const pcModeParam, char * pcStringValue, const unsigned int uiStringLen);

#define DRV_ERROR_CODES 20001
#define DRV_SUCCESS 20002
#define DRV_VXDNOTINSTALLED 20003
#define DRV_ERROR_SCAN 20004
#define DRV_ERROR_CHECK_SUM 20005
#define DRV_ERROR_FILELOAD 20006
#define DRV_UNKNOWN_FUNCTION 20007
#define DRV_ERROR_VXD_INIT 20008
#define DRV_ERROR_ADDRESS 20009
#define DRV_ERROR_PAGELOCK 20010
#define DRV_ERROR_PAGEUNLOCK 20011
#define DRV_ERROR_BOARDTEST 20012
#define DRV_ERROR_ACK 20013
#define DRV_ERROR_UP_FIFO 20014
#define DRV_ERROR_PATTERN 20015

#define DRV_ACQUISITION_ERRORS 20017
#define DRV_ACQ_BUFFER 20018
#define DRV_ACQ_DOWNFIFO_FULL 20019
#define DRV_PROC_UNKONWN_INSTRUCTION 20020
#define DRV_ILLEGAL_OP_CODE 20021
#define DRV_KINETIC_TIME_NOT_MET 20022
#define DRV_ACCUM_TIME_NOT_MET 20023
#define DRV_NO_NEW_DATA 20024
#define DRV_PCI_DMA_FAIL 20025
#define DRV_SPOOLERROR 20026
#define DRV_SPOOLSETUPERROR 20027
#define DRV_FILESIZELIMITERROR 20028
#define DRV_ERROR_FILESAVE 20029

#define DRV_TEMPERATURE_CODES 20033
#define DRV_TEMPERATURE_OFF 20034
#define DRV_TEMPERATURE_NOT_STABILIZED 20035
#define DRV_TEMPERATURE_STABILIZED 20036
#define DRV_TEMPERATURE_NOT_REACHED 20037
#define DRV_TEMPERATURE_OUT_RANGE 20038
#define DRV_TEMPERATURE_NOT_SUPPORTED 20039
#define DRV_TEMPERATURE_DRIFT 20040

#define DRV_TEMP_CODES 20033
#define DRV_TEMP_OFF 20034
#define DRV_TEMP_NOT_STABILIZED 20035
#define DRV_TEMP_STABILIZED 20036
#define DRV_TEMP_NOT_REACHED 20037
#define DRV_TEMP_OUT_RANGE 20038
#define DRV_TEMP_NOT_SUPPORTED 20039
#define DRV_TEMP_DRIFT 20040

#define DRV_GENERAL_ERRORS 20049
#define DRV_INVALID_AUX 20050
#define DRV_COF_NOTLOADED 20051
#define DRV_FPGAPROG 20052
#define DRV_FLEXERROR 20053
#define DRV_GPIBERROR 20054
#define DRV_EEPROMVERSIONERROR 20055

#define DRV_DATATYPE 20064
#define DRV_DRIVER_ERRORS 20065
#define DRV_P1INVALID 20066
#define DRV_P2INVALID 20067
#define DRV_P3INVALID 20068
#define DRV_P4INVALID 20069
#define DRV_INIERROR 20070
#define DRV_COFERROR 20071
#define DRV_ACQUIRING 20072
#define DRV_IDLE 20073
#define DRV_TEMPCYCLE 20074
#define DRV_NOT_INITIALIZED 20075
#define DRV_P5INVALID 20076
#define DRV_P6INVALID 20077
#define DRV_INVALID_MODE 20078
#define DRV_INVALID_FILTER 20079

#define DRV_I2CERRORS 20080
#define DRV_I2CDEVNOTFOUND 20081
#define DRV_I2CTIMEOUT 20082
#define DRV_P7INVALID 20083
#define DRV_P8INVALID 20084
#define DRV_P9INVALID 20085
#define DRV_P10INVALID 20086

#define DRV_USBERROR 20089
#define DRV_IOCERROR 20090
#define DRV_VRMVERSIONERROR 20091
#define DRV_USB_INTERRUPT_ENDPOINT_ERROR 20093
#define DRV_RANDOM_TRACK_ERROR 20094
#define DRV_INVALID_TRIGGER_MODE 20095
#define DRV_LOAD_FIRMWARE_ERROR 20096
#define DRV_DIVIDE_BY_ZERO_ERROR 20097
#define DRV_INVALID_RINGEXPOSURES 20098
#define DRV_BINNING_ERROR 20099
#define DRV_INVALID_AMPLIFIER 20100

#define DRV_ERROR_NOCAMERA 20990
#define DRV_NOT_SUPPORTED 20991
#define DRV_NOT_AVAILABLE 20992

#define DRV_ERROR_MAP 20115
#define DRV_ERROR_UNMAP 20116
#define DRV_ERROR_MDL 20117
#define DRV_ERROR_UNMDL 20118
#define DRV_ERROR_BUFFSIZE 20119
#define DRV_ERROR_NOHANDLE 20121

#define DRV_GATING_NOT_AVAILABLE 20130
#define DRV_FPGA_VOLTAGE_ERROR 20131

#define DRV_OW_CMD_FAIL 20150
#define DRV_OWMEMORY_BAD_ADDR 20151
#define DRV_OWCMD_NOT_AVAILABLE 20152
#define DRV_OW_NO_SLAVES 20153
#define DRV_OW_NOT_INITIALIZED 20154
#define DRV_OW_ERROR_SLAVE_NUM 20155
#define DRV_MSTIMINGS_ERROR 20156

#define DRV_OA_NULL_ERROR 20173
#define DRV_OA_PARSE_DTD_ERROR 20174
#define DRV_OA_DTD_VALIDATE_ERROR 20175
#define DRV_OA_FILE_ACCESS_ERROR 20176
#define DRV_OA_FILE_DOES_NOT_EXIST 20177
#define DRV_OA_XML_INVALID_OR_NOT_FOUND_ERROR 20178
#define DRV_OA_PRESET_FILE_NOT_LOADED 20179
#define DRV_OA_USER_FILE_NOT_LOADED 20180
#define DRV_OA_PRESET_AND_USER_FILE_NOT_LOADED 20181
#define DRV_OA_INVALID_FILE 20182
#define DRV_OA_FILE_HAS_BEEN_MODIFIED 20183
#define DRV_OA_BUFFER_FULL 20184
#define DRV_OA_INVALID_STRING_LENGTH 20185
#define DRV_OA_INVALID_CHARS_IN_NAME 20186
#define DRV_OA_INVALID_NAMING 20187
#define DRV_OA_GET_CAMERA_TYPE_ERROR 20188
#define DRV_OA_MODE_ALREADY_EXISTS 20189
#define DRV_OA_STRINGS_NOT_EQUAL 20190
#define DRV_OA_NO_USER_DATA 20191
#define DRV_OA_VALUE_NOT_SUPPORTED 20192
#define DRV_OA_MODE_DOES_NOT_EXIST 20193

#define AC_ACQMODE_SINGLE 1
#define AC_ACQMODE_VIDEO 2
#define AC_ACQMODE_ACCUMULATE 4
#define AC_ACQMODE_KINETIC 8
#define AC_ACQMODE_FRAMETRANSFER 16
#define AC_ACQMODE_FASTKINETICS 32
#define AC_ACQMODE_OVERLAP 64

#define AC_READMODE_FULLIMAGE 1
#define AC_READMODE_SUBIMAGE 2
#define AC_READMODE_SINGLETRACK 4
#define AC_READMODE_FVB 8
#define AC_READMODE_MULTITRACK 16
#define AC_READMODE_RANDOMTRACK 32
#define AC_READMODE_MULTITRACKSCAN 64

#define AC_TRIGGERMODE_INTERNAL 1
#define AC_TRIGGERMODE_EXTERNAL 2
#define AC_TRIGGERMODE_EXTERNAL_FVB_EM 4
#define AC_TRIGGERMODE_CONTINUOUS 8
#define AC_TRIGGERMODE_EXTERNALSTART 16
#define AC_TRIGGERMODE_EXTERNALEXPOSURE 32
#define AC_TRIGGERMODE_INVERTED 0x40

// Deprecated for AC_TRIGGERMODE_EXTERNALEXPOSURE
#define AC_TRIGGERMODE_BULB 32

#define AC_CAMERATYPE_PDA 0
#define AC_CAMERATYPE_IXON 1
#define AC_CAMERATYPE_ICCD 2
#define AC_CAMERATYPE_EMCCD 3
#define AC_CAMERATYPE_CCD 4
#define AC_CAMERATYPE_ISTAR 5
#define AC_CAMERATYPE_VIDEO 6
#define AC_CAMERATYPE_IDUS 7
#define AC_CAMERATYPE_NEWTON 8
#define AC_CAMERATYPE_SURCAM 9
#define AC_CAMERATYPE_USBICCD 10
#define AC_CAMERATYPE_LUCA 11
#define AC_CAMERATYPE_RESERVED 12
#define AC_CAMERATYPE_IKON 13
#define AC_CAMERATYPE_INGAAS 14
#define AC_CAMERATYPE_IVAC 15
#define AC_CAMERATYPE_UNPROGRAMMED 16
#define AC_CAMERATYPE_CLARA 17
#define AC_CAMERATYPE_USBISTAR 18
#define AC_CAMERATYPE_SIMCAM 19
#define AC_CAMERATYPE_SCMOS 20
#define AC_CAMERATYPE_EXTREME 21

#define AC_PIXELMODE_8BIT 1
#define AC_PIXELMODE_14BIT 2
#define AC_PIXELMODE_16BIT 4
#define AC_PIXELMODE_32BIT 8

#define AC_PIXELMODE_MONO 0x000000
#define AC_PIXELMODE_RGB 0x010000
#define AC_PIXELMODE_CMY 0x020000

#define AC_SETFUNCTION_VREADOUT 0x01
#define AC_SETFUNCTION_HREADOUT 0x02
#define AC_SETFUNCTION_TEMPERATURE 0x04
#define AC_SETFUNCTION_MCPGAIN 0x08
#define AC_SETFUNCTION_EMCCDGAIN 0x10
#define AC_SETFUNCTION_BASELINECLAMP 0x20
#define AC_SETFUNCTION_VSAMPLITUDE 0x40
#define AC_SETFUNCTION_HIGHCAPACITY 0x80
#define AC_SETFUNCTION_BASELINEOFFSET 0x0100
#define AC_SETFUNCTION_PREAMPGAIN 0x0200
#define AC_SETFUNCTION_CROPMODE 0x0400
#define AC_SETFUNCTION_DMAPARAMETERS 0x0800
#define AC_SETFUNCTION_HORIZONTALBIN 0x1000
#define AC_SETFUNCTION_MULTITRACKHRANGE 0x2000
#define AC_SETFUNCTION_RANDOMTRACKNOGAPS 0x4000
#define AC_SETFUNCTION_EMADVANCED 0x8000
#define AC_SETFUNCTION_GATEMODE 0x010000
#define AC_SETFUNCTION_DDGTIMES 0x020000
#define AC_SETFUNCTION_IOC 0x040000
#define AC_SETFUNCTION_INTELLIGATE 0x080000
#define AC_SETFUNCTION_INSERTION_DELAY 0x100000
#define AC_SETFUNCTION_GATESTEP 0x200000
#define AC_SETFUNCTION_TRIGGERTERMINATION 0x400000

// Deprecated for AC_SETFUNCTION_MCPGAIN
#define AC_SETFUNCTION_GAIN 8
#define AC_SETFUNCTION_ICCDGAIN 8

#define AC_GETFUNCTION_TEMPERATURE 0x01
#define AC_GETFUNCTION_TARGETTEMPERATURE 0x02
#define AC_GETFUNCTION_TEMPERATURERANGE 0x04
#define AC_GETFUNCTION_DETECTORSIZE 0x08
#define AC_GETFUNCTION_MCPGAIN 0x10
#define AC_GETFUNCTION_EMCCDGAIN 0x20
#define AC_GETFUNCTION_HVFLAG 0x40
#define AC_GETFUNCTION_GATEMODE 0x80
#define AC_GETFUNCTION_DDGTIMES 0x0100
#define AC_GETFUNCTION_IOC 0x0200
#define AC_GETFUNCTION_INTELLIGATE 0x0400
#define AC_GETFUNCTION_INSERTION_DELAY 0x0800
#define AC_GETFUNCTION_GATESTEP 0x1000
#define AC_GETFUNCTION_PHOSPHORSTATUS 0x2000
#define AC_GETFUNCTION_MCPGAINTABLE 0x4000

// Deprecated for AC_GETFUNCTION_MCPGAIN
#define AC_GETFUNCTION_GAIN 0x10
#define AC_GETFUNCTION_ICCDGAIN 0x10

#define AC_FEATURES_POLLING 1
#define AC_FEATURES_EVENTS 2
#define AC_FEATURES_SPOOLING 4
#define AC_FEATURES_SHUTTER 8
#define AC_FEATURES_SHUTTEREX 16
#define AC_FEATURES_EXTERNAL_I2C 32
#define AC_FEATURES_SATURATIONEVENT 64
#define AC_FEATURES_FANCONTROL 128
#define AC_FEATURES_MIDFANCONTROL 256
#define AC_FEATURES_TEMPERATUREDURINGACQUISITION 512
#define AC_FEATURES_KEEPCLEANCONTROL 1024
#define AC_FEATURES_DDGLITE 0x0800
#define AC_FEATURES_FTEXTERNALEXPOSURE 0x1000
#define AC_FEATURES_KINETICEXTERNALEXPOSURE 0x2000
#define AC_FEATURES_DACCONTROL 0x4000
#define AC_FEATURES_METADATA 0x8000
#define AC_FEATURES_IOCONTROL 0x10000
#define AC_FEATURES_PHOTONCOUNTING 0x20000
#define AC_FEATURES_COUNTCONVERT 0x40000
#define AC_FEATURES_DUALMODE 0x80000

#define AC_EMGAIN_8BIT 1
#define AC_EMGAIN_12BIT 2
#define AC_EMGAIN_LINEAR12 4
#define AC_EMGAIN_REAL12 8


#ifdef __cplusplus
}
#endif

#endif
