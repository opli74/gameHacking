#pragma once
#include <windows.h> 
#include <tlhelp32.h> 
#include <tchar.h> 
#include <iostream>
#include <vector>

//stole this error printing function from microsofts tlHelp32 documnetation page 
//fuck knows what it does, cba to look at it
void printError( const TCHAR* msg );

//returns the id of a process name
uintptr_t getProcessId( const wchar_t* processName );

//gets the module base address of a specified module and processID
uintptr_t getModuleBaseAddress( const uintptr_t processID , const wchar_t* moduleName );

//returns the end variable of a ptr chain
uintptr_t getPointerChain( HANDLE hProcess , uintptr_t ptr , std::vector<unsigned int> offsets );


//functions for when already inside the process
namespace internal
{
	uintptr_t getPointerChain( uintptr_t ptr , std::vector<unsigned int> offsets );
};

