#pragma once
#include <windows.h> 
#include <tlhelp32.h> 
#include <tchar.h> 
#include <iostream>

//stole this error printing function from microsofts tlHelp32 documnetation page 
//fuck knows what it does, cba to look at it
void printError( const TCHAR* msg );

//gets the module base address of a specified module and processID
uintptr_t getModuleBaseAddress( const DWORD processID , const wchar_t* moduleName );
