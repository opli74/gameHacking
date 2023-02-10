#include "Header.h"

void printError(const TCHAR* msg)
{
	DWORD eNum;
	TCHAR sysMsg[256];
	TCHAR* p;

	eNum = GetLastError();
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, eNum,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		sysMsg, 256, NULL);

	// Trim the end of the line and terminate it with a null
	p = sysMsg;
	while ((*p > 31) || (*p == 9))
		++p;
	do { *p-- = 0; } while ((p >= sysMsg) &&
		((*p == '.') || (*p < 33)));

	// Display the message
	_tprintf(TEXT("\n  WARNING: %s failed with error %d (%s)"), msg, eNum, sysMsg);
}

uintptr_t getProcessId( const wchar_t* processName )
{
	HANDLE hProcessSnapShot = INVALID_HANDLE_VALUE;
	PROCESSENTRY32 pe32;

	hProcessSnapShot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS , NULL );

	if ( hProcessSnapShot == INVALID_HANDLE_VALUE )
	{
		printError( TEXT( "CreateToolhelp32Snapshot (of processes)" ) );
		return( FALSE );
	}

	//set size of processentry structure
	pe32.dwSize = sizeof( PROCESSENTRY32 );

	if ( !Process32First( hProcessSnapShot , &pe32 ) )
	{
		printError( TEXT( "Process32First" ) );
		CloseHandle( hProcessSnapShot );
		return( FALSE );
	}

	do
	{

		if ( !_wcsicmp( pe32.szExeFile , processName ) )
		{
			CloseHandle( hProcessSnapShot );
			return pe32.th32ProcessID;
		}

	} while ( Process32Next( hProcessSnapShot , &pe32 ) );

	CloseHandle( hProcessSnapShot );
	return ( FALSE );
}

uintptr_t getModuleBaseAddress( const DWORD processID , const wchar_t* moduleName )
{
	HANDLE hModuleSnapShot = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;

	//gets snapshot of all modules depending on the bit of the process. 64-bit process can get 32-bit module using TH32CS_SNAPMODULE32
	hModuleSnapShot = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32 , processID );


	//invalid handle thingy is returned when snapshot has failed, as stated in the MS docs
	if ( hModuleSnapShot == INVALID_HANDLE_VALUE )
	{
		printError( TEXT( "CreateToolhelp32Snapshot (of modules)" ) );
		return( FALSE );
	}

	//set size of structure
	me32.dwSize = sizeof( MODULEENTRY32 );

	//another simple ass error checking, this time snapshot object needs to be destroyed
	if ( !Module32First( hModuleSnapShot , &me32 ) )
	{
		//if print error shows ERROR_NO_MORE_FILES fucking modules dont exist!
		printError( TEXT( "Module32First" ) );  // Show cause of failure    
		CloseHandle( hModuleSnapShot ); // Destroy object before leaving scope
		return ( FALSE );
	}

	//simple do-while to loop over all modules in snapshot using Module32Next
	do
	{
		//wide char compare cuz unicode innit, if value is 1 ( TRUE ) just fucking destroy snapshot and returns module Id
		if ( !_wcsicmp( me32.szModule , moduleName ) )
		{
			CloseHandle( hModuleSnapShot );
			return uintptr_t( me32.th32ModuleID );
		}

	} while ( Module32Next( hModuleSnapShot , &me32 ) );
	
	//Clearly dumbass do-while loop did not find, prolly an incorrect module name 
	std::cout << "Could not find module: ";
	std::wcout << moduleName;
	std::cout << ". Make sure module name is correct dumbass!" << std::endl;
	CloseHandle( hModuleSnapShot );
	return( FALSE ); 
}

int main()
{
	//Gets the module base address of ac_client.exe of process ID 36812
	uintptr_t moduleBaseAddress = getModuleBaseAddress( DWORD( 36812 ) , L"ac_clent.exe" );

	return 0;
}