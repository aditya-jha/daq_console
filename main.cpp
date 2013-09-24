/*********************************************************************
 *
 *        iUSBDAQ Console Resistance vs Time Code Version 1.00
 *
 *********************************************************************
 * FileName:        main.cpp
 * Author:			Aditya Jha
 * Dependencies:    None
 * Compiler:        Visual C++ 2010 Express
 *********************************************************************/

#include "stdafx.h"
#include <cstdio>
#include <iostream>
#include "iUsbApi.h"
#include <string>
#include <cstring>
#include <ctime>

using namespace std;

HINSTANCE libHandle;
DevSession dev;
unsigned long devcnt=0;
int rate = 0;


bool LoadDLL( void );
int PrintError( int code );
int help( void );
int call_function( string input );
int stop( void );
int exit_func( void );
int close( void );
int stop_func( void );
int start( void );

int main( int argc, char* args[] )
{
	BOOLEAN bQuit;
    char selection;
    bQuit = false;

	string input;

    // Load DLL when it is necessary, i.e. on start-up!
    if (!LoadDLL())
	{
		printf("Cannot Load DLL file. Reinstall the Driver.\r\n");
		scanf("%c",&selection);
		return -1;
	}

	cout << "Welcome to DAQ\n";
	cout << "--------------\n\n";
	cout << "To see the list of valid commands type help\n";
	cout << "If you know the commands then start away\n\n";

	cout << ">> ";
	cin  >> input;

	while( true )
	{
		if( input == "exit" || input == "EXIT" )
		{
			int et = exit_func();
			if( et == 0 ) break;
			else {
				cout << "\nEnter command again\n\n";
				cout << ">> ";
				cin  >> input;
				continue;
			}
		}
		
		else 
		{
			int rt = call_function( input );
			if( rt == -1 )
			{
				cout << "Unable to execute command.\n\n";
			}
		}

		cout << ">> ";
		cin  >> input;
	}

	return 0;
}

int call_function( string input ) 
{
	if( input == "help" || input == "HELP" )
	{
		return help();
	}
	else if( input == "stop" || input == "STOP" )
	{
		return stop_func();
	}
	else if( input == "start" || input == "START" )
	{
		return start();
	}
	else {
		cout << "\nNot a valid command. See help for the set of valid commands\n\n";
		return -1;
	}
}

int start( void )
{
	devcnt = 0;
	int rt = IUSBDAQ_EnumerateDevice(0, &devcnt);
	if ( rt != iUSBDAQ_NOERROR)
	{
		return PrintError(rt);				
	}

	printf("\nFound %d of iUSBDAQ connected, reading data from first iUSBDAQ\n", devcnt);
	
	rt = IUSBDAQOpen(0, 0, &dev);
	if ( rt != iUSBDAQ_NOERROR)
	{
		return PrintError(rt);				
	}	

	rate = 0;
	IUSBDAQStartDaqStream(&dev, 0, 8, 0, 2000, &rate, 0); 
	
}

int stop_func( void )
{
	int rt = stop();
	if( rt == 0 ) 
	{
		cout << "\nStopped Streaming.\n\n";
	}
	else
	{
		cout << "\nUnable to Stop Streaming.\n";
		return -1;
	}
	int cl = close();
	if( cl != 0 )
	{
		cout << "Unable to Close Session.\n";
		return -1;
	}
	else
	{
		cout << "Clossed Current Session\n\n";
		Sleep( 500 );
		return 0;
	}
}

int exit_func( void )
{
	string sure;
	cout << "Exiting will stop streaming and Close current session.\n";
	cout << "Are you sure (Y/N): ";
	cin  >> sure;
	if( sure[0] == 'Y' || sure[0] == 'y')
	{
		int st = stop();
		if( st != 0 ) 
		{
			cout<< "Unable to stop Streaming.\n";
			return -1;
		}
		int cl = close();
		if( cl != 0 )
		{
			cout << "Unable to Close Session.\n";
			return -1;
		}

		cout << "Stopping Streaming\n";
		cout << "Closing current session\n";
		Sleep( 1000 );
		return 0;
	}
	else {
		return -1;
	}
}

int help( void ) 
{
	cout << "\nWelcome to help menu\n";
	cout << "---------------------\n\n";
	cout << "COMMAND		RESULT\n";
	cout << "---------------------------\n\n";
	cout << "start		Connects device and starts reading\n";
	cout << "stop		Stops reading and closses current session\n";
	cout << "exit		Exits the software\n";
	cout << "help		Displays the list of supported commands\n";
	cout << endl;

	return 0;
}

int stop( void ) 
{
	IUSBDAQStopDaqStream(&dev);	// end of streaming section
	return 0;
}

int close( void )
{
	IUSBDAQClose(&dev);
	return 0;
}

bool LoadDLL(void)
{
    libHandle = NULL;
    libHandle = LoadLibrary("iUSBDAQ");
    if(libHandle == NULL)
    {
        printf("Error loading iUSBDAQ.dll\r\n");
		return false;
    }
    else
    {
		IUSBDAQ_EnumerateDevice=(int(*)(unsigned long, unsigned long*))\
                    GetProcAddress(libHandle,"iUSBDAQ_EnumerateDev");
        IUSBDAQOpen=(int(*)(unsigned long,unsigned long,DevSession*))\
                    GetProcAddress(libHandle,"iUSBDAQ_OpenDevice");
        IUSBDAQReadChannels=(int(*)(DevSession*,int,int,int,float*,int*))\
                    GetProcAddress(libHandle,"iUSBDAQ_ReadMultiAnalogIn");
        IUSBDAQStartDaqStream=(int(*)(DevSession*,int,int,int,int,int*,int))\
                    GetProcAddress(libHandle,"iUSBDAQ_AIStartStream");
        IUSBDAQStopDaqStream=(int(*)(DevSession*))\
                    GetProcAddress(libHandle,"iUSBDAQ_AIStopStream");
        IUSBDAQReadStream=(int(*)(DevSession*,int,int,float*,int*))\
                    GetProcAddress(libHandle,"iUSBDAQ_AIGetScans");
        IUSBDAQClose=(int(*)(DevSession*))GetProcAddress(libHandle,"iUSBDAQ_ReleaseDevice");


		if((IUSBDAQ_EnumerateDevice == NULL) || (IUSBDAQOpen == NULL) ||
            (IUSBDAQReadChannels == NULL) || (IUSBDAQStartDaqStream == NULL) ||
            (IUSBDAQStopDaqStream == NULL) || (IUSBDAQReadStream == NULL) ||
            (IUSBDAQClose == NULL))
		{
			printf("GetProcAddress Error\r\n");
			return false;
		}
    }
	return true;
}//end LoadDLL

int PrintError(int code)
{
	char a;
	printf("Error: %s\n", ErrorDesc[code]);
	return -1;
}
