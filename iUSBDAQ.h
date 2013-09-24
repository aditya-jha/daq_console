/*********************************************************************
 *
 *                  iUSBDAQ Example Code Version 1.00
 *
 *********************************************************************
 * FileName:        iUSBDAQ.h
 * Dependencies:    None
 * Compiler:        C++
 * Company:         Copyright (C) 2005 by HYTEK Automation, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by HYTEK Automation Incorporated
 * (the “Company”) for its iUSBDAQ. The software is owned by the 
 * Company, and is protected under applicable copyright laws. 
 * All rights are reserved.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * Date:       08/08/2005    Original version 1.00 completed
 ********************************************************************/

/*********************************************************************
 * Please see the iUSBDAQ user guider for the documentation of the 
 * interface functions defined in this file.
 *
 ********************************************************************/

#ifndef IUSBDAQ_EXTERN_C
    #ifdef __cplusplus
        #define IUSBDAQ_EXTERN_C extern "C"
        #define IUSBDAQ_DEFAULT(val) = val
	#else
        #define IUSBDAQ_EXTERN_C
        #define IUSBDAQ_DEFAULT(val)
    #endif
#endif

#ifndef IUSBDAQ_DLL_ENTRY 
	#define IUSBDAQ_DLL_ENTRY __declspec(dllexport)
#endif

#ifndef IUSBDAQ_API
    #define IUSBDAQ_API IUSBDAQ_EXTERN_C IUSBDAQ_DLL_ENTRY
#endif

//IUSBDAQ_API int fnIUSBDAQ(void);

typedef struct 
{
	int					DevIndex;
	int					DevInstance;
	unsigned long		DevType;
	HANDLE				iSession1;//Out Pipe, write
	HANDLE				iSession2;//In Pipe, read	
} DevSession;



enum ERRORS 
{
	iUSBDAQ_NOERROR=0,
	iUSBDAQ_Unknown=1,
	iUSBDAQ_InvalidDevIndex,//device index exceed the max device number of that type
	iUSBDAQ_WrongType,//No such iUSBDAQ module type or wrong device type
	iUSBDAQ_HandleError,//Open device session error
	iUSBDAQ_DeviceNotAvailable,//device maybe used by other application or session already
								//been opened.
	iUSBDAQ_WriteError,//Write error, it could be the device disconnected
	iUSBDAQ_ReadError,//Read error, it could be the device disconnected
	iUSBDAQ_NULLSession,//NULL session
	iUSBDAQ_RcvLength,   // Partially failed, incorrect receive length
	iUSBDAQ_InvalidByteCount,//the byte count number is invalid
	iUSBDAQ_InvalidChannel,//channel number is incorrect
	iUSBDAQ_InvalidNofChs,//number of channels invalid
	iUSBDAQ_InputRange, //Wrong input voltage range
	iUSBDAQ_InvalidDutyCycle,//Duty cycle value should be between 0- 1023
	iUSBDAQ_InvalidDIOWrite,//The IO channel is not an output channel
	iUSBDAQ_InvalidPeriod,//wrong PWM period

	iUSBDAQ_TooManyScans, // iUSBDAQ_AIGetScans function asks too many scans to get, max 6400/NrOfChannels
	iUSBDAQ_ReadSamplesTimeout,
	iUSBDAQ_LowNofSamples,
	iUSBDAQ_HighNofSamples,
	iUSBDAQ_ReadWritePassAllowedAddr,
	iUSBDAQ_PMW_Period_too_big,
	iUSBDAQ_PMW_Period_too_small,
};

char ErrorDesc[][256]=
{
		"No Error",
		"Unknown Error",
		"device index exceed the max device number of that type",
		"No such iUSBDAQ module type or wrong device type",
		"Open device session error",
		"device maybe used by other application or session already been opened",
		"Write error, it could be the device disconnected",
		"Read error, it could be the device disconnected",
		"NULL session",
		"Partially failed, incorrect receive length",
		"the byte count number is invalid or exceed the eeprom size",
		"channel number is incorrect",
		"number of channels invalid",
		"Wrong input voltage range",
		"Duty cycle value should be between 0- 1023",
		"The IO channel is not an output channel",
		"wrong PWM period",
		"Too many data sets requested, max. allowed number of samples is 3200",
		"Timeout when waiting for required number of samples",
		"At least 128 samples required",
		"Exceed maximum data throughput",
		"Try to write/read data beyond allowed EEPROM boundary",
		"PWM period parameter is too big, the max. allowed period is 333 us",
		"PWM period parameter is too small, the min. allowed period is 3 us"
};

IUSBDAQ_API int iUSBDAQ_EnumerateDev (unsigned long DevType, unsigned long* Count);

IUSBDAQ_API int iUSBDAQ_OpenDevice (unsigned long DevType, unsigned long DevIndex, DevSession* Session);

IUSBDAQ_API int iUSBDAQ_ReleaseDevice (DevSession* Session);

IUSBDAQ_API int iUSBDAQ_GetDeviceSerialNo (DevSession* Session, unsigned long* SerialNumber);


IUSBDAQ_API int iUSBDAQ_ReadSingleAnalogIn ( DevSession* Session, int Channel, 
				int InputRange, float * Voltage, int * reserved);

IUSBDAQ_API int iUSBDAQ_ReadCounter (DevSession* Session, int Reset, unsigned int *Count);

IUSBDAQ_API int iUSBDAQ_DIO (DevSession* Session, BYTE Port1to8dir, BYTE Port9to16dir,
								BYTE *Port1to8state, BYTE *Port9to16state, int updateDio);

IUSBDAQ_API int iUSBDAQ_ReadMultiAnalogIn(DevSession* Session, int StartCh, int NofChs, int InputRange,
										  float *Voltages, int * reserved);
IUSBDAQ_API float iUSBDAQ_BitsToVolt (int Bits);

IUSBDAQ_API int iUSBDAQ_VoltToBits ( float Voltage);

IUSBDAQ_API int iUSBDAQ_AIStartStream (DevSession* Session, int StartCh, int NofChs, 
									   int InputRange, int ScanRate, int * ActualRate,
									   int withExternalTrigger);

IUSBDAQ_API int iUSBDAQ_AIStopStream (DevSession* Session);

IUSBDAQ_API int iUSBDAQ_AIGetScans (DevSession* StreamSession, int NoScans, 
                                            int timeout, float* buffer, int* actualSamples);

IUSBDAQ_API int iUSBDAQ_GetCredits (DevSession* Session, BYTE* data);

IUSBDAQ_API DWORD iUSBDAQ_GetDLLVersion ();

IUSBDAQ_API int iUSBDAQ_GetFirmwareVersion (DevSession* Session, DWORD* Version);

IUSBDAQ_API void iUSBDAQ_GetErrorDes (int ErrorCode, char* ErrorString);//max 256 characters

IUSBDAQ_API int iUSBDAQ_Reset (DevSession* Session);

IUSBDAQ_API int iUSBDAQ_PWMOut (DevSession* Session, BYTE Channel, int DutyCycle, int Period);

IUSBDAQ_API int iUSBDAQ_STOPPWM (DevSession* Session, int Channel);

IUSBDAQ_API int iUSBDAQ_ReadMemory (DevSession* Session, BYTE ByteCount, BYTE Address, BYTE* Data);

IUSBDAQ_API int iUSBDAQ_WriteMemory (DevSession* Session, BYTE ByteCount, BYTE Address, BYTE* Data);

IUSBDAQ_API int iUSBDAQ_ReadIUSB_DEVID (DevSession* Session, BYTE *IUSB_DevID);
								
