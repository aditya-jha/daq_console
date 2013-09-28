/************************************************************************
*																																				*
*	FILE: main.cpp
*	AUTHOR: Aditya Jha
*                  
*
************************************************************************/

#include "stdafx.h"
#include "GNUplot.h"
#include "iUsbApi.h"
using namespace std;

HINSTANCE libHandle;


int rate = 0,res=0;
float voltage=0.00, resistance=0.00;
int no_of_channels=0;
int channels[8];

bool LoadDLL( void );
int PrintError( int code );
void DisplayResult(float* buf, int numberOfCh, int st, FILE **f);


long long g_clockFrequency;
/* A run once routine to obtain the processor clock speed. This function
*	returns non-zero if it is successful */
inline DWORD getCpuClockFrequencyMHz(void){
	DWORD BufSize = _MAX_PATH;
	DWORD dwMHz = _MAX_PATH;
	HKEY hKey;
	long lError = RegOpenKeyEx(HKEY_LOCAL_MACHINE,	//Section of registry where processor clock speed located
		TEXT("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"),	//location of processor clock speed value
		0,	//must be 0
		KEY_READ,	//Read only (to read the value of the key)
		&hKey	//store the registry key handle here
		);
	if(lError != ERROR_SUCCESS)
	{// if the key is not found, tell the user why:
		TCHAR Buffer[_MAX_PATH] = TEXT("Could not find processor clock-speed registry key\n");
		MessageBox(NULL, Buffer, TEXT("Error"), MB_OK);
		return(0);	//Error return
	}

	// query the key
	RegQueryValueEx(hKey, TEXT("~MHz"), NULL, NULL, (LPBYTE) &dwMHz, &BufSize);
	return dwMHz;
}
inline long long GetCpuClocks(void)
{
	// Counter
	struct { int low, high; } counter;

	// Use RDTSC instruction to get clocks count
	__asm push EAX
	__asm push EDX
	__asm __emit 0fh __asm __emit 031h // RDTSC
	__asm mov counter.low, EAX
	__asm mov counter.high, EDX
	__asm pop EDX
	__asm pop EAX

	// Return result
	return *(__int64 *)(&counter);
}
inline void pressEnter(void){
#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
	std::cout<<"Press any key to continue..."<<std::endl;
	_getch();
#endif
}


int main ( int argc, char** argv )
{

	BOOLEAN bQuit;
    char selection;
    bQuit = false;

	//string input;
	
	FILE *fp;
	fp = fopen( "daq.csv" , "w" );
	if( fp == NULL) 
	{
		cout << "Cannot open file" <<endl;
		exit(1);
	}

	//close file
	fclose(fp);

	//fprintf( fp, "Hell Yeah\n");

    // Load DLL when it is necessary, i.e. on start-up!
    if (!LoadDLL())
	{
		printf("Cannot Load DLL file. Reinstall the Driver.\r\n");
		scanf("%c",&selection);
		return -1;
	}
	
	//cout << "\nDLL Successfully Loaded\n\n";
	
	unsigned long devcnt=0;
	// Device type for iUSBDAQ is 0 according to user manual
	int rt = IUSBDAQ_EnumerateDevice(0, &devcnt);
	if ( rt != iUSBDAQ_NOERROR)
	{
		return PrintError(rt);				
	}
	
	if( devcnt == 0 ) {
		printf("Found %d of iUSBDAQ connected, reading data from first iUSBDAQ\n", devcnt);
	}
	
	DevSession dev;
	rt = IUSBDAQOpen(0, 0, &dev);
	if ( rt != iUSBDAQ_NOERROR)
	{
		return PrintError(rt);	
	}	
	

	cout << "Welcome to DAQ\n";
	cout << "--------------\n\n";
	
	cout << "Number of channels to scan: ";
	cin  >> no_of_channels;
	cout << endl;

	cout << "\nThe channels\n";
	for(int i=0; i<no_of_channels; i++) 
	{
		cout << "	AI: ";
		cin  >> channels[i];
	}
	cout << endl;
	
	
	cout << "Input Voltage: ";
	cin >> voltage;

	cout << endl << "Resistance: ";
	cin  >> resistance;
	
	// gnuplot
	g_clockFrequency = getCpuClockFrequencyMHz()*(1000000);
	long long startTime, endTime;

	/* Create gnuplotinterface class object */
	GNUPlotInterface gpi;

	/* Create the read/write pipes */
	gpi.CreatePipes();

	/* Start the gnuplot.exe process with rerouted input/output using pipes */
	gpi.StartProcess();
	/*  This is the intialization of realtime plotting as well as title and 
	*	line type. Be sure to use \" to send the " character. */
	string gnuplotOutput = gpi.Init("lines","GNUplot Pipe Interface Test");

	cout<<"Initialization - gnuplot output:\n"<<gnuplotOutput.c_str()<<std::endl;

	int i=0;
	string input;

	while(true) {
		float value[8];						
		IUSBDAQReadChannels(&dev, channels[0], no_of_channels, 0, value, 0); 
		
		FILE *fp = fopen( "daq.csv" , "a+" );

		DisplayResult( value, no_of_channels, channels[0], &fp);		
		
		fclose( fp );
		
		double dVal = value[channels[0]];
		CHAR chVal[20];

		sprintf(chVal,"%d",i); input.append(chVal); input.append(" ");
		sprintf(chVal,"%.5f",dVal);	input.append(chVal); input.append("\n");
		
		gnuplotOutput = gpi.Plot(input.c_str(),0);
		
		if(gnuplotOutput.length() > 0)
			std::cout<<gnuplotOutput.c_str()<<std::endl;

		i++;

		Sleep( 500 );
	}

	gpi.CloseAll();
	
	system("pause");
	return 0;
}

bool LoadDLL(void)
{
	LPCWSTR str = TEXT("iUSBDAQ");
    libHandle = NULL;
    libHandle = LoadLibrary(str);
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
	printf("Press any key to exit\r\n");
	scanf("%c", &a);
	return -1;
}

void DisplayResult(float* value, int numberOfCh, int st, FILE **f)
{
	for (int i = st; i < st+numberOfCh; i++)
		{
			cout << setw(5) << "AI"<<i<< " ";
		}
		cout<<endl;

		for (int i = st; i < st+numberOfCh; i++)
		{
			printf(" %.3f ", value[i]);
		}
		cout << endl << endl;

for (int i = st; i < st+numberOfCh; i++)
		{
			fprintf( *f, "%.3f,", value[i] );
		}
		fprintf( *f, "\n");

}