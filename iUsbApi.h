/*********************************************************************
 *
 *                  iUSBDAQ Example Code Version 1.00
 *
 *********************************************************************
 * FileName:        iUsbApi.h
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
#ifndef _IUSBDAQAPI_H_
#define _IUSBDAQAPI_H_

#include "stdafx.h"
#include <windows.h>
#include "iUSBDAQ.h"

int (*IUSBDAQ_EnumerateDevice)(
				unsigned long DevType, 
				unsigned long* Count);		// output, number of iUSBDAQ devices found

int (*IUSBDAQOpen)(unsigned long  DevType,  // Input
                unsigned long DevIndex,		// Input
                DevSession* Session);		// Output, pointer to a IUSBDAQ session structure


int (*IUSBDAQReadChannels)(
				DevSession* Session,		// Input 
				int StartCh,				// Input
				int NofChs,					// Input
				int InputRange,				// not used
				float *Voltages,			// output buffer, user must allocate enough memory
				int * reserved);		// not used


int (*IUSBDAQStartDaqStream) (
				DevSession* Session,		// Input
				int StartCh,				// Input
				int NofChs,					// Input
				int InputRange,				// Input
				int ScanRate,				// Input
				int * ActualRate,			// Output, the actual sampling rate committed to the iUSBDAQ
				int withExternalTrigger);	// Input

int (*IUSBDAQStopDaqStream) (
				DevSession* Session);		// Input, the DevSession returned by IUSBDAQOpen

// if the sampleNo is 0, then the function will return anything that has been collected so far
int (*IUSBDAQReadStream) (
				DevSession* StreamSession, 
				int NoScans, 
				int timeout, 
				float* buffer, 
				int* actualSamples);

BOOL (*IUSBDAQClose)(DevSession* Session);	// Input, the DevSession returned by IUSBDAQOpen


#endif